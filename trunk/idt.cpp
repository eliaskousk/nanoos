//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2009//
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// IDT implementation                                   //
//                                                      //
//////////////////////////////////////////////////////////
#include "idt.h"
#include "string.h"
#include "low-io.h"
namespace IDT{
	//Our Isrs
extern "C" {
	extern void _isr0(regs *);
	extern void _isr1(regs *);
	extern void _isr2(regs *);
	extern void _isr3(regs *);
	extern void _isr4(regs *);
	extern void _isr5(regs *);
	extern void _isr6(regs *);
	extern void _isr7(regs *);
	extern void _isr8(regs *);
	extern void _isr9(regs *);
	extern void _isr10(regs *);
	extern void _isr11(regs *);
	extern void _isr12(regs *);
	extern void _isr13(regs *);
	extern void _isr14(regs *);
	extern void _isr15(regs *);
	extern void _isr16(regs *);
	extern void _isr17(regs *);
	extern void _isr18(regs *);
	extern void _isr19(regs *);
	extern void _isr20(regs *);
	extern void _isr21(regs *);
	extern void _isr22(regs *);
	extern void _isr23(regs *);
	extern void _isr24(regs *);
	extern void _isr25(regs *);
	extern void _isr26(regs *);
	extern void _isr27(regs *);
	extern void _isr28(regs *);
	extern void _isr29(regs *);
	extern void _isr30(regs *);
	extern void _isr31(regs *);

	static const char *exception_messages[32] =
	{
		"Division By Zero Exception",	
	 	"Debug Exception",
	 	"Non Maskable Interrupt Exception",
	 	"Breakpoint Exception",
		"Into Detected Overflow Exception",
	 	"Out of Bounds Exception",
	 	"Invalid Opcode Exception",
	 	"No Coprocessor Exception",
	 	"Double Fault Exception",	
	 	"Coprocessor Segment Overrun Exception", 	
	 	"Bad TSS Exception",
	 	"Segment Not Present Exception",
	 	"Stack Fault Exception",
	 	"General Protection Fault Exception", 	
	 	"Page Fault Exception",
	 	"Unknown Interrupt Exception", 	
	 	"Coprocessor Fault Exception",	
	 	"Alignment Check Exception", 	
	 	"Machine Check Exception", 	
		"Reserved Exceptions",
		"Reserved Exceptions",
		"Reserved Exceptions",
		"Reserved Exceptions",
		"Reserved Exceptions",
		"Reserved Exceptions",
		"Reserved Exceptions",
		"Reserved Exceptions",
		"Reserved Exceptions",
		"Reserved Exceptions",
		"Reserved Exceptions",
		"Reserved Exceptions",
		"Reserved Exceptions"
	};
	isrfunc_t isr_routines[32]={
				0,0,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0
			      };
	void dump_regs(regs *r)
	{
		cout.flags(hex|showbase);
		cout<<"=================CPU State==================\n";
		cout<<"EAX= "<<r->eax<<" EBX= "<<r->ebx<<"\n";
		cout<<"ECX= "<<r->ecx<<" EDX= "<<r->edx<<"\n";
		cout<<"ESP= "<<r->esp<<" EBP= "<<r->ebp<<"\n";
		cout<<"ESI= "<<r->esi<<" EDI= "<<r->edi<<"\n";
		cout<<"EIP= "<<r->eip<<" CS=  "<<r->cs<<"\n";
		cout<<"DS=  "<< r->ds<<" ES=  "<<r->es<<"\n";
		cout<<"FS=  "<< r->fs<<" GS=  "<<r->gs<<"\n";
		cout<<"SS=  "<< r->ss<<"\n";
		cout<<"Interrupt# = "<<r->int_no<<"\n";
		cout<<"Err.Code= "<<r->err_code<<"\n";
		cout<<"e-Flags= "<<r->eflags<<"\n";
		cout<<"Usr.Esp= "<<r->useresp<<"\n";
		cout<<"============================================\n";
		/*		
		for(int i=0,j=0;i<64;i++,j++)
		{
			if(j==8)
			{
				cout<<"\n";
				j=0;
			}
			cout<<*(unsigned int*)r->esp++<<" ";
		}
		cout<<"\n";*/
	}
	void _fault_handler(struct regs *r)
	{
		isrfunc_t my_handler;
		my_handler=isr_routines[r->int_no];
		if (my_handler)
		{
			my_handler(r);
		}
		else		    
		/* Is this a fault whose number is from 0 to 31? */
    		if (r->int_no < 32)
    		{
        		/* Display the description for the Exception that occurred.
        		 *  In this tutorial, we will simply halt the system using an
        		 *  infinite loop */
			cout.SetBackColour(BLUE);
			cout.clear();
			dump_regs(r);
        		cout<<exception_messages[r->int_no];
			cout<<" System Halted!\n";
			halt();
        		//for (;;);
    		}
	}



}//extern C

	IDTR this_idtr; //our IDT register
	IDT_entry idt[256]; //our IDT

	void set_gate(unsigned char num, isrfunc_t func, unsigned short segment, unsigned char flags)
	{
		unsigned int addr = (unsigned int)func;
		idt[num].base_low = addr & 0xFFFF;
		idt[num].base_high = (addr >> 16) & 0xFFFF;
		idt[num].sel = segment;
		idt[num].unused = 0;
		idt[num].flags = flags;
	}
/*void set_handler(int intnum, isrfunc_t function)
{
	handlers[intnum] = function;
}*/

	void load(IDTR *midtr)
	{
		__asm__ __volatile__ ("lidt (%0)"::"p"(midtr));
	}
/*void test_int0()
{
	kprintf("interrupt 0 occoured\n");
}*/
//IDT::IDT()
	void setup()
	{
		this_idtr.limit=(sizeof(IDT_entry)*256)-1;
		this_idtr.base=(unsigned int)&idt;
		String::memset((void*)&idt,0,sizeof(IDT_entry)*256); //zero out the idt table
		// try to add isr routines here
		             //intnum  	func    codesel Present Ring 0 
		set_gate(0,  _isr0,  0x08, 0x8E);
		set_gate(1,  _isr1,  0x08, 0x8E);
		set_gate(2,  _isr2,  0x08, 0x8E);
		set_gate(3,  _isr3,  0x08, 0x8E);
		set_gate(4,  _isr4,  0x08, 0x8E);
		set_gate(5,  _isr5,  0x08, 0x8E);
		set_gate(6,  _isr6,  0x08, 0x8E);
		set_gate(7,  _isr7,  0x08, 0x8E);
		set_gate(8,  _isr8,  0x08, 0x8E);
		set_gate(9,  _isr9,  0x08, 0x8E);
		set_gate(10, _isr10, 0x08, 0x8E);
		set_gate(11, _isr11, 0x08, 0x8E);
		set_gate(12, _isr12, 0x08, 0x8E);
		set_gate(13, _isr13, 0x08, 0x8E);
		set_gate(14, _isr14, 0x08, 0x8E);
		set_gate(15, _isr15, 0x08, 0x8E);
		set_gate(16, _isr16, 0x08, 0x8E);
		set_gate(17, _isr17, 0x08, 0x8E);
		set_gate(18, _isr18, 0x08, 0x8E);
		set_gate(19, _isr19, 0x08, 0x8E);
		set_gate(20, _isr20, 0x08, 0x8E);
		set_gate(21, _isr21, 0x08, 0x8E);
		set_gate(22, _isr22, 0x08, 0x8E);
		set_gate(23, _isr23, 0x08, 0x8E);
		set_gate(24, _isr24, 0x08, 0x8E);
		set_gate(25, _isr25, 0x08, 0x8E);
		set_gate(26, _isr26, 0x08, 0x8E);
		set_gate(27, _isr27, 0x08, 0x8E);
		set_gate(28, _isr28, 0x08, 0x8E);
		set_gate(29, _isr29, 0x08, 0x8E);
		set_gate(30, _isr30, 0x08, 0x8E);
		set_gate(31, _isr31, 0x08, 0x8E);
		load(&this_idtr); //our IDT table is loaded on to the processor
		cout<<"IDT Loaded\n";
	}

	void install_handler(int isr, isrfunc_t my_handler)
	{
		isr_routines[isr] = my_handler;
	}

	/* This clears the handler for a given IRQ */
	void uninstall_handler(int isr)
	{
		isr_routines[isr] = 0;
	}
	void dump_isr_routines()
	{
		int i;
		for (i=0;i<32;i++)
		{
			if(isr_routines[i]!=0)		
			cout<<"isr ";
			cout<< i<<'\t';
		}
		cout<<"\n";
	}
}; //end namespace IDT

