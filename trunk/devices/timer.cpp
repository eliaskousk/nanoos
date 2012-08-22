//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008-2010 //
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// Timer implementation                                 //
//                                                      //
//////////////////////////////////////////////////////////
/*Timer handler for our OS*/
#include "low-io.h"
#include "OStream.h"
#include "timer.h"
#include "irq.h"
TIMER *my_timer;
//extern "C"{unsigned int task_switch(void *sp);}
	/* to set timer frequency IRQ0 frequency */
	void TIMER::timer_phase(int hz)
	{
		cout<<" Frequency = "<<hz<<" per second\n";
		int divisor = 1193180 / hz;       /* Calculate our divisor */
		outportb(0x43, 0x36);             /* Set our command byte 0x36 */
		outportb(0x40, divisor & 0xFF);   /* Set low byte of divisor */
		outportb(0x40, divisor >> 8);     /* Set high byte of divisor */
	}

	int TIMER::timer_ticks=0;
			

	void TIMER::setup()
	{
		cout<<"Timer Initializing\n";    		
		timer_phase(1000);
		cout<<"Installing handler \n"; 
		IRQ::install_handler(0,TIMER::timer_handler);
		IRQ::enable_irq(0);
	}
	void TIMER::set_ticks(int t)
	{ 
		timer_ticks=t;
	}
	int TIMER::get_ticks()
	{	
		return timer_ticks;
	}
	void TIMER::sleep(int ms)
	{
		int cur;
		cur=get_ticks();
		while(cur+ms-get_ticks());
	}
	
	void TIMER::timer_handler(void *sp)	
	{		
		timer_ticks++;
	}	

