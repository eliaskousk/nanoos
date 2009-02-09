//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2009//
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// FAT16 : implementation                               //
//  todo: implement FAT32 code                          //
//////////////////////////////////////////////////////////
#include "fatfs.h"
#include "drive.h"
#include "OStream.h"
#include "string.h"
//copied from ACE os FAT.h
int const ATTR_READONLY =0x01;
int const ATTR_HIDDEN   =0x02;
int const ATTR_SYSTEM   =0x04;
int const ATTR_VOLUMEID =0x08;
int const ATTR_DIRECTORY=0x10;
int const ATTR_ARCHIVE  =0x20;

bool fat16::valid_fat_fs()
{
	unsigned char tmp,tmp1;
	tmp=boot_sect.jmp_oem.jmp[0];
	tmp1=boot_sect.jmp_oem.jmp[1];
	//cout.flags(hex);
	//cout<<(unsigned int)tmp<<" "<<(unsigned int)tmp1<<"\n";
	//cout.flags(dec);
	if((tmp == 0xeb) || (tmp == 0xe9 && tmp1 ==0x00))
	{
		//cout<<"a valid boot jump\n";
		return true;
	}
	else
	{
		//cout<<"not a valid boot jump\n";
		return false; //valid FAT fs
	}
}

void fat16::dump_fat_info()
{
	cout<<"Super Block Information\n";
	cout<<"OEM name         = "<<boot_sect.jmp_oem.OEM_name<<"\n";
	cout<<"\tBytes / sector = "<<boot_sect.bpb.BytesPerSector<<"\n";
	cout<<"\tSectr / clustr = "<<boot_sect.bpb.SectorsPerCluster<<"\n";
	cout<<"\tReserved Sects = "<<boot_sect.bpb.ReservedSectors<<"\n";
	cout<<"\tNum of FATs    = "<<boot_sect.bpb.NumberFATs<<"\n";
	cout<<"\tRoot dir entrs = "<<boot_sect.bpb.RootDirEntries<<"\n";
	cout<<"\tTotal sectors  = ";
	if(boot_sect.bpb.TotalSectors16==0)
		cout<<boot_sect.bpb.TotalSectors32<<"\n";
	else
		cout<<boot_sect.bpb.TotalSectors16<<"\n";
	cout.flags(hex|showbase);
	cout<<"\tMedia Descrptr = "<<boot_sect.bpb.MediaDescriptor<<"\n";
	cout.flags(dec);
	cout<<"\tFAT size       = "<<boot_sect.bpb.FATSize16<<"\n";
	cout<<"\tSectors / Trk  = "<<boot_sect.bpb.SectorsPerTrack<<"\n";
	cout<<"\tNumber of Heads= "<<boot_sect.bpb.NumberOfHeads<<"\n";
	cout<<"\tHidden Sectors = "<<boot_sect.bpb.HiddenSectors<<"\n";
	cout.flags(hex|showbase);
	cout<<"\tDrive Number   = "<<boot_sect.fat_param.DriveNumber<<"\n";
	cout<<"\tBoot Signature = "<<boot_sect.fat_param.BootSignature<<"\n";
	cout<<"\tVolume ID      = "<<boot_sect.fat_param.VolumeID<<"\n";
	cout<<"\tVolume Label   = "<<boot_sect.fat_param.VolumeLabel<<"\n";
	cout<<"\tFile System    = "<<boot_sect.fat_param.FileSystem<<"\n";
	if((boot_sect.boot_sig&0x00ff==0xaa && boot_sect.boot_sig&0xff00==0x55) ||  \
		(boot_sect.boot_sig&0x00ff==0x55 && boot_sect.boot_sig&0xff00==0xaa))
		cout<<"\t Bootable FS!!!\n";
}

//read the first sector of the partition
//map to fat_12_16_bs
//check the jmp_oem.jmp[0] for 0xeb or 0xe9 if not present not valid FATFS
//check fat_param for FAT12 or FAT16 if any of these are present then it is a FAT fs
//check for number of clusters to determine which FAT format is there
void fat16::init_fat16(void *buf)
{
	String::memcpy(&boot_sect,(fat_12_16_bs *)buf,sizeof(fat_12_16_bs));
	/*cout<<"Dumping boot sect\n";
	hex_dump((unsigned char *)&boot_sect,128);*/
}
void fat16::fat_read()
{
//not implemnted
}
void fat16::fat_write()
{
	//not implemented
}
void fat16::fat_read_dir_entry()
{
	//not implemented
}
void fat16::fat_write_dir_entry()
{
	//not implemented
}

