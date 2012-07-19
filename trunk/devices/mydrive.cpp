//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2009//
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// drive : implementation                               //
//                                                      //
//////////////////////////////////////////////////////////
#include "OStream.h"
#include "string.h"
#include "myata.h"
#include "mydrive.h"
#include "fatfs.h"


using namespace std;
using namespace String;
/*class filesystem
{
	private:
		unsigned char fstype;
	public:
		unsigned int fs_read();
		unsigned int ts_write();
};*/

drive::drive(slot *s, unsigned short n)
{
	if(!s)
		drive_slot = NULL;
	else 
		drive_slot = s;
	if(n<0 || n>4)
		part_num=0;
	else
		part_num = n;

	strcpy(name,"hd");
	if(drive_slot->ps)
	{
		if(drive_slot->ms)
			strcat(name,"d");
		else
			strcat(name,"c");
	}
	else
	{
		if(drive_slot->ms)
			strcat(name,"b");
		else
			strcat(name,"a");
	}
	
	switch(part_num)
	{
		case 0:	strcat(name,"-1");
			break;
		case 1: strcat(name,"-2");
			break;
		case 2: strcat(name,"-3");
			break;
		case 3: strcat(name,"-4");
	}
	guess_fs_type();
	cout<<"Trying detecting FS...on"<<name<<"\n";
	fat_12_16_bs *bs = new fat_12_16_bs;
	if(read(0, (unsigned char *)bs)) // 0 offset from partition begining is the boot sector
	{
		if(bs->fat_param.BootSignature == 0x29)
		{
			cout<<"FAT FS found on "<<name<<"\n";
			guessed = false; // we confirmed it is what we guessed from partition type.
			fs = new fat16();
			fs->init_fat16(this);
		}
		else
		{
			cout<<"Sorry no FAT FS\n";
			cout<<bs->jmp_oem.OEM_name<<"\n";
		}	
	}
	else
		cout<<"Cant read boot sector of drive "<<name<<" start lba ="<<drive_slot->partition_table[part_num].start_lba<<"\n";
	delete bs;
}
char *drive::get_name()
{
	return (name);
}
void drive::set_partition(unsigned short p)
{
	part_num = p;
}
unsigned short drive::get_partition()
{
	return (part_num); 
}
void fs_type_to_string(unsigned short fs_type)
{
	switch(fs_type)
	{
		case 0x01: cout<<"FAT12\n";
				break;
		case 0x04: 
		case 0x05:
		case 0x06:
		case 0x0e: cout<<"FAT16\n";
				break;
		case 0x0b: 
		case 0x0c: cout<<"FAT32\n";
				break;
		case 0x83: cout<<"EXT2\n";
				break;
		default: cout<<"Un known File System\n";
	}
}
void drive::display_info()
{
	cout<<"Drive ::"<<name<< " partition ";
	display_partition_info(&drive_slot->partition_table[part_num]);
	fs_type_to_string(fs_type);
}
void drive::set_fs_type(unsigned char fst)
{
	fs_type=fst;
}
void drive::guess_fs_type()
{
	fs_type = drive_slot->partition_table[part_num].system_id;
	cout<<"Guessed File System ID = "<<(int)fs_type<<" Name = ";
	fs_type_to_string(fs_type);
	guessed = true;
}
// drive read() should read a relative block from the begining of the partition
//  
unsigned int drive::read(unsigned int block, unsigned char *buffer)
{
	unsigned int st,en;
	st= drive_slot->partition_table[part_num].start_lba;
	en = st + drive_slot->partition_table[part_num].total_sectors;
	block += st; 
	if((block < st) || (block > en))
	{
		cout<<"Trying to access out side the boundary\n";
		return 0;	
	}
	return ata_r_sector(drive_slot,block,(unsigned short *)buffer);
}
/*unsigned int drive::write()
{
}*/


