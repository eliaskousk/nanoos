//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2009//
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// kheap a heap based memory manager from osdev.org     //
//                                                      //
//////////////////////////////////////////////////////////
#include "kheap.h"

unsigned int kmalloc_internal(unsigned int sz, bool align, unsigned int * phys);

extern unsigned int end;
unsigned int placement_address = (unsigned int)&end;

// This is just a dead-simple memory allocator so I can make global objects.
// As of now, the memory can't be freed. It can do page allignment, and 
// optionally output a physical address

// I plan to implement better heap management once I have enabled paging.

unsigned int kmalloc_internal(unsigned int sz, bool align, unsigned int * phys)
{	
	// We check if we want to page-allign the memory, and whether it
	// is already page aligned
	
	if(align && (placement_address & 0xFFFFF000) )
	{
		// If it's not aligned...
		placement_address &= 0xFFFFF000;
		placement_address += 0x1000;
	}
	
	if(phys != NULL)
	{
		*phys = placement_address;
	}
	
	unsigned int tmp = placement_address;
	
	placement_address += sz;
	
	return tmp;
}

unsigned int kmalloc(unsigned int sz, unsigned int * phys, bool align)
{
	return kmalloc_internal(sz, align, phys);
}

unsigned int kmalloc(unsigned int sz, bool align)
{	
	return kmalloc_internal(sz, align, NULL);
}
