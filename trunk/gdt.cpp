//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008-2010 //
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// gdt.cpp : defines our GDT and associated functions   //
//                                                      //
//////////////////////////////////////////////////////////
#include "gdt.h"
#include "low-io.h"
#include "OStream.h"
#define dword unsigned int
#define byte unsigned char
namespace GDT{

	DESCR_SEG gdt[3]; /* we will have only 4 segments null, k-code,k-data,k-stack*/
	GDTR gdtr;        //our GDT register

/* Creates a descriptor segment for a GDT*/
	void setup_entry (DESCR_SEG *item, unsigned int base, unsigned int limit, unsigned char access, unsigned char attribs)
	{	
		item->base_l = base & 0xFFFF;
		item->base_m = (base >> 16) & 0xFF;
		item->base_h = base >> 24;
		item->limit = limit & 0xFFFFFFFF;
		item->attribs = attribs | ((limit >> 16) & 0x0F);
		item->access = access;
	}
	void load_gdt(GDTR *mgdtr)
	{
		__asm__ __volatile__ ("lgdt (%0)"::"p"(mgdtr));
		
		__asm__ __volatile__("movl $0x10, %eax\n"
        	  "movw %ax, %ss\n"
        	  "movw %ax, %ds\n"
        	  "movw %ax, %es\n"
        	  "movw %ax, %fs\n"
        	  "movw %ax, %gs");
		__asm__ __volatile__("ljmp $8, $__flush\n"
        	  "__flush:");
	}	
	void setup()
	{
		cout<<"setting up GDT\n";	
		/* 0x00 -- null descriptor */
  		setup_entry (&gdt[0], 0, 0, 0, 0);
	  	/* 0x08 -- code segment descriptor */
  		setup_entry (&gdt[1], 0, 0xFFFFFFFF, ACS_CODE, 0xCF);
  		/* 0x10 -- data segment descriptor */
  		setup_entry (&gdt[2], 0, 0xFFFFFFFF, ACS_DATA, 0xCF);
		gdtr.base=(unsigned long) &gdt;
		gdtr.limit=(sizeof(DESCR_SEG) * 3) - 1;
		/* GDT loaded*/
        	load_gdt(&gdtr);
		cout<<"\tGDT Loaded\n";
	}
};
