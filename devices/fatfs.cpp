//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2009//
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// FAT16 : implementation                               //
//  todo: implement FAT32 code                          //
//////////////////////////////////////////////////////////
#include "fatfs.h"
//#include "mydrive.h"
#include "OStream.h"
#include "string.h"
#include "utils.h"

using namespace std;


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
unsigned int fat16::offset_of_cluster(unsigned int cluster)
{
	unsigned int lba;
	lba = ((cluster -2)* boot_sect.bpb.SectorsPerCluster);
	return (lba);

}

void fat16::dump_fat_info()
{
	cout<<"Super Block Information\n";
	cout<<"OEM name         = "<<boot_sect.jmp_oem.OEM_name<<"\n";
	cout<<"\tBytes / sector = "<<boot_sect.bpb.BytesPerSector<<"\n";
	cout<<"\tSectr / clustr = "<<(int)boot_sect.bpb.SectorsPerCluster<<"\n";
	cout<<"\tReserved Sects = "<<boot_sect.bpb.ReservedSectors<<"\n";
	cout<<"\tNum of FATs    = "<<(int)boot_sect.bpb.NumberFATs<<"\n";
	cout<<"\tRoot dir entrs = "<<boot_sect.bpb.RootDirEntries<<"\n";
	cout<<"\tTotal sectors  = ";
	if(boot_sect.bpb.TotalSectors16==0)
		cout<<boot_sect.bpb.TotalSectors32<<"\n";
	else
		cout<<boot_sect.bpb.TotalSectors16<<"\n";
	cout.flags(hex|showbase);
	cout<<"\tMedia Descrptr = "<<(int)boot_sect.bpb.MediaDescriptor<<"\n";
	cout.flags(dec);
	cout<<"\tFAT size       = "<<boot_sect.bpb.FATSize16<<"\n";
	cout<<"\tSectors / Trk  = "<<boot_sect.bpb.SectorsPerTrack<<"\n";
	cout<<"\tNumber of Heads= "<<boot_sect.bpb.NumberOfHeads<<"\n";
	cout<<"\tHidden Sectors = "<<boot_sect.bpb.HiddenSectors<<"\n";
	cout.flags(hex|showbase);
	cout<<"\tDrive Number   = "<<(int)boot_sect.fat_param.DriveNumber<<"\n";
	cout<<"\tBoot Signature = "<<(int)boot_sect.fat_param.BootSignature<<"\n";
	cout<<"\tVolume ID      = "<<boot_sect.fat_param.VolumeID<<"\n";
	cout<<"\tVolume Label   = "<<boot_sect.fat_param.VolumeLabel<<"\n";
	cout<<"\tFile System    = "<<boot_sect.fat_param.FileSystem<<"\n";
	if(((boot_sect.boot_sig&0x00ff)==0xaa && (boot_sect.boot_sig&0xff00)==0x55) ||  \
		((boot_sect.boot_sig&0x00ff)==0x55 && (boot_sect.boot_sig&0xff00)==0xaa))
		cout<<"\t Bootable FS!!!\n";
	cout.flags(dec);
}

//read the first sector of the partition
//map to fat_12_16_bs
//check the jmp_oem.jmp[0] for 0xeb or 0xe9 if not present not valid FATFS
//check fat_param for FAT12 or FAT16 if any of these are present then it is a FAT fs
//check for number of clusters to determine which FAT format is there
//void fat16::init_fat16(fat_12_16_bs *bs)
void fat16::init_fat16(drive *d )
{
	
	drv = d;  //set the drive
	// we will read the first sector which is passed to us from drve class
	drv->read(0,(unsigned char*)&boot_sect);
	FirstFATbegin = boot_sect.bpb.ReservedSectors; // relative to begining of partitition
	SecondFATbegin = FirstFATbegin + boot_sect.bpb.FATSize16;
	RootDirbegin =  boot_sect.bpb.ReservedSectors + boot_sect.bpb.NumberFATs * boot_sect.bpb.FATSize16;
	RootDirSectors = (boot_sect.bpb.RootDirEntries * sizeof(direntry) + boot_sect.bpb.BytesPerSector-1) \
				/boot_sect.bpb.BytesPerSector;
	DataAreabegin = RootDirbegin + RootDirSectors;
	TotalDataSectors = boot_sect.bpb.TotalSectors16 - DataAreabegin;
	TotalClusters = TotalDataSectors / boot_sect.bpb.SectorsPerCluster;
}
void fat16::fat16_read_cluster(unsigned int cluster, unsigned char *buffer)
{
	unsigned int lba;
	lba = offset_of_cluster(cluster);
	drv->read(lba,buffer);
}
void fat16::fat16_write_cluster(unsigned int cluster, unsigned char *buffer)
{
	unsigned int lba;
	lba = offset_of_cluster(cluster);
	drv->write(lba,buffer);
}
void fat16::fat16_read_sector(unsigned int sector, unsigned char *buffer)
{
	drv->read(sector,buffer);
}
void fat16::fat16_write_sector(unsigned int sector, unsigned char *buffer)
{
	drv->write(sector,buffer);
}
void fat16::display_root_dir()  // test funtion to demo our fat read and write works
{
//not implemnted
	unsigned short root_dir_sectors = (boot_sect.bpb.RootDirEntries * 32)/ boot_sect.bpb.BytesPerSector;
	unsigned char *root_dir_buffer = new unsigned char[boot_sect.bpb.BytesPerSector];
	String::memset(root_dir_buffer,0,root_dir_sectors * boot_sect.bpb.BytesPerSector);
	for(int i = 0;i<root_dir_sectors;i++)
	{
		fat16_read_sector(RootDirbegin + i,root_dir_buffer);
		int j;
		for(j= 0; j<16; j++)
		{
			direntry *d;
			d = (direntry *)root_dir_buffer+j;
			if(d->FileName[0] == '\0')
				break;
			else
				cout<<" "<<i*16+j<<" "<<d->FileName<<" "<<d->Extension<<" "<<d->FileSize<<"\n";
		}
		if(j<16)
			break;
	}
/*
	// we got the whole root dir entries in the root_dir_buffer
	// now les parse it 
	for(int i = 0; i< 512;i++)
	{
		direntry *d;
		d = (direntry *)root_dir_buffer+i;
		if(d->FileName[0]>'\0')
		{
			cout<<" "<<i<<" "<<d->FileName<<" "<<d->Extension<<" "<<d->FileSize<<"\n";
		}
		//continue;
	}
	//hex_dump(root_dir_buffer,(boot_sect.bpb.BytesPerSector * root_dir_sectors));
*/	
	delete root_dir_buffer;
}

/*
	these bellow functions are to be imlemented in user space program and/or library
void fat16::fat_read_dir_entry()
{
	//not implemented
}
void fat16::fat_write_dir_entry()
{
	//not implemented
}
direntry *fat_find_first(char *filename)
{
	
}
direntry *fat_find_next(char *filename)
{
}
*/
