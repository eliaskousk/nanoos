//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2009//
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// Drives: Prototypes for Our drives                    //
//                                                      //
//////////////////////////////////////////////////////////
#include "drive.h"
/* 
   first we will scan all the IDEs in disk array.
   Then we will scan for the partitions if any and
   Populate our sys_drv array.
*/
unsigned char super[512];
drive *sys_drv[26];
void init_sys_drv()
{
	drive *temp_drv;
	int drv_num=0;
	
	for(int i=0;i<26;i++)
	{
		sys_drv[i]=NULL;
	}
	for(int i=0;i<4;i++) //4 ide disks
	{
		//cout<<"\nDEBUG :ide = "<<i;
		if(disks[i])
		{
			if(disks[i]->is_partitioned())
			{
				for(int j=0;j<4;j++) //parttions
				{
					
					if (disks[i]->is_valid_part_entry(j))
					{
												
						//cout<<"Partition = "<<j;
						sys_drv[drv_num] = new drive();
						sys_drv[drv_num]->set_ide_chan(i);
						sys_drv[drv_num]->set_is_partition(true);
						sys_drv[drv_num]->set_partition_num(j);
						//cout<<"beg-lba : "<<disks[i]->part_table[j].beg_lba;
						disks[i]->read_sector(disks[i]->part_table[j].beg_lba+1,super);
						//hex_dump(super,128);
						//cout<<"Read super\n";
						sys_drv[drv_num]->fs.init_fat16(super);
						//cout<<"FAT16 init done\n";
						//to populate FS related stuff 
						// barebone fs detection code should be written first
						//TODO please add some code for ext2 and hopefully MINIX fs 
						
						if(sys_drv[drv_num]->fs.valid_fat_fs())
						{
							//cout<<"Got a FAT fs\n";
							sys_drv[drv_num]->set_has_an_fs(true);
							sys_drv[drv_num]->set_fs_type(FAT16);
						}
						drv_num++;
					}
				}
			}	
			//unpartitioned raw disk
			else
			{
				sys_drv[drv_num] = new drive();
				sys_drv[drv_num]->set_ide_chan(i);
				sys_drv[drv_num]->set_is_partition(false);
				drv_num++;
			}
		}
	}
}

void sys_drv_info()
{
	cout<<"Installed disk Information\n";
	cout<<"==========================\n";
	//drive *installed_drv=sys_drv;
	int drv_num=0;
	while(sys_drv[drv_num]!=NULL)// || drv_num<26)
	{
		cout<<"system drive : "<<drv_num<<" : \n";		
		sys_drv[drv_num]->info();
		drv_num++;	
	}
}
drive::drive()
{
	ide_chan=0;
	is_partition=false;
	partition=0;
	has_fs=false;
	fs_type= UNKFS;
		
}

void drive::info()
{
	cout<<"Parent IDE channel : "<<ide_chan<<"\n";
	if(is_partition)
		cout<<"Partition          : "<<partition<<"\n";
	else
		cout<<"Unpartitioned disk\n";
	if(has_fs)
	{
		cout<<"File system         : ";
		switch(fs_type)
		{
			case 0x01: cout<<"FAT12\n";
					break;
			case 0x02: cout<<"FAT16\n";
					break;
			case 0x03: cout<<"FAT32\n";
					break;
			case 0x04: cout<<"EXT2\n";
					break;
			default: cout<<"Un supported Filesystem\n";
		}
	}
	else
		cout<<"Unformated drive\n";
}
void drive::read()
{
	//disk read calls IDE read
}
