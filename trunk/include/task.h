//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2009//
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// TASK Defination                                  //
// Taken from                                           //
// http://hosted.cjmovie.net/TutMultitask.htm           //
//////////////////////////////////////////////////////////
#ifndef __TASK_H__
#define __TASK_H__

#include "idt.h"
#include "low-io.h"

using namespace IDT;
using namespace String;
typedef enum states{
		INVALID=0,
		CREATED=1,
		READY,
		RUNNING,
		WAITING,
		BLOCKED,
		FINISHED,
	}states;
#define MAX_SLICE 20 // 20 ticks
typedef struct thread
{
	unsigned int stack_top; 	
	unsigned char *stack;	
	IDT::regs *r;
	void *arg;
	unsigned int timeslice;
	states state;
	int retval;
	unsigned int id;
	//struct thread *next;
}thread;
 
typedef void (*func)(unsigned int earg);
//thread threads[32]; //we will have maximum 32 tasks running.		

//static int current_task=-1;// no task is started.
//static char tid=1;  //tid=0 is for idle process
// this function will create a task and add to the threads[]
//volatile int tasker=0;
thread *create_thread(func entry,unsigned int args);
extern "C" {unsigned int task_switch(void *sp);}
extern thread *g_current;
void init_tasks();


#endif
