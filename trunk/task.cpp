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
#include "low-io.h"
using namespace IDT;
using namespace String;
typedef struct thread{
	unsigned int id;
	regs *r;           //the regs *r, is obtained from the 
                           //handler function
}thread;

thread threads[4];		

static int current_task=-1;// no task is started.

// this function will create a task and add to the threads[]
void create_thread(int id,void (*mythread)())
{
	IDT::regs *tr=(IDT::regs*)new(IDT::regs);
	memset((void*)tr,0,sizeof(IDT::regs));
	//threads[id].r=(IDT::regs *)kmalloc(sizeof(IDT::regs));
	threads[id].r=tr;	
	threads[id].id=id;
	threads[id].r->esp= (unsigned int)kmalloc(1024)+1024;
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
	//threads[id].err_code=0x;
	threads[id].r->cs=0x8;
	threads[id].r->eflags=0x0202;
	memcpy((unsigned int *)threads[id].r->esp,(unsigned int*)threads[id].r,sizeof(IDT::regs));
	//threads[id].r->esp=threads[id].r->esp+sizeof(IDT::regs);	
	cout<<"Createing Task stack at = "<<(unsigned int)threads[id].r->esp<<"\n";
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

extern "C" { extern unsigned int read_eip();
	     extern void cpu_context_restore(IDT::regs *r);
	     extern void cpu_context_save(IDT::regs *r);	
 	}			
//Switch between our two tasks
//Notice how we get the old esp from the ASM code
//It's not a pointer, but we actually get the ESP value
//That way we can save it in our task structure
extern "C" {

void task_switch(IDT::regs *r)
{
		
	//unsigned long ebp,esp,sz;
	//cout<<"task_switch\n";
	if(current_task != -1)
	{ 
		asm("cli");
		memcpy(threads[current_task].r,r,sizeof(IDT::regs));
		++current_task;
		current_task%=4;
		memcpy(r,threads[current_task].r,sizeof(IDT::regs));		
		asm("sti");		
		
	}
	else
	{
		asm("cli");		
		current_task = 0; //We just started multi-tasking, start with task 0
		memcpy(r,threads[current_task].r,sizeof(IDT::regs));		
		asm("sti");
	}
	//outportb(0x20, 0x20);
	//outportl(0x80,inportl(0x80));
}
} //extern C

void thread1()
{
	for(;;)
	{	
		asm("cli");
		cout.gotoxy(40,15);
		cout<<"\\\n";
		asm("sti");
	}
}
void thread2()
{
	for(;;)
	{
		asm("cli");		
		cout.gotoxy(40,15);
		cout<<"|\n";
		asm("sti");
	}
}
void thread3()
{
	for(;;)
	{
		asm("cli");
		cout.gotoxy(40,15);
		cout<<"/\n";
		asm("sti");
	}
}
void thread4()
{
	for(;;)
	{
		asm("cli");
		cout.gotoxy(40,15);
		cout<<"-\n";
		asm("sti");
	}
}
void init_tasks()
{
	cout<<"initializing Tasks\n";
	create_thread(0,thread1);
	create_thread(1,thread2);
	create_thread(2,thread3);
	create_thread(3,thread4);
}


