//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2009//
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// Drives: Prototypes for Our drives                    //
//                                                      //
//////////////////////////////////////////////////////////
#ifndef __DRIVE_H__
#define __DRIVE_H__

#include "ide.h"
#include "fatfs.h"
/* drives in Nano-os is an instance of raw hard drive if it is not partition
   or the partition
   A drive has a parent IDE channel ide0,1,2,3,4
           has a partition 0-4
           has a filesystem 
*/

#define FAT12 0x01
#define FAT16 0x02
#define FAT32 0x03
#define EXT2  0x04
#define UNKFS 0x05
class drive{
		private:
			int ide_chan;
			bool is_partition;			
			int partition;
			bool has_fs;
			int fs_type;
			
		public:
			unsigned char super[512];
			drive();
			~drive(){};
			fat16 fs; //here I guess if i can do some trick and use a pointer to a generic fs class
                                   //We can implement any file system I will do it in future but not sure when  **TODO**
			void read();
			void write(){ cout<<"Not Implemented\n";};
			void format(){ cout<<"Not implemented\n";};
			void info();
			void set_ide_chan(int chan){ ide_chan=chan;};
			bool is_a_partition(){ return is_partition; };
			void set_is_partition(bool val){ is_partition = val;};
			void set_partition_num(int part){ partition = part;};
			void set_has_an_fs(bool val){ has_fs = val; };
			bool has_an_fs(){ return has_fs;};
			void set_fs_type(int fstyp){ fs_type = fstyp; };
			int get_fs_type(){ return fs_type;};
			int get_ide_chan(){ return ide_chan; };
			int get_partition_number(){ return partition;};
			
				
}; //end class
extern drive *sys_drv[26];
void init_sys_drv();
void sys_drv_info();

#endif //__DRIVE_H__
