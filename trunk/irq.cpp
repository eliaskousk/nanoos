//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008-2010 //
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// IRQ Implementation                                   //
//                                                      //
//////////////////////////////////////////////////////////
#include "low-io.h"
#include "irq.h"
#include "OStream.h"
//extern void kprintf(const char *fmt,...);
namespace IRQ{
static unsigned short volatile IRQ_mask;
extern "C" {
		irqfunc_t irq_routines[16] =
		{
		    0, 0, 0, 0, 0, 0, 0, 0,
		    0, 0, 0, 0, 0, 0, 0, 0
		};
	void end_irq(IDT::regs *r)
	{
		unsigned char irq,cmd;
		irq=r->int_no;
		cmd=0x60 | (irq & 7);
		if(irq<8)
			outportb(0x20,0x20);
		else
		{
			outportb(0xA0,0x20);
			outportb(0x20,0x20);
		}
	}
	bool is_spurious7()
	{
		outportb(0x20,0x0B);
		if(inportb(0x20)&0x80)
			return true;
		return false;
	}	
	/* Each of the IRQ ISRs point to this function, rather than
	*  the 'fault_handler' in 'isrs.c'. The IRQ Controllers need
	*  to be told when you are done servicing them, so you need
	*  to send them an "End of Interrupt" command (0x20). There
	*  are two 8259 chips: The first exists at 0x20, the second
	*  exists at 0xA0. If the second controller (an IRQ from 8 to
	*  15) gets an interrupt, you need to acknowledge the
	*  interrupt at BOTH controllers, otherwise, you only send
	*  an EOI command to the first controller. If you don't send
	*  an EOI, you won't raise any more IRQs 
	*/
	void _irq_handler(void *sp)
	{
		IDT::regs *r=(IDT::regs *)sp;		
		irqfunc_t my_handler;
		
		/* Find out if we have a custom handler to run for this
		*  IRQ, and then finally, run it */
		my_handler = irq_routines[r->int_no - 32];
		if (my_handler)
		{
			my_handler(sp);
		}
		else
		{
			cout<<"No handlers for IRQ "<< (int)r->int_no-32<< "installed\n";
		}
		end_irq(r);
		//outportl(0x80,inportl(0x80)); //iodelay
	}

	/* These are own ISRs that point to our special IRQ handler
	*  instead of the regular 'fault_handler' function */
	extern void _irq0(IDT::regs *);
	extern void _irq1(IDT::regs *);
	extern void _irq2(IDT::regs *);
	extern void _irq3(IDT::regs *);
	extern void _irq4(IDT::regs *);
	extern void _irq5(IDT::regs *);
	extern void _irq6(IDT::regs *);
	extern void _irq7(IDT::regs *);
	extern void _irq8(IDT::regs *);
	extern void _irq9(IDT::regs *);
	extern void _irq10(IDT::regs *);
	extern void _irq11(IDT::regs *);
	extern void _irq12(IDT::regs *);
	extern void _irq13(IDT::regs *);
	extern void _irq14(IDT::regs *);
	extern void _irq15(IDT::regs *);
}// c declarationc end


	/* This installs a custom IRQ handler for the given IRQ */
	void install_handler(int irq, irqfunc_t my_handler)
	{
		irq_routines[irq] = my_handler;
	}

	/* This clears the handler for a given IRQ */
	void uninstall_handler(int irq)
	{
		irq_routines[irq] = 0;
	}

	/* Normally, IRQs 0 to 7 are mapped to entries 8 to 15. This
	*  is a problem in protected mode, because IDT entry 8 is a
	*  Double Fault! Without remapping, every time IRQ0 fires,
	*  you get a Double Fault Exception, which is NOT actually
	*  what's happening. We send commands to the Programmable
	*  Interrupt Controller (PICs - also called the 8259's) in
	*  order to make IRQ0 to 15 be remapped to IDT entries 32 to
	*  47 */
	void remap(void)
	{
		outportb(0x20, 0x11);
		outportb(0xA0, 0x11);
    		outportb(0x21, 0x20);
		outportb(0xA1, 0x28);
		outportb(0x21, 0x04);
		outportb(0xA1, 0x02);
		outportb(0x21, 0x01);
		outportb(0xA1, 0x01);
		outportb(0x21, 0xFF);
		outportb(0xA1, 0xFF);
		IRQ_mask=0xFFFF;
	}
	void spurious_irq7(void *sp)
	{
		//////do nothing
		//asm("iret");
	}
/* We first remap the interrupt controllers, and then we install
*  the appropriate ISRs to the correct entries in the IDT. This
*  is just like installing the exception handlers */
	void setup()
	{
		cout<<"Remaping vectors\n";
		remap();
		
		IDT::set_gate(32, _irq0, 0x08, 0x8E);
		IDT::set_gate(33, _irq1, 0x08, 0x8E);
		IDT::set_gate(34, _irq2, 0x08, 0x8E);
		IDT::set_gate(35, _irq3, 0x08, 0x8E);
		IDT::set_gate(36, _irq4, 0x08, 0x8E);
		IDT::set_gate(37, _irq5, 0x08, 0x8E);
		IDT::set_gate(38, _irq6, 0x08, 0x8E);
		IDT::set_gate(39, _irq7, 0x08, 0x8E);
		IDT::set_gate(40, _irq8, 0x08, 0x8E);
		IDT::set_gate(41, _irq9, 0x08, 0x8E);
		IDT::set_gate(42, _irq10, 0x08, 0x8E);
		IDT::set_gate(43, _irq11, 0x08, 0x8E);
		IDT::set_gate(44, _irq12, 0x08, 0x8E);
		IDT::set_gate(45, _irq13, 0x08, 0x8E);
		IDT::set_gate(46, _irq14, 0x08, 0x8E);
    		IDT::set_gate(47, _irq15, 0x08, 0x8E);
		cout<<"IRQ initialized\n";
		install_handler(7,spurious_irq7);
     		//outportb(0x21, 0xFF);
		//outportb(0xA1, 0xFB);

	}
	void dump_irq_routines()
	{
		int i;
		for (i=0;i<16;i++)
		{
			if(irq_routines[i]!=0)
			cout<<"irq "<< i <<"\t";
		}
		cout<<"\n";
	}
	irqfunc_t get_vect(int irq_num)
	{
		return irq_routines[irq_num];
	}
	void set_vect(int irq_num,irqfunc_t vect)
	{
		irq_routines[irq_num]=vect;
	}
	// return current IRQ mask
	unsigned short get_irq_mask()
	{
		return IRQ_mask;
	}
	unsigned short get_irq_mask_low()
	{
		unsigned short msk;
		msk=inportb(0xA1);
		msk=msk<<8;
		msk+=inportb(0x21);
		return msk;
	}
	// set IRQ mask 
	void set_irq_mask(unsigned short mask)
	{
		unsigned short old_mask;
		unsigned char cmask;
		old_mask=get_irq_mask();
		cmask=mask&0xff;
		if(cmask!=(old_mask&0xff)) 
			outportb(0x21,cmask); // mask master controller
		cmask=(mask>>8)&0xff;
		if(cmask!=((old_mask>>8)&0xff))
			outportb(0xA1,cmask); // mask slave controller
		IRQ_mask=mask;
		//outportl(0x80,inportl(0x80)); //iodelay
	}
	// enable a perticular IRQ
	void enable_irq(int irq)
	{
		unsigned short set;
		set=~(1<<irq);
		set&=get_irq_mask();
		set_irq_mask(set);
	}
	// disable or stop a perticular IRQ
	void disable_irq(int irq)
	{
		unsigned short set;
		set=(1<<irq);
		set|=get_irq_mask();
		set_irq_mask(set);
	}
}; //end namespace IRQ


