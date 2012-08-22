//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2009//
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// ATA: implementation                                  //
//  todo: ATAPI                                         //
//////////////////////////////////////////////////////////
#include "low-io.h"
#include "idt.h"
#include "irq.h"
#include "OStream.h"
#include "IStream.h"
#include "timer.h"
#include "pci.h"
#include "ide.h"
#include "myata.h"
#include "mydrive.h"
#include "string.h"
#include "utils.h"
using namespace std;
using namespace String;

chan channels[2]={{0,0,0,0},{0,0,0,0}};



// flags if a drive is found 
unsigned int my_drives[4]={0,0,0,0};
// here we will store the slot info for further processing
slot *slots=NULL;  
//==========================|helper functions|==============================
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
// reads a word 2 byts or short int
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
void pio_rep_outb(unsigned short port, unsigned char *buffer, unsigned int count)
{
	outsb(port, buffer, count);
}
void pio_rep_outw(unsigned short port, unsigned short *buffer, unsigned int count)
{
	outsw(port, buffer, count);
}
void pio_rep_outdw(unsigned short port, unsigned int *buffer, unsigned int count)
{
	outsdw(port, buffer, count);
}
unsigned char pio_get_status(unsigned short port)
{
	return pio_inbyte(port+STATUS_REG);
}
unsigned char pio_get_astatus(unsigned short port)
{
	return pio_inbyte(port+ALT_ST_REG);
}
bool pio_wait_busy(unsigned short port)
{
	// should have a delay here !!! guess it will work
	for(int i=0;i<4;i++)
		my_timer->sleep(10); 
	return(pio_get_status(port)&STA_BSY);  // true if busy else false
}

bool pio_wait_busy_astat(unsigned short port)
{
	for(int i=0;i<4;i++)
		my_timer->sleep(10); 
	return(pio_get_astatus(port)&STA_BSY);
}

bool is_device_ready(slot *s)
{
	return(pio_inbyte(s->chanl->base_reg + ALT_ST_REG)&STA_DRDY);
}
bool is_device_busy(slot *s)
{
	return(pio_inbyte(s->chanl->base_reg + ALT_ST_REG)&STA_BSY);
}
// stop sending interrupts
// this should be called after selecting a drive 
void stop_ata_intr(unsigned short ctrl_port)
{
	pio_outbyte(ctrl_port,ATA_CTL_nIEN);
}
// reset the drives on this controller
// call this if some drive behave insane
// or in the begining of driver
bool reset_controller(unsigned short port)
{
	pio_outbyte(port + DEV_CTRL_REG,ATA_CTL_SRST|ATA_CTL_nIEN);
	my_timer->sleep(2);
	//cout<<"Stopping interrupt of channel\n";
	pio_outbyte(port + DEV_CTRL_REG,ATA_CTL_nIEN);
	my_timer->sleep(2);
	unsigned int timeout=300000;
	while(pio_wait_busy(port))
	{
		timeout--;
		my_timer->sleep(1);
	};
	if(!timeout)
		cout<<"Device is in busy state for a long time even after reset\n";
	unsigned short err=pio_inbyte(port+ERR_REG);
	if(err)
	{
		cout<<"Warning Error flag is "<<err<<" "<<(unsigned short)pio_inbyte(port+ALT_ST_REG)<<"\n";
	}
	if(timeout)
		return true;
	return false;
}

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
//===========================================================================
// this function will check a master drive in a channel
// return true if found
// return false if not
// takes the base port for the channel
bool detect_master(unsigned short port)
{
	int tmp;
	outportb(port + DRV_HD_REG, 0xA0);	// Set drive
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
	outportb(port + DRV_HD_REG, 0xB0);	// Set drive
	pio_wait_busy(port);	
	tmp = inportb(port+STATUS_REG);	// Read status
	if (tmp & STA_DRDY)
		return true;
	else
		return false;
}

bool search_disks()
{

	unsigned int cmdbase_pri=ATA_BASE_PRI,cmdbase_sec=ATA_BASE_SEC;
	unsigned int ctrlbase_pri,ctrlbase_sec,bmidebase,bmidebase_pri,bmidebase_sec;
	pci_bus *pb=pci_bus::Instance();
	pci_dev *dev=NULL;
	cout.flags(hex|showbase);
	cout<<"Initializing IDE harddisks\n";	
	cout<<"Checking PCI bus for IDE \n";
	dev=pb->get_dev((unsigned char) 0x01,(unsigned char)0x01);
	if(dev && (dev->next!=NULL))
	{
		cout<<"WARNING: No support for more than one one IDE card\n";
		cout<<"Default is the first card detected\n";
		cout<<"On this machine "<<dev->bus<<":"<<dev->dev<<":"<<dev->func<<"\n";
	}
	
	if(dev!=NULL)
	{
		
		cout<<vendor_to_string(dev->common->vendor_id) \
                <<vendor_device_to_string(dev->common->vendor_id,dev->common->device_id) \
		<<" Present on bus="<<dev->bus<<":"<<dev->dev<<":"<<dev->func<<" irq ="<<(unsigned int)dev->irq \
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
				cout<<"Device doesn't provide specific reg for primary ...defaulting\n";
				cmdbase_pri=ATA_BASE_PRI;
				ctrlbase_pri=cmdbase_pri + DEV_CTRL_REG;
			}
			else
				ctrlbase_pri -=4;
			bmidebase_pri=bmidebase;
			channels[0].base_reg = cmdbase_pri;
			channels[0].ctrl_reg = ctrlbase_pri;
			channels[0].bmide  =   bmidebase_pri;
			channels[0].nIEN =     0;
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
				cout<<"Device doesn't provide specific reg for secondary... defaulting\n";
				cmdbase_sec=ATA_BASE_SEC;
				ctrlbase_sec=cmdbase_sec + DEV_CTRL_REG;
			}
			else
				ctrlbase_sec -=4;
			bmidebase_sec =bmidebase+8;
			channels[1].base_reg = cmdbase_sec;
			channels[1].ctrl_reg = ctrlbase_sec;
			channels[1].bmide  =   bmidebase_sec;
			channels[1].nIEN =     0;
		}
	}
	else
	{
		cout<<"No PCI IDE found\n";
		return false;
/*
		cout<<"Using Default IDE ports\n";
		channels[0].base_reg=ATA_BASE_PRI;
		channels[0].ctrl_reg=0x3F6;
		channels[0].bmide=-1;
		channels[0].nIEN=0;
		channels[1].base_reg=ATA_BASE_SEC;
		channels[1].ctrl_reg=0x376;
		channels[1].bmide=0;
		channels[1].nIEN=0;*/
	}
	cout<<"Details of channels found\n";
	for(int k=0;k<2;k++)
	{
		cout<<"  "<<k<<" base = "<<channels[k].base_reg<<" ctrl = "<<channels[k].ctrl_reg<<" bmide = " \
		<<channels[k].bmide<<" nIEN = "<<(short)channels[k].nIEN<<"\n";
	}
	cout<<"Reseting controllers\n";
	if(!reset_controller(channels[0].base_reg))
		cout<<"can't reset channel 0\n";
	if(!reset_controller(channels[1].base_reg))
		cout<<"can't reset channel 1\n";
	
	for(int c=0,s=0;c<2;c++)
	{
		if(detect_master(channels[c].base_reg))
			my_drives[s]=1;
		s++;
		if(detect_slave(channels[c].base_reg))
			my_drives[s]=1;
		s++;
	}
	slot *temp=NULL,*cur_slot=NULL;
	for(int k=0;k<4;k++)
	{	// for every entry in my_drives we will create a linklist of drives
		cout<<" Found drives ";
		if(my_drives[k]==1)
		{
		 	cout<<k<<" ";
			temp = new slot;
			if(!temp)
			{
				cout<<"Insufficient Memory for slot information\n";
				while(1);
			}
			memset(temp,'\0',sizeof(slot));
			temp->chanl = new chan;
			memset(temp->chanl,'\0',sizeof(chan));
			
			temp->exists = 1;
			temp->drv_number = k;
			temp->next=NULL;
			if(k%2)
				// slave device
				temp->ms = 1;
			else
				// master device
				temp->ms = 0;
			if(k<2)
			{
				// primary channel
				temp->chanl->base_reg = channels[0].base_reg;
				temp->chanl->ctrl_reg = channels[0].ctrl_reg;
				temp->chanl->bmide = channels[0].bmide;
				temp->chanl->nIEN  = channels[0].nIEN;
				temp->ps = 0;
			}
			else
			{
				// secondary channel
				temp->chanl->base_reg = channels[1].base_reg;
				temp->chanl->ctrl_reg = channels[1].ctrl_reg;
				temp->chanl->bmide    = channels[1].bmide;
				temp->chanl->nIEN     = channels[1].nIEN;
				temp->ps = 1;
			}
			if(slots==NULL)
			{
				slots = temp;
				cur_slot=temp;
			}
			else
			{
				cur_slot->next = temp;
				cur_slot=cur_slot->next;
			}
		}	
	}
	cout<<"\n";		
	cout.flags(dec);
	return true;		
}
void select_device(slot *s)
{
	unsigned short port;
	unsigned char val=0;
	port = s->chanl->base_reg + DRV_HD_REG;
	if(s->ms)
		val = 0xB0 ;
	else
		val = 0xA0 ;
	pio_outbyte(port,val);
}


unsigned int ata_r_sector(slot *drv,unsigned int block,unsigned short *buf);
unsigned int ata_w_sector(slot *drv,unsigned int block,unsigned short *buf);
void display_partition_info(partition *p);

void browse_slots()
{
	slot *temp;
	temp = slots;
	
	unsigned char id_cmd=0x00;
	ata_ident *dat = new ata_ident;
	memset(dat,'\0',sizeof(ata_ident));
	/*while(is_device_busy(temp))
	{
		cout<<"device is busy!!! sleeping\n";
		my_timer->sleep(300);
	};*/
	cout.flags(hex|showbase);
	while(temp)
	{	
		cout<<"Selecting ::";
		cout<<(temp->ps ? "Secondary ":"Primary ")<<(temp->ms ? "Slave ":"Master ")<<"drive Number :"<<temp->drv_number<<"\n";
		select_device(temp);
		if(is_device_ready(temp))
		{
			cout<<"device is ready\n";
		}
		else 
		{
			cout<<"device is not ready\n";
			continue;
		}
		if(is_device_busy(temp))
			cout<<"device is Busy\n";
		pio_outbyte(temp->chanl->base_reg + CMD_REG, ATA_CMD_ID); 
		// Now if it is pata drive it will put an error flag in flags register
		if(pio_inbyte(temp->chanl->base_reg + ERR_REG) & STA_ERR)
		{
			temp->devtype = PATA;
			id_cmd=ATA_CMD_ID; // ATA Identify
		}
		else
		{
			unsigned char temp1;
			unsigned char temp2;
			
			temp1 = inportb(temp->chanl->base_reg + LBA_MID_REG);
			temp2 = inportb(temp->chanl->base_reg + LBA_HI_REG);
			
			if(temp1 == 0x14 && temp2 == 0xEB)
			{
				temp->devtype = PATAPI;
				id_cmd=ATA_CMD_PID; // ATAPI identify
			}
			if(temp1 == 0x69 && temp2 == 0x96)
			{
				temp->devtype = SATAPI;
				id_cmd=ATA_CMD_PID; // we don't know how to handle it
			}
			if(temp1 == 0 && temp2 == 0 )
			{
				temp->devtype = PATA;
				id_cmd=ATA_CMD_ID; // ATA Identify
			}
			if(temp1==0x3c && temp2 == 0xc3 )
			{
				temp->devtype = SATA;
				id_cmd=0; // we don't know how to handle it
			}
		}
		switch(id_cmd)
		{
			case ATA_CMD_ID : //we already sent it so we only will read if STA_DRQ status is present
						//pio_outbyte(temp->chanl->base_reg + CMD_REG, ATA_CMD_ID); 
						if(pio_get_status(temp->chanl->base_reg) & STA_DRQ)
						{
							pio_rep_inw(temp->chanl->base_reg,(unsigned short *)dat,256);
							IDENTIFY_TEXT_SWAP(dat->model,40);
							temp->heads = dat->discard1[6];
							temp->sectors = dat->discard1[3];
							temp->cylinders = dat->discard1[1];
							temp->sectors28= dat->lba28maxsects;
							temp->sectors48= dat->lba48maxsects;
							if((dat->capability & 1<<8) == 1<<8)
								temp->dma=1;
							if((dat->capability & 1<<9) == 1<<9)
								temp->lba=1;
							cout<< dat->model<<"\n";
							// here read the MBR then extract the partition table
							mbr *tmbr = new mbr;
							if(!ata_r_sector(temp,0,(unsigned short *)tmbr))
								cout<<"Error reading MBR";
							else
							{
								//for the timebeing we are only intersted in partition table
								// copy it
								memcpy(temp->partition_table,tmbr->partitions,4*sizeof(partition)); 
							}
							delete tmbr; // so now free the allocated space.
						}
						break;
			case ATA_CMD_PID :
						pio_outbyte(temp->chanl->base_reg + CMD_REG, ATA_CMD_PID);
							 pio_rep_inw(temp->chanl->base_reg,(unsigned short*)dat,256);
							 IDENTIFY_TEXT_SWAP(dat->model,40);
							cout<< dat->model<<"\n";
							
						break;
		}
		temp=temp->next;
	}
	cout.flags(dec);
	delete dat;
}

void display_slot_info()
{
	slot *temp;
	temp = slots;
	cout<<"[Displaying slot infornations]\n";
	while(temp)
	{
		cout<<(temp->ps ? "sec ":"pri ")<<(temp->ms ? "slv ":"mst ")<<(temp->devtype ? "SATA(PI)/PATAPI ":"PATA ") \
		<<(temp->lba ? " LBA ":"CHS ")<<(temp->dma ? "DMA ":"no DMA ")<<temp->heads<<" "<<temp->sectors<<" " \
		<<temp->cylinders<<" lab28sects= "<<temp->sectors28<<" lba48sects= "<<temp->sectors48<<"\n";
		if(temp->devtype)
		goto ml;
		if(temp->partition_table)
		{
			// show the partition s with infos
			for(int p=0;p<4;p++)
			{
				cout<<(int)p+1<<" ";
				display_partition_info(&temp->partition_table[p]);
			}
		}
	ml:
		temp=temp->next;
	}
}
slot *get_device(unsigned short type)
{
	slot *temp=NULL,*dev1=NULL,*dev=NULL;
	temp = slots;
	if(!temp)
		return NULL;
	while(temp)
	{
		if(temp->devtype == type)
		{
			if(dev1==NULL)
			{
				dev=temp;
				dev->next=NULL;
				dev1=dev;
			}
			else
				dev->next=temp;	
			dev=dev->next;
		}
		temp = temp->next;
	}
	//cout<<"nodevice of type ="<<type<<" found\n";
	return dev1;
}
int wait_ready(unsigned short port,unsigned int timeout)
{
	unsigned char stat;
	outportb(port+ERR_REG,0);
	while(timeout)
	{
		stat=ata_read_status(port);
		if(!stat) return 1;
		if((stat & (STA_DRDY|STA_BSY))==STA_DRDY) return 1;
		timeout--;
	}
	return -1;
}
// generic function to read or write a sector
// drv is the ide drive , block is LBA , direction is read =0 write =1

unsigned int ata_rw_sector(slot *drv,unsigned int block,unsigned short *buf,unsigned char direction) 
{
	unsigned char sc, cl, ch, hd, cmd;
	unsigned int timeout=300000;
	unsigned short iobase = drv->chanl->base_reg;
	//iobase = drv->chanl->base_reg;
	if(drv->sectors28<block)
		return 0;
	/* put exclusion thigs here */
	//select_device(drv);
	/*{
		cout<<"select drive failed \n";
		// uninitialize exclusion here
		return 0;	
	}*/
	//stop_ata_intr(drv->chanl->ctrl_reg);
	if (drv->lba) 
	{
		sc = block & 0xff;
		cl = (block >> 8) & 0xff;
		ch = (block >> 16) & 0xff;
		hd = (block >> 24) & 0x0f;
		//if(drv->ms)
		//	hd |=(1<<4);
		//if(drv->ps)
		//	hd |=0xf0;
	} 
	else 
	{
        /* See http://en.wikipedia.org/wiki/CHS_conversion */
	        int cyl = block / (drv->heads * drv->sectors);
	        int tmp = block % (drv->heads * drv->sectors);
	        sc = tmp % drv->sectors + 1;
	        cl = cyl & 0xff;
	        ch = (cyl >> 8) & 0xff;
	        hd = tmp / drv->sectors;
	}
	if(direction == 0)
	{
		cmd= ATA_CMD_READ;
	}
	else
	{
		cmd= ATA_CMD_WRITE;	
	}
	pio_outbyte(iobase + FEATURE_REG,0); // ????
	pio_outbyte(iobase + SECT_CNT_REG, 1); //we want only one sector
	pio_outbyte(iobase + LBA_LOW_REG, sc);
	pio_outbyte(iobase + LBA_MID_REG, cl);
	pio_outbyte(iobase + LBA_HI_REG, ch);
	pio_outbyte(iobase + DRV_HD_REG, 0xE0|hd|drv->ms<<4);//(drv->ms<<4)|0xE0|hd should be passed 
						//according to http://wiki.osdev.org/ATA_PIO_Mode#28_bit_PIO
						// but it never works
	pio_outbyte(iobase + CMD_REG, cmd);
	/* The host shall wait at least 400 ns before reading the Status register.
	See PIO data in/out protocol in ATA/ATAPI-4 spec. */
	TIMER *tmr=TIMER::Instance();
	tmr->sleep(30);
	//cout<<" Reading hd= "<<(int)hd<<" sect= "<<(int)sc<<" cyl= "<<(int)cl<<"\n";
	//while(pio_inbyte(iobase+DATA_REG)==0x08);
/*	while (timeout)	
	{
		// wait for busy flag to clear
		if(!pio_inbyte(iobase + STATUS_REG)& STA_BSY)
			break;
		timeout--;
		tmr->sleep(1);
	}
	if(timeout==0)
	{
		// put unlock for mutex here
		cout<<"Time out but device never came back from busy state\n";	
		return 0;
	}*/
	/* Did the device report an error? */
	if (pio_inbyte(iobase + ALT_ST_REG) & STA_ERR) 
	{
		//put unlock mutex 
		cout<<"Error in operation\n";
		return 0;
	}
	// we are ok till now
	// as we habe stopped the interrupt we should poll data
	/*timeout = 300000;
	for(; timeout>0; timeout--)
	{
		if(pio_inbyte(iobase + STATUS_REG) & STA_DRQ)
			break;
	}
	if(timeout==0)
	{
		// put unlock for mutex here
		cout<<"time out waiting for data request\n";	
		return 0;
	}*/
	// well our request is successfull
	// now read it to the output buffer
	while(!(pio_inbyte(iobase + ALT_ST_REG) & STA_DRQ));
	if(direction == 0)
	{
		//cmd= ATA_CMD_READ;
		pio_rep_inw(iobase + DATA_REG, buf, 256);
		while(is_device_busy(drv))
		{
			my_timer->sleep(30);
		}
	}
	else
	{
		//cmd= ATA_CMD_WRITE;
		pio_rep_outw(iobase + DATA_REG, buf, 256);	
	}
	return 1;
}
unsigned int ata_r_sector(slot *drv,unsigned int block,unsigned short *buf)
{
	return ata_rw_sector(drv, block, buf,0);
}
unsigned int ata_w_sector(slot *drv,unsigned int block,unsigned short *buf)
{
	return ata_rw_sector(drv, block, buf,1);
}
void display_partition_info(partition *p)
{
	if(p)
	{
		unsigned short shd=0,ssc=0,scy=0,ehd=0,esc=0,ecy=0, cyl_hi=0;
		shd = p->starting_head;
		ssc = p->starting_sec_cyl & 0x003F;
		scy = p->starting_sec_cyl;
		scy >>=6;   // 10 bits remain  the lower 2 bits are actually spill over of higher 2 bits of 10 bit
		cyl_hi= scy & 0x0003; // higher 2bits of cylinder
		scy >>=2;        // only lower 8 bits
		scy = scy | (cyl_hi<<8) ; // covert the cylinder back to normal by putting higher 2bits
		ehd = p->ending_head;
		esc = p->ending_sec_cyl & 0x003F;
		ecy = p->ending_sec_cyl;
		ecy >>=6;
		cyl_hi = scy & 0x0003;
		ecy >>=2;
		ecy = ecy | (cyl_hi<<8);
		cout<<"    "<<((p->boot_indicator == 0x80)?"A ":"  ")<<shd<<" "<<ssc<<" "<<scy<<" "<<ehd<<" "<<esc<<" "<<ecy<<" ";
		cout<<p->start_lba<<" "<<p->total_sectors<<" ";
		cout.flags(hex|showbase);
		cout<<(int)p->system_id<<"\n";
		cout.flags(dec);	
	}
}
drive *sysdrives[16]; //we are currently suupporting 4 partitions per slot ant 4 slots
void init_sysdrives()
{
	slot *temp=NULL;
	temp = slots;
	
	cout<<"[Initializing System drives ]\n";
	int i=0;
	while(temp)
	{
		//cout<<"Found a slot\n";
		
		if(temp->devtype) // PATA 0
			goto ml;
		else
		{
			cout<<"found a PATA drive\n";
			if(temp->partition_table)
			{
				// show the partition s with infos
				for(int p=0;p<4;p++)
				{
					if(temp->partition_table[p].total_sectors>0)
					{
						//cout<<"found a non zero partition\n";
						drive *d = new drive(temp, p);
						sysdrives[i] = d;
						i++;
					}
				}
			}
		}
	ml:
		temp=temp->next;
	}
	cout<<"Total "<<i<<" drives found\n";
}
void display_sysdrive_info()
{
	char ans[5];
	for(int i=0;i<16;i++)
	{
		if(sysdrives[i]!=NULL)
		{
			cout<<" "<<i<<" ";
			sysdrives[i]->display_info();
			cin>>ans;
			sysdrives[i]->fs->dump_fat_info();
			sysdrives[i]->fs->display_root_dir();
		}
	}
}
void init_disks()
{
	if(search_disks())
	{
		browse_slots();
		//display_slot_info();
		init_sysdrives();
		//display_sysdrive_info();
	}
	else
		cout<<"No IDE disks!!!\n";
	/*char ans[5];*/
	/*if(slot *temp = get_device(0))
	{
		unsigned short *sector = new unsigned short[256];
		unsigned int stlba = temp->partition_table[0].start_lba;
		cout<<"Reading "<<stlba<<" \n";
		if(ata_r_sector(temp,stlba,sector))
		hex_dump((unsigned char*)sector,512);
	}*/	
}
