//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2009//
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// IDE: Prototypes declaration                          //
//                                                      //
//////////////////////////////////////////////////////////
#ifndef __IDE_H__
#define __IDE_H__
#include "idt.h"
#define ATA_BASE_PRI	0x1f0 	// default ISA
#define ATA_BASE_SEC	0x170	// default ISA
#define MASTER				0xA0
#define SLAVE				0xB0
// bellow are offset from ATA_BASE_xxx
#define DATA_REG	0	//RW
#define ERR_REG		1	//R
#define FEATURE_REG	1	//RW
#define SECT_CNT_REG	2	//RW
#define LBA_LOW_REG	3	//rw
#define LBA_MID_REG	4	//rw
#define LBA_HI_REG	5	//rw
#define DRV_HD_REG	6	//rw
#define STATUS_REG	7	//r
#define CMD_REG		7	//W
#define ALT_ST_REG	0x206	//R
#define DEV_CTRL_REG	0x206	//w 
//status register fields
#define STA_ERR		1<<0	//error
#define STA_IDX		1<<1	//index mark
#define STA_CORR	1<<2	//data corrected
#define STA_DRQ		1<<3	//Data Transfer Requested
#define STA_DSC		1<<4	//seek complete
#define STA_DF		1<<5	//Device Fault
#define STA_DRDY	1<<6	//device ready
#define STA_BSY		1<<7	//busy
//error register fields
#define ERR_AMNF	1<<0	//Address Mark Not Found
#define ERR_TK0NF	1<<1	//Track 0 Not Found
#define ERR_ABRT	1<<2	//command aborted
#define ERR_MCR		1<<3	//Media Change Requested
#define ERR_IDNF	1<<4	//ID mark Not Found
#define ERR_MC		1<<5	//Media Changed
#define ERR_UNC		1<<6	//Uncorrectable data error
#define ERR_BBK		1<<7	//Bad Block
/* ATA command bytes */
#define	ATA_CMD_READ		0x20	/* read sectors */
#define	ATA_CMD_PKT		0xA0	/* signals ATAPI packet command */
#define	ATA_CMD_PID		0xA1	/* identify ATAPI device */
#define	ATA_CMD_READMULT	0xC4	/* read sectors, one interrupt */
#define	ATA_CMD_MULTMODE	0xC6
#define	ATA_CMD_ID		0xEC	/* identify ATA device */
//helper functions(reads)
#define ata_read_error(address)		(inportb((address)+ERR_REG))
#define ata_read_status(address)	(inportb((address)+STATUS_REG))
#define ata_read_sector_count(address)	(inportb((address)+SECT_CNT_REG))
#define ata_read_feature(address)	(inportb((address)+FEATURE_REG))
#define ata_read_alt_status(address)	(inportb((address)+ALT_ST_REG))
//helper functions(writes)
#define ata_write_feature(address,data)		(outportb((address)+FEATURE_REG,(data)))
#define ata_write_drv_ctrl(address,data)	(outportb((address)+DEV_CTRL_REG,(data)))
#define ata_write_cmd(address,data)		(outportb((address)+CMD_REG,(data)))
#define ata_write_head(address,data)		(outportb((address)+DRV_HD_REG,(data)))
#define	ata_write_lba(address,lba)		(outportb((address)+LBA_LOW_REG,(lba&0x0000ff))) \
						(outportb((address)+LBA_MID_REG,(lba&0x00ff00)>>8)) \
						(outportb((address)+LBA_HI_REG,(lba&0xff0000)>>16)) \

#define NUM_IO_SPANS	2
#define	read_le16(X)	*(unsigned short *)(X)
#define	read_be16(X)	bswap16(*(unsigned short *)(X))

// devtype defines
#define PATA	0
#define PATAPI	1
#define SATA	2
#define	SATAPI	3
// Primary or Secondary channel's register values
typedef struct channel  
{
	unsigned short base_reg; // base reg for the challel 
	unsigned short ctrl_reg; // ctrl reg for the channel
	unsigned short bmide;    // bus master IDE same for both channel but we will put
                                 // it in every instance  
	unsigned int   intr;	 // Interrupt assigned to this channel
	unsigned char nIEN;  // no interrupt 
} __attribute__((packed)) channel;
extern channel ctrl_channel[2];
// the structure to represent one ide/PATA drive
typedef struct IDEdrive
{
	channel 	*chan;		// structure to represent the channel of this IDE device
	unsigned short	devnum:1;	// master = 0 slave = 1
	unsigned short	devtyp:2;	// ATA or ATAPI,SATA,SATAPI
	unsigned short	lba:2;		// no LBA, LBA-28,LBA-48
	unsigned short	use_lba:1;	// are we going to use LBA(1) or CHS(0)
	unsigned short	dma:1;		// DMA capable???
	unsigned short	use_dma:1;	// use dma(1) don't use(0)
	unsigned short	multimode:1;	// has multi mode 
	unsigned short	use_multimode:1;// use multimode ??
	unsigned short	has_valid_mbr:1;
	unsigned short	is_partitioned:1;
	unsigned short	mult_count;
	unsigned int	totalsectors;
	unsigned char	model_name[41];	// 40 char name nullterminated
	unsigned char   serial[20];
	unsigned char	firmware[8];
	unsigned char	mbr[512];
} __attribute__((packed)) IDEdrive;
// partition entry in the drive
typedef struct part_entry
{
	unsigned int boot_flag:8;
	unsigned int beg_head:8;
	unsigned int beg_sect:8;
	unsigned int beg_cyl:8;
	unsigned int part_type:8;
	unsigned int end_head:8;
	unsigned int end_sect:8;
	unsigned int end_cyl:8;
	unsigned int  beg_lba;
	unsigned int  tot_sect;
} __attribute__((packed)) part_entry_t;
typedef struct IdentifyData
{
    unsigned short type;
    unsigned short reserved1[9];
    unsigned char  serial[20];
    unsigned short reserved2[3];
    unsigned char  firmware[8];
    unsigned char  model[40];
    unsigned short maxTransfer; // max num of sects to be xfered in read/write multiple
    unsigned short trustedFeatures;
    unsigned short capabilities[2];
    unsigned short reserved3[8];
    unsigned int sectors28;
    unsigned short reserved4[18];
    unsigned short majorRevision;
    unsigned short minorRevision;
    unsigned short supported[6]; // command set supported
    unsigned short reserved5[12];
    unsigned long long sectors48;
    unsigned short reserved6[2];
    unsigned short sectorSize;
} __attribute__((packed)) identify_data;

bool pio_wait_ready(unsigned short,bool);
bool ata_identify(IDEdrive *drv);
/*class disk
{
	private:
		IDEdrive *physical;
		bool has_valid_partition_tbl;
		
		//unsigned char sect_buf[512]={0};
	public:
		disk()
		{
			physical=new IDEdrive();
			has_valid_partition_tbl=false;
		};
		~disk()
		{
			delete physical;
		};
		part_entry_t part_table[4]; //drive has 4 primary partitions
		void populate_ide_disk(channel *chan, unsigned short unit)
		{
			physical->chan=chan;
			physical->devnum=unit;
		};
		bool wait_ready(unsigned short port)
		{
			return (pio_wait_ready(port,true));
		};
		
		//int wait_interrupt(unsigned short int_mask,unsigned int timeout);
		bool identify()
		{
			return (ata_identify(physical));
		}
		
		void read_sector(unsigned int blk,unsigned char *read_buf){};
		void write_sector(unsigned int blk,unsigned char *buf){};
		void part_ent_info(int i){};
		
		//static void disk_handler(IDT::regs *r);
		void disk_info(){};
		void populate_partitions(){};
		void disk_handler(IDT::regs *r){};
		bool is_partitioned(){ return has_valid_partition_tbl;};
		bool is_valid_part_entry(int partn){ if(part_table[partn].tot_sect>0)
							return true;
						     else
							return false;
						    };
		void read_first_sector_of_partition(int partn,unsigned char *buf)
		{
			//please do a checks.			
			read_sector(part_table[partn].beg_lba,buf);
		};
};*/
//extern disk *disks[4];
void init_disks();
//int ide_select(ide_t *ide);
bool detect_cntrlr(unsigned short port);
void detect_ide();
void read_sector(unsigned short port,unsigned int blk,char *read_buf);
//void ide_handler(regs *r);
void dump(void *data_p, unsigned count);
void hex_dump (const unsigned char *data, int len);
//void ide_read_handler(regs *r);
void display_ide();
#endif
