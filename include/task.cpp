//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2009//
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// TASK Implementation                                  //
// Taken from                                           //
// http://hosted.cjmovie.net/TutMultitask.htm           //
//////////////////////////////////////////////////////////
#include "kheap.h"

typedef struct thread{
	unsigned long esp0;
	unsigned long esp3;
}thread;
thread threads[2]; //we will have 2 tasks for the timebeing.
int current_task=-1;// no task is started.

// this function will create a task and add to the threads[]
void create_thread(int id,void (*mythread)())
{
	unsigned long *stack;
	
	threads[id].esp0=kmalloc(4096);
	stack = (unsigned long*)threads[id].esp0; //This makes a pointer 
						 //to the stack for us
	//First, this stuff is pushed by the processor
	*--stack = 0x0202; //This is EFLAGS
	*--stack = 0x08;   //This is CS, our code segment
	*--stack = (unsigned long)mythread; //This is EIP
//Next, the stuff pushed by 'pusha'
	*--stack = 0; //EDI
	*--stack = 0; //ESI
	*--stack = 0; //EBP
	*--stack = 0; //Just an offset, no value
	*--stack = 0; //EBX
	*--stack = 0; //EDX
	*--stack = 0; //ECX
	*--stack = 0; //EAX
//Now these are the data segments pushed by the IRQ handler
	*--stack = 0x10; //DS
	*--stack = 0x10; //ES
	*--stack = 0x10; //FS
	*--stack = 0x10; //GS
	threads[id].esp0 = (unsigned long)stack; //Update the stack pointer
}

//Switch between our two tasks
//Notice how we get the old esp from the ASM code
//It's not a pointer, but we actually get the ESP value
//That way we can save it in our task structure
unsigned int task_switch(unsigned long OldEsp)
{
	if(current_task != -1)
	{ 
		//Were we even running a task?
		Threads[current_task].esp0 = OldEsp; //Save the new esp for 
						   //the thread
		//Now switch what task we're on
		if(current_task == 0)
			current_task = 1;
		else 
			current_task = 0;
	}
	else
	{
  		current_task = 0; //We just started multi-tasking, start with task 0
	}
 
	return threads[current_task].esp0; //Return new stack pointer to ASM
}

void thread1()
{
	for(;;)
	{	
		video::gotoxy(1,78);
		video::putchar('\');
	}
}
void thread2()
{
	for(;;)
	{
		video::gotoxy(1,78);
		video::putchar('/');
	}
}

create_thread(0,thread1);
create_thread(1,thread2);

