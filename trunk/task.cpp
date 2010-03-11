//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2009//
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// TASK Implementation                                  //
// Taken from                                           //
// http://hosted.cjmovie.net/TutMultitask.htm           //
//////////////////////////////////////////////////////////
#include "kheap.h"
#include "OStream.h"
#include "idt.h"
using namespace IDT;
using namespace String;
typedef struct thread{
	unsigned int id;
	regs *r;           //the regs *r, is obtained from the 
                           //handler function
}thread;

thread threads[2];		

int current_task=-1;// no task is started.

// this function will create a task and add to the threads[]
void create_thread(int id,void (*mythread)())
{
	threads[id].r=(IDT::regs *)kmalloc(sizeof(IDT::regs));
	threads[id].id=id;
	threads[id].r->esp= (unsigned long int)kmalloc(512)+512;
	threads[id].r->gs = 0x10;
	threads[id].r->fs = 0x10;
	threads[id].r->es = 0x10;
	threads[id].r->ds = 0x10;
	threads[id].r->edi=0;
	threads[id].r->esi=0;
	threads[id].r->ebp=0;
	threads[id].r->esp=0;
	threads[id].r->ebx=0;
	threads[id].r->edx=0;
	threads[id].r->ecx=0;
	threads[id].r->eax=0;
	threads[id].r->eip=(unsigned long)mythread;
	//threads[id].err_code=;
	threads[id].r->cs=0x8;
	threads[id].r->eflags=0x0200;
	/*				 //to the stack for us
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
	*/
}

extern "C" { extern unsigned int read_eip();}			
//Switch between our two tasks
//Notice how we get the old esp from the ASM code
//It's not a pointer, but we actually get the ESP value
//That way we can save it in our task structure

IDT::regs *task_switch(IDT::regs *r)
{
		
	unsigned int ebp;	
	if(current_task != -1)
	{ 
		//Were we even running a task?
		threads[current_task].r->esp = r->esp; //Save the new esp for 
		asm volatile("mov %%ebp, %0" : "=r"(ebp));		
		threads[current_task].r->ebp = ebp;
		threads[current_task].r->eip = read_eip();
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
 
	return threads[current_task].r; //Return new stack pointer to ASM
}


void thread1()
{
	for(;;)
	{	
		cout.gotoxy(5,70);
		cout<<"hello ";
	}
}
void thread2()
{
	for(;;)
	{
		cout.gotoxy(5,70);
		cout<<"World ";
	}
}

void init_tasks()
{
	
	create_thread(0,thread1);
	create_thread(1,thread2);
}


