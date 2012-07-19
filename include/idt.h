//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2012//
// ashok.s.das@gmail.com          GNU GPL-V2            //
//////////////////////////////////////////////////////////
// IDT header                                           //
//                                                      //
//////////////////////////////////////////////////////////
#ifndef __IDT_H__
#define __IDT_H__ 

#include "OStream.h"
namespace IDT{ //IDT is also global so there is no point for creating a class for it 
	typedef struct IDT_entry_struc
		{
			unsigned short base_low;
			unsigned short sel;
			unsigned char unused;
			unsigned char flags;
			unsigned short base_high;
		} __attribute__((packed)) IDT_entry;
	typedef struct IDTR_struc
		{
			unsigned short limit;
			unsigned int base;
		} __attribute__((packed)) IDTR;
	typedef struct regs
		{	
			unsigned long gs, fs, es, ds;
			unsigned long edi, esi, ebp, esp, ebx, edx, ecx, eax;
			unsigned long int_no, err_code;
			unsigned long eip, cs, eflags;//, useresp, ss;	
		} regs;
	typedef void (*isrfunc_t)(regs *);

		
		
		
		void load(IDTR *midtr);
		
		
		void setup();
		void set_gate(unsigned char num, isrfunc_t func, unsigned short segment, unsigned char flags);
		void install_handler(int isr, isrfunc_t my_handler);
		void uninstall_handler(int isr);
		void dump_isr_routines();
}; //end namespace IDT



#endif

