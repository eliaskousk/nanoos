//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2012//
// ashok.s.das@gmail.com    GNU GPL-V2                  //
//////////////////////////////////////////////////////////
// mydrive.h defines drive class which is actually a    //
// partition.                                           //
//////////////////////////////////////////////////////////
#ifndef __MYDRIVE_H__
#define __MYDRIVE_H__
#include "myata.h"
#include "fatfs.h"

using namespace std;

class drive
{
	private:
		slot *drive_slot;
		unsigned short part_num;
		unsigned char fs_type;
		bool guessed; // this will be true in the begining then after we detect the actual FS it will be set to false
		char name[5];
		void guess_fs_type();
	public:
		class fat16 *fs;
		drive(){};
		drive(slot *s,unsigned short n);
		void set_partition(unsigned short p);
		unsigned short get_partition();
		void display_info();
		char *get_name();
		void set_fs_type(unsigned char fst);
		unsigned int read(unsigned int block,unsigned char *buffer); // block is relative to partition begining
		unsigned int write(unsigned int block,unsigned char *buffer){};
		
};

extern void init_sysdrives();
extern void display_sysdrive_info();
#endif
