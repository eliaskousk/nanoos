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
#include "irq.h"
#include "low-io.h"
#include "task.h"
#include "shell.h"
#include "timer.h"
using namespace IDT;
using namespace String;
using namespace IRQ;
typedef void (*func)(void *earg);
thread threads[32]; //we will have maximum 32 tasks running.		

static int current_task=-1;// no task is started.
static int tid=1;  //tid=0 is for idle process
// this function will create a task and add to the threads[]
volatile int tasker=0;
extern thread *g_current;
void thread_stack_push(thread *t, unsigned int val)
{
	t->stack_top-=4;
	*((unsigned int*)t->stack_top)=val;
}
void thread_run(func entry,void *args)
{
	cout<<"In thread_run\n";
	enable();	
	entry(args);
	disable();
	cout<<"Exit!!!\n";
	threads[current_task].state=FINISHED;
	enable();
}
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
	threads[id].stack= new unsigned char[1024];
	if(threads[id].stack==NULL)
	{
		cout<<"Can't allocate memory for stack\n";
		enable();
		return;
	}
	threads[id].stack_top=(unsigned int)threads[id].stack+1024;
	threads[id].arg=args;
	threads[id].id=tid;
	++tid;
	
	thread_stack_push(&threads[id],(unsigned int)args);
	//thread_stack_push(&threads[id],(unsigned int)entry);
	//thread_stack_push(&threads[id],0);
	threads[id].stack_top-=sizeof(IDT::regs);
	threads[id].r=(IDT::regs*)threads[id].stack_top;	
	threads[id].r->gs = 0x10;
	threads[id].r->fs = 0x10;
	threads[id].r->es = 0x10;
	threads[id].r->ds = 0x10;
	threads[id].r->edi=0;
	threads[id].r->esi=0;
	threads[id].r->ebp=0;
	threads[id].r->esp=threads[id].stack_top;
	threads[id].r->ebx=0;
	threads[id].r->edx=0;
	threads[id].r->ecx=0;
	threads[id].r->eax=0;
	threads[id].r->eip=(unsigned int)entry;
	//threads[id].err_code=0x;
	threads[id].r->cs=0x8;
	threads[id].r->eflags=0x0200;
	threads[id].state=CREATED;	
	enable();
}

extern "C" {extern unsigned int read_eip();};			
//Switch between our two tasks
//Notice how we get the old esp from the ASM code
//It's not a pointer, but we actually get the ESP value
//That way we can save it in our task structure
extern "C" {

void task_switch(void *sp)
{
		
	//unsigned long ebp,esp,sz;
	//cout<<"task_switch\n";
	IDT::regs *r=(IDT::regs *)sp;
	if(current_task != -1)
	{ 
		
		disable();
		memcpy(threads[current_task].r,r,sizeof(IDT::regs));
		++current_task;
		while((threads[current_task].state==BLOCKED)||(threads[current_task].state==FINISHED))
			++current_task;			
		current_task%=32;
		memcpy(r,threads[current_task].r,sizeof(IDT::regs));
		end_irq(r);
		enable();		
	}
	else
	{
		disable();		
		current_task = 0; //We just started multi-tasking, start with task 0
		memcpy(r,threads[current_task].r,sizeof(IDT::regs));
		end_irq(r);		
		enable();
	}
}

} //extern C
//TIMER *tm=TIMER::Instance();
void idle(void *)
{
	for(;;){
	//tm->sleep(10);
	}
}
void thread1(void *n)
{
	int x,y;
	for(;;)
	{	
		disable();
		x=cout.GetX();
		y=cout.GetY();		
		cout.gotoxy(70,5);
		cout<<"\\\n"<<"   "<<*(int*)n<<"   \n";
		cout.gotoxy(x,y);
		enable();
		//tm->sleep(10);
	}
}
void thread2(void *n)
{
	int x,y;	
	for(;;)
	{
		disable();
		x=cout.GetX();
		y=cout.GetY();		
		cout.gotoxy(70,5);
		cout<<"|\n"<<"   "<<*(int*)n<<"   \n";
		cout.gotoxy(x,y);
		enable();
		//tm->sleep(10);
	}
}
void thread3(void *n)
{
	int x,y;	
	for(;;)
	{
		disable();
		x=cout.GetX();
		y=cout.GetY();
		cout.gotoxy(70,5);
		cout<<"/\n"<<"   "<<*(int*)n<<"   \n";
		cout.gotoxy(x,y);
		enable();
		//tm->sleep(10);
	}
}
void thread4(void *n)
{
	int x,y;	
	for(;;)
	{
		disable();
		x=cout.GetX();
		y=cout.GetY();		
		cout.gotoxy(70,5);
		cout<<"-\n"<<"   "<<*(int*)n<<"   \n";
		cout.gotoxy(x,y);
		enable();
		//tm->sleep(10);
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
	create_thread(thread1,&current_task);
	threads[1].state=RUNNING;
	create_thread(thread2,&current_task);
	threads[2].state=RUNNING;
	create_thread(thread3,&current_task);
	threads[3].state=RUNNING;
	create_thread(thread4,&current_task);
	threads[4].state=RUNNING;
	//cout<<*((unsigned int*)&current_task)<<"\n";
	//create_thread(thread5,NULL);
	//threads[5].state=RUNNING;
	tasker=1;
}


