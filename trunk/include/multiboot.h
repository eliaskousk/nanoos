//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2009//
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// Multiboot implementation header                      //
//                                                      //
//////////////////////////////////////////////////////////
#ifndef __MBOOT_H__
#define __MBOOT_H__

#define MULTIBOOT_MEMORY	(1L<<0)
#define MULTIBOOT_BOOT_DEVICE	(1L<<1)
#define MULTIBOOT_CMDLINE	(1L<<2)
#define MULTIBOOT_MODS		(1L<<3)
#define MULTIBOOT_AOUT_SYMS	(1L<<4)
#define MULTIBOOT_ELF_SHDR	(1L<<5)
#define MULTIBOOT_MEM_MAP	(1L<<6)
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


#endif
