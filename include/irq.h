//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2012//
// ashok.s.das@gmail.com          GNU GPL-V2            //
//////////////////////////////////////////////////////////
// IRQ header                                           //
//                                                      //
//////////////////////////////////////////////////////////
//irq.h
#ifndef __IRQ_H__
#define __IRQ_H__

#include "idt.h"

namespace IRQ{ //IRQs are global
	typedef void(*irqfunc_t)(void *sp);	
	void setup();
	void remap(void);
	void dump_irq_routines(void);
	void install_handler(int irq, irqfunc_t my_handler);
	void uninstall_handler(int irq);
	unsigned short get_irq_mask();
	unsigned short get_irq_mask_low();
	void set_irq_mask(unsigned short mask);
	void enable_irq(int irq);
	void disable_irq(int irq);
	extern "C"{ void end_irq(IDT::regs *r);};
}; //end namespace IRQ


#endif
