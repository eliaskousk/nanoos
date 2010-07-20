// ata pio driver
// no DMA no Interrupt
// for now
#include "low-io.h"
#include "idt.h"
#include "irq.h"
#include "OStream.h"
#include "IStream.h"
#include "timer.h"
#include "pci.h"
#include "ide.h"
#include "string.h"
using namespace String;
unsigned char buff[4096]; // ATAPI needs a 4096 byte buffer. 
                            // ATA hd can work with 512 bytes
channel ctrl_channel[2];
int ide_drv[4];
//disk *disks[4]={NULL};//*ide0,*ide1,*ide2,*ide3; //maximum 4 physical disks on 2 IDE controller

IDEdrive *ide_disks[4]={NULL};


#define IDENTIFY_TEXT_SWAP(field,size) \
    \
    ({ \
	unsigned char tmp; \
        \
        for (int i = 0; i < (size); i+=2) \
        { \
	    tmp = (field)[i]; \
	    (field)[i]   = (field)[i+1]; \
	    (field)[i+1] = tmp; \
	} \
    })

//==============Helper Functions=======================
//=====================================================

// depending on the second parameter sz
// reads a byte, word or dword from 
// port
unsigned int pio_inport( unsigned short port , size_t sz)
{
	switch(sz)
	{
		case 1: return inportb(port);
		case 2: return inportw(port);
		case 4: return inportl(port);
		default: return 0;
	}
}
// depending on the 3rd parameter sz
// sends a val byte/word/dword to
// port
void pio_outport(unsigned short port, unsigned int val, size_t sz)
{
	switch(sz)
	{
		case 1: outportb(port,(unsigned char)val); break;
		case 2: outportw(port,(unsigned short)val); break;
		case 4: outportl(port,(unsigned int)val); break;
	}
}
// reads single byte from port and returns it
unsigned char pio_inbyte(unsigned short port)
{
	return (unsigned char)pio_inport(port,1);
}
// reads a word(2byte) from port and returns it
unsigned short pio_inword(unsigned short port)
{
	return (unsigned short)pio_inport(port,2);
}
// reads a dword(4-byte) from port and returns it
unsigned int pio_indword(unsigned short port)
{
	return (unsigned int)pio_inport(port,4);
}
// sends a byte value val to port
void pio_outbyte(unsigned short port,unsigned char val)
{
	pio_outport(port,val,1);
}
// sends a short value val to port
void pio_outword(unsigned short port,unsigned short val)
{
	pio_outport(port,val,2);
}
// sends a int(4-byte) val to port
void pio_outdword(unsigned short port,unsigned int val)
{
	pio_outport(port,val,4);
}
// reads count number of bytes from port to buffer
void pio_rep_inb(unsigned short port, unsigned char *buffer, unsigned int count)
{
	insb(port,buffer,count);
}
// reads count number of shorts from port to buffer
void pio_rep_inw(unsigned short port, unsigned short *buffer, unsigned int count)
{
	insw(port,buffer,count);
}
// reads count number of ints from port to buffer
void pio_rep_indw(unsigned short port, unsigned int *buffer, unsigned int count)
{
	insdw(port,buffer,count);
}
// stop sending interrupts
// this should be called after selecting a drive 
void stop_ata_intr(unsigned short ctrl_port)
{
	pio_outbyte(ctrl_port,(0x01<<1));
}
// start sending interrupts

// reset the drives on this controller
// call this if some drive behave insane
// or in the begining of driver
void reset_controller(unsigned short ctrl_port)
{
	pio_outbyte(ctrl_port,(0x01<<2));
}
unsigned char pio_get_status(unsigned short port)
{
	return pio_inbyte(port+STATUS_REG);
}
unsigned char pio_get_astatus(unsigned short port)
{
	return pio_inbyte(port);
}
// this will function will block until the status bit is clear 
void pio_wait_busy(unsigned short port)
{
	// should have a delay here !!! guess it will work
	for(int i=0;i<4;i++)
		pio_inbyte(port+0x200);
	while(pio_get_status(port)&STA_BSY);
}
bool pio_wait_ready(unsigned short port,bool nodata)
{
	TIMER *tmr = TIMER::Instance();
	unsigned int ctr=tmr->get_ticks();
	while (true)
    	{
		unsigned char status = inportb(port + STATUS_REG);
		if (!(status & STA_BSY) && (status & STA_DRQ || nodata))
		{
			break;
		}
		if(tmr->get_ticks()-ctr>200) // 200ms
			return false;
	}
	return true;
}
void pio_ata_delay_400ns(unsigned short alt_ctrl_reg)
{
	for(int i=0;i<4;i++)
		inportb(alt_ctrl_reg);
}

//=================================================

// this function will check a master drive in a channel
// return true if found
// return false if not
// takes the base port for the channel
bool detect_master(unsigned short port)
{
	int tmp;
	outportb(port + DRV_HD_REG, 0x40|MASTER);	// Set drive
	pio_wait_busy(port);	
	tmp = inportb(port+STATUS_REG);	// Read status
	if (tmp & STA_DRDY)
		return true;
	else
		return false;
}
bool detect_slave(unsigned short port)
{
	int tmp;
	outportb(port + DRV_HD_REG, 0x40|SLAVE);	// Set drive
	pio_wait_busy(port);	
	tmp = inportb(port+STATUS_REG);	// Read status
	if (tmp & STA_DRDY)
		return 1;
	else
		return 0;
}
// checks if a ATA controller is present or not
// really not needed if we have a PCI system and we detected the the PCI-IDE controller
bool detect_cntrlr(unsigned short port)
{
	unsigned char temp1,temp2,stat,timeout=255;
	TIMER *tmr=TIMER::Instance();
	outportb(port+LBA_LOW_REG,0x55);
	outportb(port+LBA_MID_REG,0xAA);
	temp1=inportb(port+LBA_LOW_REG);
	temp2=inportb(port+LBA_MID_REG);
	if((temp1==0x55)&&(temp2==0xAA))
	{
		outportb(port+DEV_CTRL_REG,0x06);	//soft reset ????	
		my_timer->sleep(1);
		outportb(port + DEV_CTRL_REG, 0x00);
		tmr->sleep(1);
		//while (!(inportb(port+STATUS_REG) & STA_DRDY));
		//qemu needs bellow vvvv		
		while(timeout)
		{
			stat=ata_read_status(port);
			if(!stat) return true;
			if(stat & ((STA_DRDY|STA_BSY)==STA_DRDY)) return true;
			stat=ata_read_alt_status(port);
			if(!stat) return true;
			if(stat & ((STA_DRDY|STA_BSY)==STA_DRDY)) return true;
			timeout--;
			tmr->sleep(1);
		}
		//qemu needs ^^^^
		//bochs needs this vvvv
		while (!(inportb(port+STATUS_REG) & STA_DRDY));
		return true;
		//bochs needs ^^^^
	}
	//else
		return false;
}
// this function will search number of IDE devices connected to the system
// this will check for pci-ide controller
// if it doesn't have a PCI device then it will revert back to standard ide ports
// search for the controllers existance
// if a controller found then it will check for master device and slave device
// depending on which devices found it put a 1 in ide_drv array
void search_disks()
{
	pci_bus *pb=pci_bus::Instance();
	pci_dev *dev;
	unsigned int cmdbase_pri=ATA_BASE_PRI,cmdbase_sec=ATA_BASE_SEC;
	unsigned int ctrlbase_pri,ctrlbase_sec,bmidebase,bmidebase_pri,bmidebase_sec;	
	cout<<"Initializing IDE harddisks\n";	
	cout<<"Checking PCI bus for IDE \n";
	dev=pb->get_dev((unsigned char) 0x01,(unsigned char)0x01);
	if(dev->next!=NULL)
	{
		cout<<"WARNING: No support for more than one one IDE card\n";
		cout<<"Default is the first card detected\n";
		cout<<"On this machine "<<dev->bus<<":"<<dev->dev<<":"<<dev->func<<"\n";
	} 
	if(dev!=NULL)
	{
		cout<<vendor_to_string(dev->common->vendor_id) \
                <<vendor_device_to_string(dev->common->vendor_id,dev->common->device_id) \
		<<" Present on bus="<<dev->bus<<":"<<dev->dev<<":"<<dev->func \
		<<"\n";
		cmdbase_pri=get_bar(dev,0);
		ctrlbase_pri=get_bar(dev,1);
		cmdbase_sec=get_bar(dev,2);
		ctrlbase_sec=get_bar(dev,3);
		bmidebase=get_bar(dev,4);
		bmidebase &=0xfffe;
		if(cmdbase_pri==0xffff || ctrlbase_pri==0xffff || bmidebase==0xffff)
		{
			cout<<"Error in PCI config for primary channel\n";
		}
		else
		{
			cmdbase_pri &= 0xfffe;
			ctrlbase_pri &= 0xfffe;
			if(cmdbase_pri==0)
			{
				cout<<"device doesn't provide specific reg for primary ...defaulting\n";
				cmdbase_pri=ATA_BASE_PRI;
				ctrlbase_pri=0x3f0;
			}
			else
				ctrlbase_pri -=4;
			bmidebase_pri=bmidebase;
			ctrl_channel[0].base_reg = cmdbase_pri;
			ctrl_channel[0].ctrl_reg = ctrlbase_pri;
			ctrl_channel[0].bmide  =   bmidebase_pri;
			ctrl_channel[0].nIEN =     0;
		}
		if(cmdbase_sec==0xffff || ctrlbase_sec==0xffff || bmidebase==0xffff)
		{
			cout<<"Error in PCI config for secondary channel\n";
		}
		else
		{
			cmdbase_sec &= 0xfffe;
			ctrlbase_sec &= 0xfffe;
			if(cmdbase_sec==0)
			{
				cout<<"doesn't provide reg for secondary... defaulting\n";
				cmdbase_sec=ATA_BASE_SEC;
				ctrlbase_sec=0x370;
			}
			else
				ctrlbase_sec -=4;
			bmidebase_sec =bmidebase+8;
			ctrl_channel[1].base_reg = cmdbase_sec;
			ctrl_channel[1].ctrl_reg = ctrlbase_sec;
			ctrl_channel[1].bmide  =   bmidebase_sec;
			ctrl_channel[1].nIEN =     0;
		}
	}
	else
	{
		cout<<"No PCI IDE found\n";
		cout<<"Using Default IDE ports\n";
		ctrl_channel[0].base_reg=ATA_BASE_PRI;
		ctrl_channel[0].ctrl_reg=0x3F0;
		ctrl_channel[0].bmide=-1;
		ctrl_channel[0].nIEN=0;
		ctrl_channel[1].base_reg=ATA_BASE_SEC;
		ctrl_channel[0].ctrl_reg=0x370;
		ctrl_channel[0].bmide=0;
		ctrl_channel[1].nIEN=0;
	}
		
	//detect primary first
	int j=0;
	for(int i=0;i<2;i++)
	{
		if(detect_cntrlr(ctrl_channel[i].base_reg))
		{
			if(i==0)
				cout<<"Primary Controller found at ";
			
			else
				cout<<"Secondary controller found at ";
			cout.flags(hex|showbase);
			cout<<ctrl_channel[i].base_reg<<"\n";
			cout.flags(dec);
			if(detect_master(ctrl_channel[i].base_reg))
			{
				ide_drv[j]=1;
				j++;
				cout<<" Master found("<<j<<") ";
			}
			if(detect_slave(ctrl_channel[i].base_reg))
			{
				ide_drv[j]=1;
				j++;
				cout<<" Slave found("<<j<<") ";
			}
			cout<<"\n";
		}	
	}
	for(int i=0;i<4;i++)
	{
		if(ide_drv[i]==1)
		{
			ide_disks[i]=new IDEdrive;
			if(i<2)
				ide_disks[i]->chan=&ctrl_channel[0]; //primary
			else
				ide_disks[i]->chan=&ctrl_channel[1]; // secondary
			if(i%2)
				ide_disks[i]->devnum=1; //slave
			else
				ide_disks[i]->devnum=0; // master
		}
	}
	cout.flags(dec);
}
bool ata_identify(IDEdrive *drv)
{
	unsigned char temp1, temp2, id_cmd=0, buf[512], swap_chars;
	unsigned short ioadr, temp, id_delay;
	identify_data *dat;
	outportb(drv->chan->base_reg + DRV_HD_REG, 0x40|(drv->devnum ? SLAVE:MASTER));//select device
	temp1=inportb(drv->chan->base_reg+ SECT_CNT_REG);
	temp2=inportb(drv->chan->base_reg+ LBA_LOW_REG);
	if(temp1 != 0x01 || temp2 != 0x01)
	{
		cout<<"no drive on interface "<<drv->chan->base_reg;
		return false;
	}
	temp1 = inportb(drv->chan->base_reg + LBA_MID_REG);
	temp2 = inportb(drv->chan->base_reg + LBA_HI_REG);
	temp = inportb(drv->chan->base_reg + STATUS_REG);
	if(temp1 == 0x14 && temp2 == 0xEB)
	{
		cout<<"PATAPI CD \n";
		drv->devtyp=PATAPI;
		id_cmd=ATA_CMD_PID; // ATAPI identify
	}
	else if(temp1 == 0x69 && temp2 == 0x96)
	{
		cout<<"SATAPI CD forget it!!!\n";
		drv->devtyp=SATAPI;
		id_cmd=ATA_CMD_PID; // we don't know how to handle it
	}
	else if(temp1 == 0 && temp2 == 0 && temp != 0)
	{
		cout<<"PATA drive\n";
		drv->devtyp=PATA;
		id_cmd=ATA_CMD_ID; // ATA Identify
	}
	else if(temp1==0x3c && temp2 == 0xc3 && temp != 0)
	{
		cout<<"SATA drive, ";
		drv->devtyp=SATA;
		id_cmd=0; // we don't know how to handle it
	}
	else
	{
		cout<<"Unknown Drive type\n";
	}
	outportb(drv->chan->base_reg  + CMD_REG, id_cmd); // ATA_CMD_ID
	pio_ata_delay_400ns(drv->chan->ctrl_reg);
	if(!pio_wait_ready(drv->chan->base_reg,true))
	{
		cout<<"WARNING: time lapsed!!! but device is not ready!!!\n";
		return false;
	}
	if(id_cmd)
	{
		// read 256 shorts
		pio_rep_inw(drv->chan->base_reg,(unsigned short*)buf,256);
		dat=(identify_data *)buf;
		IDENTIFY_TEXT_SWAP(dat->serial,20);
		IDENTIFY_TEXT_SWAP(dat->firmware,8);
		IDENTIFY_TEXT_SWAP(dat->model,40);
		strcpy((char *)drv->model_name,(const char*)dat->model);
		strcpy((char *)drv->serial,(const char*)dat->serial);
		strcpy((char *)drv->firmware,(const char*)dat->firmware);
		if((buf[99] & 1) != 0)
		{
			cout<<"DMA, ";
			drv->dma = 1;
			drv->use_dma=0; // We are Idiots/Lazy so we don't do DMA
		}
		if((buf[99] & 2) != 0)
		{
			cout<<"LBA, ";
			drv->lba = 1;
			drv->use_lba=1; // use lba 
		}
		cout<<drv->model_name<<" "<<drv->firmware<<" "<<drv->serial<<"\n";
		hex_dump((unsigned char *)buf,20);
	}
	if(id_cmd==0) 
		return false;
	return true;
}
/*#define BPERL 16
void dump(void *data_p, unsigned count)
{
	unsigned char *data = (unsigned char *)data_p;
	unsigned i, j;

	while(count != 0)
	{
		for(i = 0; i < BPERL; i++)
		{
			if(count == 0)
				break;
			kprintf("%x ",data[i]);
			count--;
		}
		kprintf("\t");
		for(j = 0; j < i; j++)
		{
			if(data[j] < ' ')
				kprintf(".");
			else
				kprintf("%x",data[j]);
		}
		kprintf("\n");
		data += BPERL;
	}
}*/
int MIN ( int a, int b)
{
	if(a<b)
		return a;
	return b;
}
void hex_dump (const unsigned char *data, int len)
{
        int pos = 0;
	int i,lin=0;
	char ans;
  
	while (pos < len)
	{
		cout.flags(hex);
		cout<< pos<<"  ";

		int chunk = MIN (len - pos, 32);

		for ( i = 0; i < chunk; ++i)
		{
			if (i % 4 == 3)
				cout<<(unsigned short)data[pos + i]<<" ";
			else
				cout<<(unsigned short)data[pos + i];
		}
     // if (chunk < 16)
	//printf ("%*s", (int) ((16 - chunk) * 2 + (16 - chunk + 3) / 4), "");
	
		for ( i = 0;(unsigned int) i < chunk; ++i)
		{
			cout.flags(dec);
			unsigned char b = data[pos + i];
			if(b<' ')
				cout<<'.';
			else
				cout<<b;
	  //printf ("%c", isprint (b) ? b : '.');
		}

		cout<<'\n';
		pos += chunk;
		lin++;
		if(lin%24==0)
		{ 
			cin>>ans;
			lin=0;
		}
	}
	cout.flags(dec);
}

// this function will add up the real disks to the disks array
// this function will call the search_disks() first which will populate the ide_drv[]
// then it will iterate the arrary and try to fit the different param for the real 
// device hopefully by issueing a ATA_IDENTIFY command.
void init_disks()
{
	search_disks();
	for(int i=0;i<4;i++)
	{
		if(ide_drv[i]==1)
		{
			if(ata_identify(ide_disks[i]))
			{
				cout<<"So we got some drive and we think we can\n";
				cout<<"Process it... Well what next\n";
				switch(ide_disks[i]->devtyp)
				{
					case	PATA:
							cout<<" do check for partitions\n";
							cout<<" populate some drive info\n";
							cout<<" reflect in sysdrive\n";
							break;
					case	PATAPI: cout<<" Put it in to sysdrive and\n";
							cout<<" use a cdfs \n";
							break;
					case	SATA:
					case	SATAPI:
							cout<<" For the time-being forget it\n";
				}
			}
		}
	}
}
