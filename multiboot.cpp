//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2009//
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// multiboot   implementation                           //
//                                                      //
//////////////////////////////////////////////////////////
#include "multiboot.h"
#include "string.h"
#include "OStream.h"
/*multiboot::multiboot(multibootInfo *mbinfo)
{
	bootInfo = new multibootInfo;
	String::memcpy(bootInfo,mbinfo,sizeof(multibootInfo));
};*/
extern "C"{
unsigned long get_available_memory(multibootInfo *bootInfo)
{
  if (bootInfo->flags & MULTIBOOT_MEMORY) {
      return (unsigned long)((bootInfo->memoryUpper + 1024) << 10); 
  } else {
      /* this shouldn't happen - the bootloader has
       * to provide information on available memory */

      cout<<"No memory information!\n";
	return 0;
  }
}
unsigned long get_used_memory(multibootInfo *bootInfo)
{
  int i;
  extern unsigned int end;
  unsigned long top = (unsigned long)&end;

  if (bootInfo->flags & MULTIBOOT_MODS) {
      for (i=0; i<bootInfo->modulesCount; i++)
        if (bootInfo->modules[i].end > top)
          top = bootInfo->modules[i].end;
  }

  top = (top + 4095) & 0xFFFFF000;
  return (unsigned long)top;
}
void show_memory_map(multibootInfo *bootInfo)
{
  int i,parts;
  if (bootInfo->flags & MULTIBOOT_MEM_MAP)
  {
    cout<<"GRUB: BIOS provided physical RAM map\n";
    parts=bootInfo->mmap_length/sizeof(memoryMap);
    cout.flags(hex|showbase);
    for (i=0;i<parts;i++)
    {
      cout<<"GRUB: addr: ";
      cout<<(unsigned int)bootInfo->mmap_addr[i].BaseAddrHigh<<(unsigned int)bootInfo->mmap_addr[i].BaseAddrLow;
      cout<<" len : "<<(unsigned int) bootInfo->mmap_addr[i].LengthHigh<<(unsigned int)bootInfo->mmap_addr[i].LengthLow;
      if(1==bootInfo->mmap_addr[i].Type)
      	cout<<"(usable)\n";
      else
	cout<<"(reserved)\n";
    }
    cout<<"\n";
    cout.flags(dec);	
  }
  else
	cout<<"Memory infos not available through GRUB!!!\n";
}
void show_elf_info(multibootInfo *bootInfo)
{	
	cout<<"Kernel ELF Infos\n";
	cout<<"number of sections        => "<<(unsigned int)bootInfo->num<<"\n";
	cout<<"Kernel size               => "<<(unsigned int)bootInfo->size<<"kb\n";
	cout.flags(hex|showbase);
	cout<<"Kernel end address        => "<<(unsigned int)bootInfo->addr<<"\n";
	cout.flags(dec);
}
extern struct multibootHeader mboot;
unsigned long get_kernel_start()
{
	return(mboot.kernel_start);
}
unsigned long get_kernel_end()
{
	return(mboot.kernel_end);
}
unsigned long get_kernel_length()
{
	return(mboot.kernel_end-mboot.kernel_start);
}	
/*void print_boot_dev(multibootInfo *bootInfo)
{
	cout<<(char*)bootInfo->bootDevice<<"\n";
}*/

};//extern C ends
