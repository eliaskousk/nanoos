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

unsigned long multiboot::get_mem_avail()
{
	if (binfo->flags & MULTIBOOT_MEMORY)
	{
		return (unsigned long)((binfo->memoryUpper + 1024) << 10);
	}
	else
	{
      		/* this shouldn't happen - the bootloader has
       		 * to provide information on available memory */

		cout<<"No memory information!\n";
		return 0;
  	}
}
unsigned long multiboot::get_mem_used()
{
	int i;
	extern unsigned int end;
	unsigned long top = (unsigned long)&end;

	if (binfo->flags & MULTIBOOT_MODS)
	{
		for (i=0; (unsigned int)i<binfo->modulesCount; i++)
		if (binfo->modules[i].end > top)
          		top = binfo->modules[i].end;
	}

	top = (top + 4095) & 0xFFFFF000;
	return (unsigned long)top;
}
unsigned long multiboot::get_k_start()
{
	return mbooth->kernel_start;
}
unsigned long multiboot::get_k_end()
{
	return mbooth->kernel_end;
}
unsigned long multiboot::get_k_length()
{
	return (mbooth->kernel_end - mbooth->kernel_start);
}
multibootInfo *multiboot::binfo=NULL;
multibootHeader *multiboot::mbooth=NULL;
//multiboot *m_boot;

