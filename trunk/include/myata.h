//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2012//
// ashok.s.das@gmail.com      GNU GPL-V2                //
//////////////////////////////////////////////////////////
// ATA implementation header                            //
//                                                      //
//////////////////////////////////////////////////////////
#ifndef __MYATA_H__
#define __MYATA_H__

#define ATA_TIMEOUT 300000             // a large value should be better

// channel referes to primary or secondary
// we will populate while discovering
typedef struct chan
{
	unsigned short base_reg;
	unsigned short ctrl_reg;
	unsigned short bmide;
	unsigned char nIEN;
}chan;
extern chan channels[2];
typedef struct partition
{
	unsigned char boot_indicator;
	unsigned char starting_head;
	unsigned short starting_sec_cyl;
	//unsigned short starting_cylinder:10;
	unsigned char  system_id;
	unsigned char  ending_head;
	unsigned short ending_sec_cyl;
	//unsigned short ending_cylinder:10;
	unsigned int   start_lba;
	unsigned int   total_sectors;
} __attribute__((packed)) partition;
typedef struct mbr
{
	unsigned char boot_code[446]; // 436 bytes of boot code + 10 bytes of Uniq ID of disk but all can be used for boot code
	partition partitions[4];      // 4 partitions
	unsigned short signature;     // 0x55,0xaa
} __attribute__((packed)) mbr;      // TOTAL 446+4*16+2=446+64+2=512 bytes
// slots are end devices attached to either master or slave
typedef struct slot
{
	unsigned char ps:1;      // primary 0, secondary 1
	unsigned char ms:1;      // master =0 slave =1
	unsigned char exists:1;  // if exists 1 else 0
	unsigned char devtype:2; // unknown 000, 001 ata, 010 atapi, 011 sata, 100 satapi 
	unsigned char lba:1;
	unsigned char dma:1;
	chan *chanl;              // which channel this drive is connected primary or secondary
	unsigned short heads;    // number of heads 
	unsigned short sectors;  // number of sectors 
	unsigned int cylinders;  // number of cylinders
	unsigned int capacity;   // total number of sectors
	unsigned int sectors28;
	unsigned long long sectors48;
	unsigned short drv_number; // drive number 0-4
	partition partition_table[4]; // a Partition table has 4 partitions
	struct slot *next;
}slot;


// flags if a drive is found 
extern unsigned int my_drives[4];
// here we will store the slot info for further processing
extern slot *slots; // defined in myata.cpp
//=============================================================
extern void init_disks();
slot *get_device(unsigned short type);
unsigned int ata_r_sector(slot *drv,unsigned int block,unsigned short *buf);
unsigned int ata_w_sector(slot *drv,unsigned int block,unsigned short *buf);
void display_partition_info(partition *p);
#endif //__MYATA_H__
