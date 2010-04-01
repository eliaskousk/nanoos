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
#include "task.h"
#include "shell.h"
using namespace IDT;
using namespace String;

typedef void (*func)(void *earg);
thread threads[32]; //we will have maximum 32 tasks running.		

static int current_task=-1;// no task is started.
static int tid=1;  //tid=0 is for idle process
// this function will create a task and add to the threads[]
volatile int tasker=0;
void create_thread(func entry,void *args)
{
	disable();
	static unsigned int id=0;
	if (tid>=32)
	{
		tid=0;
		while(threads[tid].state!=FINISHED)
			tid++;
	}
	if(tid>=32)
	{
		cout<<"No free slot: tid>=32 possible\n";
		enable();
		return;
	}
	id=tid;
	IDT::regs *tr=(IDT::regs*)new(IDT::regs);
	if(!tr)
	{
		cout<<"Can't allocate memory for regs\n";
		halt();
	} 
	memset((void*)tr,0,sizeof(IDT::regs));
	threads[id].stack= new unsigned char[1024];
	if(threads[id].stack==NULL)
	{
		cout<<"Can't allocate memory for stack\n";
		halt();
	}
	threads[id].arg=args;
	
	threads[id].r=tr;	
	threads[id].id=tid;
	++tid;
	threads[id].r->esp= (unsigned int)threads[id].stack+1024;
		
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
	threads[id].r->eip=(unsigned long)entry;
	//threads[id].err_code=0x;
	threads[id].r->cs=0x8;
	threads[id].r->eflags=0x0202;
	memcpy((unsigned int *)threads[id].r->esp,(unsigned int*)threads[id].r,sizeof(IDT::regs));
	threads[id].state=CREATED;	
	
	enable();
}

extern "C" {extern unsigned int read_eip();};			
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
		//asm("cli");
		disable();
		memcpy(threads[current_task].r,r,sizeof(IDT::regs));
		++current_task;
		while((threads[current_task].state==BLOCKED)||(threads[current_task].state==FINISHED))
			++current_task;			
		current_task%=32;
		memcpy(r,threads[current_task].r,sizeof(IDT::regs));		
		//asm("sti");
		enable();		
	}
	else
	{
		//asm("cli");
		disable();		
		current_task = 0; //We just started multi-tasking, start with task 0
		memcpy(r,threads[current_task].r,sizeof(IDT::regs));		
		//asm("sti");
		enable();
	}
	
	//outportb(0x20, 0x20);
	//outportl(0x80,inportl(0x80));
}
} //extern C
void idle(void *)
{
	for(;;){};
}
void thread1(void *)
{
	int x,y;
	for(;;)
	{	
		disable();
		x=cout.GetX();
		y=cout.GetY();		
		cout.gotoxy(70,5);
		cout<<"\\\n";
		cout.gotoxy(x,y);
		enable();
	}
}
void thread2(void *)
{
	int x,y;	
	for(;;)
	{
		disable();
		x=cout.GetX();
		y=cout.GetY();		
		cout.gotoxy(70,5);
		cout<<"|\n";
		cout.gotoxy(x,y);
		enable();
	}
}
void thread3(void *)
{
	int x,y;	
	for(;;)
	{
		disable();
		x=cout.GetX();
		y=cout.GetY();
		cout.gotoxy(70,5);
		cout<<"/\n";
		cout.gotoxy(x,y);
		enable();
	}
}
void thread4(void *)
{
	int x,y;	
	for(;;)
	{
		disable();
		x=cout.GetX();
		y=cout.GetY();		
		cout.gotoxy(70,5);
		cout<<"-\n";
		cout.gotoxy(x,y);
		enable();
	}
}
void thread5(void *)
{
	shell *s=new shell();
	s->start();
}
void init_tasks()
{
	cout<<"initializing Tasks\n";
	for(int i=0;i<32;i++)
	{
		create_thread(idle,NULL);
		threads[i].state=FINISHED;
	}
	threads[0].state=RUNNING;
	create_thread(thread1,NULL);
	threads[1].state=RUNNING;
	create_thread(thread2,NULL);
	threads[2].state=RUNNING;
	create_thread(thread3,NULL);
	threads[3].state=RUNNING;
	create_thread(thread4,NULL);
	threads[4].state=RUNNING;
	create_thread(thread5,NULL);
	threads[5].state=RUNNING;
	tasker=1;
}


