//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2012//
// ashok.s.das@gmail.com         GNU GPL-V2             //
//////////////////////////////////////////////////////////
// Timer header                                         //
//                                                      //
//////////////////////////////////////////////////////////
#ifndef __TIMER_H__
#define __TIMER_H__
#include "idt.h"
#include "singleton.h"
class TIMER: public Singleton<TIMER>
{ //timer interrupt
		public:
			TIMER(){};
			~TIMER(){};		
			void setup();
		//static int timer_ticks;
			void timer_phase(int hz); //number of ticks in a second IRQ0 frequency
			int get_ticks();
			void set_ticks(int t);
			void sleep(int ms);
			//static void timer_handler(IDT::regs *r);
			static void timer_handler(void *sp);
		private:
			static int timer_ticks;
}; 
extern TIMER *my_timer;
#endif

