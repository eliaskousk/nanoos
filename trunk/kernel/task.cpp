//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008-2010 //
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// TASK Implementation                                  //
// Taken from                                           //
// geekOs thread creation mechanism but not taskswitch  //
//////////////////////////////////////////////////////////
#include "kheap.h"
#include "OStream.h"
#include "IStream.h"
#include "idt.h"
#include "irq.h"
#include "low-io.h"
#include "task.h"
#include "shell.h"
#include "timer.h"

using namespace IDT;
using namespace String;
using namespace IRQ;
//#define _DEBUG_ FALSE
//=========================thread_que class def================
bool thread_que::add(thread *t)		// add a thread to the list
{
	thread_node *temp=new thread_node;
	if(temp==NULL)
		return false;
	this->thq->try_lock();
	temp->data=t;
	temp->next=NULL;
	if(head==NULL)
	{
		head=tail=curr=temp;
	}
	else
	{
		tail->next=temp;
		tail=temp;
	}
	num_nodes++;
	this->thq->unlock();
	return true;
};
thread *thread_que::get()		// get next thread from list
{
	this->thq->try_lock();
	if(curr==NULL)
	{
		curr=head;
	}
	thread_node *temp;
	temp=curr;
	curr=curr->next;
	this->thq->unlock();
	return temp->data;
};
thread *thread_que::get(states stat)
{
	thread *t;
	this->thq->try_lock();
again:
	t=get();
	if(t->state==stat)
	{
		this->thq->unlock();
		return t;
	}
	else
		goto again;
}
			
bool thread_que::remove(thread *t)	// remove a thread from list
{
	thread_node *temp1,*temp2;
	this->thq->try_lock();
	temp1=head;
	if(head->data==t)
	{
		head=head->next;
		delete temp1;
		num_nodes--;
		this->thq->unlock();
		return true;
	}
	while(temp1->next)
	{
		temp2=temp1->next;
		if(temp2->data==t)
		{
			temp1->next=temp2->next;
			delete temp2;
			num_nodes--;
			this->thq->unlock();
			return true;
		}
		temp1=temp1->next;
	}
	this->thq->unlock();
	return false;
};

thread *thread_que::get_thread_by_id(unsigned int tid) //get the thread by id
{
	this->thq->try_lock();
	thread_node *temp=head;
	while(temp)
	{
		if(temp->data->id==tid)
		{
			this->thq->unlock();
			return (temp->data);
		}
		temp=temp->next;
	}
	this->thq->unlock();
	return NULL;
}
//==========================thread_que def ends=======================

thread_que *task_q;	// our task que 
thread_que *zombie;	// zombie que, all finished tasks will be here 	
thread_que *wait_q;	// wait que, all thread waiting for either IO/child

thread *curr=NULL;
static int current_task=-1;// no task is started.

volatile int tasker=0;
void wait_ms(unsigned int ms);
void thread_stack_push(thread *t, unsigned int val)
{
	t->stack_top-=4;
	*((unsigned int*)t->stack_top)=val;
}
// if a thread exits then it will be caught by this function 
static void thread_catcher()
{
	//disable();
#ifdef _DEBUG_
	cout.gotoxy(0,22);
	cout<<" thread "<<curr->id<<" exiting\n";
#endif
	// do we have a parent?
	if(curr->pid>0)
	{
#ifdef _DEBUG_ 
		cout.gotoxy(0,23);
		cout<<" it has a parent waiting \n";
#endif
		// yes we have, so we see if it is waiting for us?
		thread *parent=task_q->get_thread_by_id(curr->pid);
		if(!parent) goto no_parent;
		if(parent->state==WAITING)
		parent->state=RUNNING;
		parent->num_child--;
	}
	no_parent:
	//enable();
	// do we have some child who are active
	while(curr->num_child!=0)
	{	
#ifdef _DEBUG_
		cout<<curr->num_child<<" remains\n";
#endif	
		curr->state=WAITING;
	}
	curr->state=FINISHED;
	//curr->max_ticks=1;
	//cout<<"\n";
	for(;;);
}
void thread_run(func entry,unsigned int args)
{
	//cout<<"In thread_run\n";
	enable();	
	entry(args);
	thread_catcher();
}
static unsigned int new_tid()
{
	static unsigned int tid;
	tid++;
	return tid;
}
void wait_on_thread(thread *t)
{
	//if the child is not finished 
	if(t->state!=FINISHED)
	{
	// parent should wait
		curr->state=WAITING;
		curr->timeslice=curr->max_ticks;
	}
	wait_ms(10);
	//while(curr->num_child==0);
}
thread *create_thread(func entry,unsigned int args,PRIO p,bool detached)
{
	// Allocate a thread control structure	
	thread *t=new thread;
	if(!t)
	{
		cout<<"Insufficient memory for Task creation\n";
		return NULL;
	} 
	memset((char*)t,'\0',sizeof(thread));
	t->stack_top=(unsigned int)t->stack+1024; // align the stack_top
	if((curr==NULL)|| detached)
		t->pid=0;          // either detached or first level thread 
	else
	{
		t->pid=curr->id;   // put the parent id
		curr->num_child++; // increase child count
	}	
	t->id=new_tid();			  // get the next tid
	t->timeslice=0;				  // No CPU time consumed
	t->p=p;					  // priority
	t->max_ticks=MAX_SLICE/p;		  // maximum eligible time 
						  // slice for the priority
	thread_stack_push(t,args);		  // push the address of 
						  // thread argument
	thread_stack_push(t,(unsigned int)entry); // push the thread 
						  // function
	thread_stack_push(t,(unsigned int)0);	  // push a fake return 
						  // address for thread func
	t->stack_top-=sizeof(IDT::regs);	  // allign thread context
						  // on the top of the stack
	t->r=(IDT::regs *)t->stack_top;		  // 
	t->r->gs = 0x10;			  // put the context
	t->r->fs = 0x10;			  
	t->r->es = 0x10;
	t->r->ds = 0x10;
	t->r->edi=0;
	t->r->esi=0;
	t->r->ebp=0;
	t->r->esp=t->stack_top;
	t->r->ebx=0;
	t->r->edx=0;
	t->r->ecx=0;
	t->r->eax=0;
	t->r->eip=(unsigned int)thread_run;	  // thread_run will start
						  // our thread function
	//tt->err_code=0x;
	t->r->cs=0x8;
	t->r->eflags=0x0200;
	t->state=CREATED;				
	return (t);				  // return thread strucure
}
thread *get_next_runnable(thread_que *q)
{
	if(q==NULL)
	{
		cout<<"Error\n";
		disable();
		halt();
		for(;;);
	}
	else
	{
		thread *t;
		t=q->get();
		while(1)
		{
			if(t->state==RUNNING)
				return t;
			t=q->get();
		}
	}
}
void _print_state(states stat)
{
	switch(stat)
	{
		case INVALID: cout<<"INV\n";
				break;
		case CREATED: cout<<"CRE\n";
				break;
		case READY:   cout<<"RDY\n";
				break;
		case RUNNING: cout<<"RUN\n";
				break;
		case WAITING: cout<<"WAT\n";
				break;
		case BLOCKED: cout<<"BLC\n";
				break;
		case FINISHED: cout<<"FIN\n";
				break;
	}
}	
void show_tasks(thread_que *t)
{
	int num_tasks=t->get_num_nodes();
	thread *th;
	for(int i=0;i<num_tasks;i++)
	{
		th=t->get();
		cout<<"["<<i<<"] "<<th->id<<" parent-id "<<th->pid;
		cout<<" children "<<th->num_child<<" ";
		_print_state(th->state);
	}
}
void all_tasks()
{
	cout<<"The running...\n";
	show_tasks(task_q);
	cout<<"The Zombies...\n";
	show_tasks(zombie);
}
// Switch between our tasks
// Notice how we get the old esp from the ASM code
// It's not a pointer, but we actually get the ESP value
// That way we can save it in our task structure
extern "C" {

unsigned int task_switch(void *sp)
{
	if(!tasker )			// our task que is not ready
		return(unsigned int)sp; // hence return what ever stack
	else
	{
					// we have a task_q and ...
		if(current_task==-1)	// have we started yet?
		{
			curr=task_q->get();      // start the first task
			curr->state=RUNNING;
			current_task=0;          // mark we already started
			return(curr->stack_top); // return the current task
						 // note we dont save the 
						 // stack when we start the
						 // first task, is it right?
		}
		else			// we have already started
		{
			if(curr->state==CREATED) // first time task is here
				curr->state=RUNNING;
			if(curr->timeslice<curr->max_ticks) 
			{			   // our slice not finished
				curr->timeslice++; // we used one tick more 
				return ((unsigned int)sp); //return stack
			}
			else
				curr->stack_top=(unsigned int) sp; //well our slice
							   // finished
		
			curr=task_q->get();	     // get next task
			if(curr->state==FINISHED) // is the task finished?
			{
				zombie->add(curr);    // put in deads				
				task_q->remove(curr); // remove the task 
						      // from task que
				curr=task_q->get();   // get next task as
						      // curr
			}
			if((curr->state==BLOCKED) || (curr->state==WAITING))
			{
				curr=task_q->get();  // blocked or waiting
						     // get next
			}
			
			curr->timeslice=0;	     // mark slice unused
			return (curr->stack_top);    // return stack
		}
	}
}		

} //extern C

 
TIMER *tm=TIMER::Instance();
void wait_ms(unsigned int ms)
{
	int last;
	last=tm->get_ticks();
	while(tm->get_ticks()<last+ms);
}

void idle(unsigned int )
{
	for(;;)
	{
#ifdef _DEBUG_
		cout.gotoxy(0,24);
		if(task_q->get_num_nodes()>0)
			cout<<"running="<<task_q->get_num_nodes();
		if(zombie->get_num_nodes()>0)
			cout<<" zombies="<<zombie->get_num_nodes();
#endif
		//cout<<"Idle\n";
	}
}

int a=10,b=20,c=30,d=40;
void thread1(unsigned int n)
{
	int x,y;
	//tm->sleep(50);
	for(;;)
	{	
		//disable();
		x=cout.GetX();
		y=cout.GetY();		
		//cout.gotoxy(70,5);
		//cout<<"\\\n"<<"   "<<*(int *)n<<"   \n";
		
		//cout.gotoxy(x,y);
		//enable();
		//tm->sleep(100);
	}
}
void thread2(unsigned int n)
{
	int x,y;
	//tm->sleep(50);
	//for(int i=0;i<5;i++)
	{
		//disable();
		x=cout.GetX();
		y=cout.GetY();	
		cout<<" value at "<<n<<" is "<<*(int *)n<<"\n";
		*(int *)n=20;
		cout<<" value at "<<n<<" is "<<*(int *)n<<"\n";
		//cout.gotoxy(70,5);
		//cout<<"|\n"<<"   "<<*(int *)n<<"   \n";
		//cout.clear();
		//////**(char **)n='p';
		//cout.gotoxy(x,y);
		//enable();
		//tm->sleep(100);
	}
}
void thread3(unsigned int n)
{
	int x,y;
	//wait_ms(5);
	for(int i=0;;i++)
	{
		//disable();
		x=cout.GetX();
		y=cout.GetY();
		*(int *)n=999;
		//cout.gotoxy(70,5);
		//cout<<"-\n"<<"   "<<*(int *)n<<"   \n";
		//cout.gotoxy(x,y);
		//enable();
		//wait_ms(10);
	}
}

void thread4(unsigned int n)
{
	int x,y;
	int z=10,k=100;	
	//wait_ms(5);
	//for(;;)
	{
		//disable();
		x=cout.GetX();
		y=cout.GetY();
				
		//cout.gotoxy(70,5);
		//cout<<"/\n"<<"   "<<*(int *)n<<"   \n";
		//cout.gotoxy(x,y);
		//enable();
		//cout<<"input an int ";
		//cin>>z;
		cout<<" address of z is "<<(unsigned)&z<<"\n";
		cout<<" address of k is "<<(unsigned)&k<<"\n";
		cout<<"z="<<z<<"\n";
		cout<<"k="<<k<<"\n";
		thread *chld,*chld1;
		chld=create_thread(thread2,(unsigned int)&z,LOW_PRIO,0);
		chld1=create_thread(thread3,(unsigned int)&k,LOW_PRIO,0);
		task_q->add(chld);
		task_q->add(chld1);
		wait_on_thread(chld);
		wait_on_thread(chld1);
		cout<<"z="<<z<<"\n";
		cout<<"k="<<k<<"\n";
		//wait_ms(10);
	}
}
void thread5(unsigned int )
{
	shell *s=new shell();
	s->start();
}
void init_tasks()
{
	cout<<"initializing Tasks\n";
	disable();
	task_q=new thread_que();
	zombie=new thread_que();
	task_q->add(create_thread(idle,0,IDLE_PRIO,1));
	task_q->add(create_thread(thread1,(unsigned int )&b,LOW_PRIO,1));
	//task_q->add(create_thread(thread2,(unsigned int )&c,LOW_PRIO,1));
	//task_q->add(create_thread(thread3,(unsigned int )&d,LOW_PRIO,1));
	task_q->add(create_thread(thread4,(unsigned int )&a,LOW_PRIO,1));
	task_q->add(create_thread(thread5,0,HIGH_PRIO,1)); //our shell
	tasker=1;
	enable();
}

#undef _DEBUG_
