//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2009//
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// kheap header                                         //
//                                                      //
//////////////////////////////////////////////////////////
#ifndef __KHEAP_H__
#define __KHEAP_H__

#ifndef NULL
#define NULL 0
#endif 
unsigned int kmalloc(unsigned int sz, unsigned int * phys, bool align = false);
unsigned int kmalloc(unsigned int sz, bool align = false);

#endif

