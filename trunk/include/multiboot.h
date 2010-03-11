//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2009//
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// Multiboot implementation header                      //
//                                                      //
//////////////////////////////////////////////////////////
#ifndef __MBOOT_H__
#define __MBOOT_H__
#include "singleton.h"
#define MULTIBOOT_MEMORY	(1L<<0)
#define MULTIBOOT_BOOT_DEVICE	(1L<<1)
#define MULTIBOOT_CMDLINE	(1L<<2)
#define MULTIBOOT_MODS		(1L<<3)
#define MULTIBOOT_AOUT_SYMS	(1L<<4)
#define MULTIBOOT_ELF_SHDR	(1L<<5)
#define MULTIBOOT_MEM_MAP	(1L<<6)
extern "C"{
typedef struct multibootModule
{
  /* physical start and end addresses of the module data itself.  */
  unsigned long start;
  unsigned long end;

  /* arbitrary ASCII-Z string associated with the module.  */
  char *string;

  /* boot loader must set to 0; OS must ignore.  */
  unsigned long reserved;
} __attribute__ ((packed)) multibootModule;

typedef struct memoryMap
{
  unsigned long Size;
  unsigned long BaseAddrLow;
  unsigned long BaseAddrHigh;
  unsigned long LengthLow;
  unsigned long LengthHigh;
  unsigned long Type;
} __attribute__ ((packed)) memoryMap;

typedef struct multibootHeader
{
	unsigned long header_magic;    //multiboot header magic 0x1BADB002
	unsigned long header_flags;    //flags enabled 
	unsigned long ckecksum;        //checksum
	unsigned long header_address;  //the location of this structure
	unsigned long kernel_start;    // start of kernel .text section
	unsigned long kernel_data_end; //end of kernel .bss
	unsigned long kernel_end;      // kernel end address
	unsigned long kernel_entry;    //kernel entry point
} __attribute__ ((packed)) multibootHeader;

typedef struct multibootInfo
{
  /* these flags indicate which parts of the multiboot_info are valid;
   * see below for the actual flag bit definitions.  */
  unsigned long flags;

  /* lower/upper memory installed in the machine.
   * valid only if MULTIBOOT_MEMORY is set in flags word above.  */
  unsigned long memoryLower;
  unsigned long memoryUpper;

  /* BIOS disk device the kernel was loaded from.
   * Valid only if MULTIBOOT_BOOT_DEVICE is set in flags word above.  */
  unsigned char bootDevice[4];

  /* command-line for the OS kernel: a null-terminated ASCII string.
   * valid only if MULTIBOOT_CMDLINE is set in flags word above.  */
  char *commandLine;

  /* list of boot modules loaded with the kernel.
   * valid only if MULTIBOOT_MODS is set in flags word above.  */
  unsigned long modulesCount;
  multibootModule *modules;

  /* info about kernel ELF file */
  unsigned long num;
  unsigned long size;
  unsigned long addr;
  unsigned long reserved;

  /* memory map -- int 0x15 BIOS call (e820) */
  unsigned long mmap_length;
  memoryMap *mmap_addr;
} __attribute__ ((packed)) multibootInfo;
extern multibootHeader mboot; //from startup.asm

}; //extern C ends
//class multiboot

//class multiboot :public singleton<multiboot>
class multiboot : public singleton<multiboot>
{
	private:
		static multibootInfo *binfo;
		static multibootHeader *mbooth; // set by startup.asm
	public:
		//multiboot(){};
		
		void set_multiboot_info(multibootInfo *b)
		{ 
			binfo=b; 
		};
		void set_multiboot_hdr(multibootHeader *h=&mboot)
		{
			mbooth=h;
		};
		unsigned long get_mem_avail();//{ return (get_available_memory());};
		unsigned long get_mem_used();//{ return (get_used_memory());};
		unsigned long get_k_start();//{ return (get_kernel_start());};
		unsigned long get_k_end();//{ return (get_kernel_end());};
		unsigned long get_k_length();//{ return(get_kernel_length());};
};

#endif //mboot_h

