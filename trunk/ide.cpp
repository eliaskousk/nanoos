//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2009//
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// ide: IDE hard disk driver routine implementation     //
//                                                      //
//////////////////////////////////////////////////////////
#include "ide.h"
#include "low-io.h"
#include "idt.h"
#include "OStream.h"
#include "timer.h"

#define MASTER 0xA0
#define SLAVE 0xB0
int ide_drv[4]={0}; //stores the boolean value if a device is present 1de_drv[0]=primary master ide_drv[3]= secondary slave
unsigned char ide_interrupt_14=0;
unsigned char rd_buf[512]={0};
disk *disks[4]={NULL};//*ide0,*ide1,*ide2,*ide3; //maximum 4 physical disks on 2 IDE controller
//swaps bytes in a short int
unsigned short bswap16(unsigned short arg)
{
	return ((arg << 8) & 0xFF00) |
		((arg >> 8) & 0xFF00);
}

void disk::populate_ide_disk(unsigned short which, unsigned short unit) //which = ATA_BASE_PRI/SEC, unit = MASTER/SLAVE
{
	physical.blkdev.io.dma=0;
	physical.blkdev.io.adr[0]=which;
	physical.blkdev.io.adr[1]=which+DEV_CTRL_REG;
	physical.blkdev.io.span[0]=8;
	physical.blkdev.io.span[1]=1;
	if(which==0x1f0)
		physical.blkdev.io.irq=0x4000;
	else
		physical.blkdev.io.irq=0x8000;
	physical.blkdev.unit=unit;
}
int disk::wait_ready(unsigned short port,unsigned int timeout)
{
	unsigned char stat;
	outportb(port+ERR_REG,0);
	while(timeout)
	{
		stat=ata_read_status(port);
		if(!stat) return 1;
		if(stat & ((STA_DRDY|STA_BSY)==STA_DRDY)) return 1;
		stat=ata_read_alt_status(port);
		if(!stat) return 1;
		if(stat & ((STA_DRDY|STA_BSY)==STA_DRDY)) return 1;
		timeout--;
	}
	return -1;
}
int disk::wait_interrupt(unsigned short int_mask,unsigned int timeout)
{
	unsigned short interrupted=0;
	int_mask |= ide_interrupt_14; ///***currently vauge statement :
					/// Should be worked upon	
	while(timeout--)
	{
		if(ide_interrupt_14!=0)
		{
			interrupted=1;
			break;
		}
		my_timer->sleep(10);
	}
	if(!interrupted)
		return -1;
	else
		return 1;
}
//identify the geometry and populate accordingly
int disk::identify()
{
	unsigned char temp1, temp2, id_cmd, buf[512], swap_chars=1,drv;
	unsigned short  temp, id_delay;	
	cout<<"Identify\n";
	if(this->physical.blkdev.unit==0xa0)
		drv=0;
	if(this->physical.blkdev.unit==0xb0)
		drv=1;
	outportb(this->physical.blkdev.io.adr[0]+FEATURE_REG,0);
	outportb(this->physical.blkdev.io.adr[0]+DRV_HD_REG,drv<<4|this->physical.blkdev.unit);
	temp1=inportb(this->physical.blkdev.io.adr[0]+ SECT_CNT_REG);
	temp2=inportb(this->physical.blkdev.io.adr[0]+ LBA_LOW_REG);
	if(temp1 != 0x01 || temp2 != 0x01)
	{
		cout<<"no drive on interface ";
		cout.flags(hex|showbase);		
		cout<<physical.blkdev.io.adr[0]<<"\n";
		cout.flags(dec);		
		return -1;
	}
	temp1 = inportb(this->physical.blkdev.io.adr[0] + LBA_MID_REG);
	temp2 = inportb(this->physical.blkdev.io.adr[0] + LBA_HI_REG);
	temp  = inportb(this->physical.blkdev.io.adr[0] + STATUS_REG);
	if(temp1 == 0x14 && temp2 == 0xEB)
	{
		cout<<"ATAPI CD forget it!!!\n";
	}
	else if(temp1 == 0 && temp2 == 0 && temp != 0)
	{
		cout<<"ATA drive, ";
		id_cmd = ATA_CMD_ID;
		id_delay = 30000;	/* WAIT_ID */
		this->physical.blkdev.bytes_per_blk = 512;
	}
	else
	{	
		cout<<"unknown drive type\n";
		return -1;
	}
	cout.flags(dec);
	outportb(this->physical.blkdev.io.adr[0]  + CMD_REG, id_cmd);
	my_timer->sleep(10);
	if(wait_ready(this->physical.blkdev.io.adr[0],1000)!=1)
	{
		cout<<"WARNING: timeout on waiting\n";
		cout<<"Continuing furher is erroneous\n";
	}
	insw(this->physical.blkdev.io.adr[0]+ DATA_REG, (unsigned short *)buf, 256);/*read ATAPI xfer count */
	for(temp = 54; temp < 94; temp += 2)
	{
		cout<<(char)buf[(temp + 0) ^ swap_chars];
		cout<<(char)buf[(temp + 1) ^ swap_chars];
	}
	this->physical.blkdev.cyls = read_le16(buf + 2);
	this->physical.blkdev.heads = read_le16(buf + 6);
	this->physical.blkdev.sectors = read_le16(buf + 12);
	
	cout<<"\n CHS="<<physical.blkdev.cyls<<":"<<physical.blkdev.heads<<":"<<physical.blkdev.sectors<<"\n";
	physical.blkdev.num_blks=physical.blkdev.cyls*physical.blkdev.heads*physical.blkdev.sectors;
	if((buf[99] & 1) != 0)
	{
		cout<<"DMA, ";
		physical.has_dma = 1;
	}
	if((buf[99] & 2) != 0)
	{
		cout<<"LBA, ";
		physical.has_lba = 1;
	}
	if(((buf[119] & 1) != 0) && (buf[118] != 0))
	{
		temp = buf[94];
		cout<<"mult_count= "<<temp<<"\n";
	}
	else
		temp = 1;
	physical.mult_count = temp;
	cout<<(read_le16(buf + 42) >> 1)<<"K cache\n";
	return 1;
}
//read a sector
void disk::read_sector(unsigned int blk,unsigned char *read_buf)
{
	unsigned char head,lo,mid,hi,drv;
	unsigned short stmp;	
	int i=0;
	if((blk< 1) || (blk > physical.blkdev.num_blks))
	{
		cout<<"Error: lba not within limit\n";
		return;
	}
	cout<<"DEBUG : requested blk = "<<blk<<"\n";
	while(!wait_ready(physical.blkdev.io.adr[0],1000));
	outportb(physical.blkdev.io.adr[0]+ERR_REG,0);
	if(physical.blkdev.unit==MASTER)
		drv=0;
	else if(physical.blkdev.unit==SLAVE)
		drv=1;
//break lba stuff to put in to different register
	head=blk>>24;
	head=head&0x0f;
	hi=blk>>16;
	hi=hi&0xff;
	mid=blk>>8;
	mid &=0xff;
	lo=blk&0xff;
//put the above value in to resoective register
	outportb(physical.blkdev.io.adr[0]+LBA_LOW_REG,lo);
	outportb(physical.blkdev.io.adr[0]+LBA_MID_REG,mid);
	outportb(physical.blkdev.io.adr[0]+LBA_HI_REG,hi);
	outportb(physical.blkdev.io.adr[0]+SECT_CNT_REG,1);
	outportb(physical.blkdev.io.adr[0]+DRV_HD_REG,drv<<4|physical.blkdev.unit|head);
//we need to read from the device
	outportb(physical.blkdev.io.adr[0]+CMD_REG,ATA_CMD_READ);
//wazit until the DATA REQUEST status is gone
	while (!(inportb(physical.blkdev.io.adr[0]+STATUS_REG) & STA_DRQ));
	for (i = 0; i <256; i++)
	{
		stmp = inportw(physical.blkdev.io.adr[0]+DATA_REG);
		read_buf[i * 2] =  stmp;
		read_buf[(i*2)+1] = (stmp >> 8);
	}
	//read_sect(port,read_buf);
	//ata_write_cmd(port,0x21);
}
//interrupt handler for primary controller
void disk::disk_handler(IDT::regs *r)
{
	ide_interrupt_14 |= 0x4000;
	cout<<"IDE interrupt\n";
}

void disk::disk_info()
{
	cout<<"=========DISK INFO============\n";
	cout<<"Disk is ";	
	if(physical.blkdev.io.adr[0]==0x1f0)
		cout<<"primary ";
	else
		cout<<"secondary ";
	if(physical.blkdev.unit==0xa0)
		cout<<"master : ";
	else
		cout<<"slave : ";
	if(!has_valid_partition_tbl)
	{
		cout<<"WARNING : Invalid Partition Table\n";
		cout<<"\t try fdisk under DOS/Linux to correct it\n";
		return;
	}
	//else has_valid_partition_tbl==true
	cout<<"Geometry CHS= "<<physical.blkdev.cyls<<":"<<physical.blkdev.heads<<":"<<physical.blkdev.sectors<<"\n";
	cout<<"=========Partitions===========\n";
	for(int i=0;i<4;i++)
	{
		if(part_table[i].tot_sect>0)
		{
			unsigned int start_head,end_head,start_sect,start_cyl,end_sect,end_cyl;
			start_head=part_table[i].beg_head;
			end_head=part_table[i].end_head;
			start_sect=part_table[i].beg_sect;
			start_cyl=part_table[i].beg_cyl;
			start_cyl +=(start_sect&192)<<2;
			start_sect &=63;
			end_sect=part_table[i].end_sect;
			end_cyl=part_table[i].end_cyl+((end_sect&192)<<2);
			end_sect &=63;
			
			cout<<"Part["<<i<<"] starts at "<<"H="<<start_head<<" S="<<start_sect<<" C="<<start_cyl;
			cout<<"\n        ends at"<<" H="<<end_head<<" S="<<end_sect<<" C="<<end_cyl;		
			cout<<" LBA= "<<part_table[i].beg_lba<<" and spans "<<part_table[i].tot_sect<<" sectors ";
			if(part_table[i].boot_flag==0x80) cout<<"Bootable ";
			cout<<"type= "<<(int)part_table[i].part_type<<"\n";
		}
	}
	cout<<"=============================\n";

}
void disk::populate_partitions()
{
	unsigned char mbr[512]={'\0',};
	unsigned char parttbl[64]={'\0',};
	read_sector(1,mbr);
	String::memcpy((char *)parttbl,(char *)(mbr+0x1be),64);
	if(*(unsigned char *)(mbr+0x1fe)!=0x55 && *(unsigned char *)(mbr+0x1ff)!=0xaa) //check if the magic signature is at the end of MBR
	{
		cout<<"Invalid MBR or corrupted Partition Table\n";
		has_valid_partition_tbl=false;
		return;
	}
	has_valid_partition_tbl=true;
	//hex_dump(mbr,512);
	//cout<<sizeof(part_entry_t)<<"\n";
	for(int i=0;i<4;i++)
	{
		String::memset((unsigned char *)&part_table[i],0,sizeof(part_entry_t));
		String::memcpy((char *)&part_table[i],(char *)(parttbl+(i*16)),sizeof(part_entry_t));
	}
}
//bellow functions are not in the class they are generic
//int ide_status_ready(ide_t *ide,unsigned int timeout)
//{
//	return wait_ready(ide->blkdev.io.adr[0],timeout);
//}
/*int ide_select(ide_t *ide)
{
	ata_write_head(ide->blkdev.io.adr[0],ide->blkdev.unit);
	sleep(4);
	return ide_status_ready(ide,1000);
}*/
//port = 1f0 for primary controller detection and 170 for secondary controller	
int detect_cntrlr(unsigned short port)
{
	unsigned char temp1,temp2,stat,timeout=255;
	outportb(port+LBA_LOW_REG,0x55);
	outportb(port+LBA_MID_REG,0xAA);
	temp1=inportb(port+LBA_LOW_REG);
	temp2=inportb(port+LBA_MID_REG);
	if((temp1==0x55)&&(temp2==0xAA))
	{
		outportb(port+DEV_CTRL_REG,0x06);	//soft reset ????	
		my_timer->sleep(10);
		outportb(port + DEV_CTRL_REG, 0x00);
		my_timer->sleep(10);
		//while (!(inportb(port+STATUS_REG) & STA_DRDY));
		//qemu needs bellow vvvv		
		while(timeout)
		{
			stat=ata_read_status(port);
			if(!stat) return 1;
			if(stat & ((STA_DRDY|STA_BSY)==STA_DRDY)) return 1;
			stat=ata_read_alt_status(port);
			if(!stat) return 1;
			if(stat & ((STA_DRDY|STA_BSY)==STA_DRDY)) return 1;
			timeout--;
			my_timer->sleep(10);
		}
		//qemu needs ^^^^
		//bochs needs this vvvv
		while (!(inportb(port+STATUS_REG) & STA_DRDY));
		return 1;
		//bochs needs ^^^^
	}
	//else
		return 0;
}
/*
void detect_ide()
{
	kprintf("Detecting Primary Controller\n");
	if(detect_cntrlr(ATA_BASE_PRI)==1)
	{
		kprintf("Primary controller found\n");
		ide_interrupt_14=0;
		irq_install_handler(14,ide_handler);
		populate_ide_disk(ATA_BASE_PRI, MASTER,&ide0);
		populate_ide_disk(ATA_BASE_PRI, SLAVE,&ide1);
		if(identify(&ide0)!=1)
			kprintf("Problem in identifying IDE Primary master\n");
	//	if(identify(&ide1)!=1)
	//		kprintf("Problem in identifying IDE Primary slave\n");
		
		//irq_install_handler(7,ide_read_handler);
	}
	else
		kprintf("No primary controller\n");
	
	kprintf("Detecting Secondary Controller\n");
	if(detect_cntrlr(ATA_BASE_SEC)==1)
		kprintf("secondary controller found\n");
	else
		kprintf("No secondary controller\n");
}*/

//void ide_read_handler(regs *r)
//{
//	kprintf("Read handler\n");
//}

/*
static inline read_sect(unsigned char port,unsigned char *buf)
{
	insw(port+DATA_REG,buf,256);
}
*/
#define BPERL 32
void dump(void *data_p, unsigned count)
{
	unsigned char *data = (unsigned char *)data_p;
	unsigned i, j;
	cout.flags(hex|showbase);
	while(count != 0)
	{
		for(i = 0; i < BPERL; i++)
		{
			if(count == 0)
				break;
			cout<<data[i];
			count--;
		}
		cout<<"\t";
		for(j = 0; j < i; j++)
		{
			if(data[j] < ' ')
				cout<<".";
			else
				cout<<data[j];
		}
		cout<<"\n";
		data += BPERL;
	}
}
int detect_master(unsigned short port)
{
	int tmp;
	outportb(port + DRV_HD_REG, 0x40|MASTER);	// Set drive
	//my_timer->sleep(100);
	while ((inportb(port+STATUS_REG) & STA_BSY));	
	tmp = inportb(port+STATUS_REG);	// Read status
	//cout<<tmp<<"\n";
	if (tmp & STA_DRDY)
		return 1;
	else
		return 0;
}
int detect_slave(unsigned short port)
{
	int tmp;
	outportb(port + DRV_HD_REG, 0x40|SLAVE);	// Set drive
	//my_timer->sleep(100);
	while ((inportb(port+STATUS_REG) & STA_BSY));	
	tmp = inportb(port+STATUS_REG);	// Read status
	//cout<<tmp<<"\n";
	if (tmp & STA_DRDY)
		return 1;
	else
		return 0;
}
void init_disks()
{
	cout<<"Initializing IDE harddisks\n";	
	//detect primary first
	cout.flags(hex|showbase);
	if(detect_cntrlr(ATA_BASE_PRI)==1)
	{
		cout<<"Primary Controller found at ";
		
		cout<<ATA_BASE_PRI<<"\n";
		
		if(detect_master(ATA_BASE_PRI)==1)
		{
			cout<< "found master\n";
			ide_drv[0]=1;
		}
		if(detect_slave(ATA_BASE_PRI)==1)
		{
			cout<<"found slave\n";		
			ide_drv[1]=1;
		}	
	}
	if(detect_cntrlr(ATA_BASE_SEC)==1)
	{
		cout<<"Secondary controller found at ";
		
		cout<<ATA_BASE_SEC<<"\n";
		if(detect_master(ATA_BASE_SEC)==1)
		{
			ide_drv[2]=1;
			cout<< "found master\n";
		}
		if(detect_slave(ATA_BASE_SEC)==1)
		{
			ide_drv[3]=1;
			cout<<"found slave\n";
		}
	}
	for(int i=0;i<4;i++)
	{
		unsigned short which,unit;
		if(ide_drv[i]==1)
		{
			disks[i]=new disk();
			if((i==0)||(i==2))
				unit=MASTER;
			else
				unit=SLAVE;
			if(i<2)
				which=ATA_BASE_PRI;
			else
				which=ATA_BASE_SEC;
			disks[i]->populate_ide_disk(which,unit);
			disks[i]->identify();
			disks[i]->populate_partitions();
		}
		else
			disks[i]=NULL;
	}
	cout.flags(dec);
}
int MIN ( int a, int b)
{
	if(a<b)
		return a;
	return b;
}
void hex_dump (const unsigned char *data, int len)
{
        int pos = 0;
	int i;
	
  while (pos < len)
    {
	cout.flags(hex);
      cout<< pos<<"  ";

      const size_t chunk = MIN (len - pos, 16);

      for ( i = 0;(unsigned int) i < chunk; ++i)
	if (i % 4 == 3)
	  cout<<(unsigned short)data[pos + i]<<" ";
	else
	  cout<<(unsigned short)data[pos + i];

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
    }
	cout.flags(dec);
}
