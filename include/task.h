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
		CREATED=1,
		READY,
		RUNNING,
		WAITING,
		BLOCKED,
		FINISHED,
	}states;

typedef struct thread
{
	unsigned int id;
	IDT::regs *r;
	unsigned char *stack;
	void *arg;
	states state;
	int retval;
}thread;
 
typedef void (*func)(void *earg);
//thread threads[32]; //we will have maximum 32 tasks running.		

//static int current_task=-1;// no task is started.
//static char tid=1;  //tid=0 is for idle process
// this function will create a task and add to the threads[]
//volatile int tasker=0;
void create_thread(func entry,void *args);
extern "C" {void task_switch(IDT::regs *r);}

void init_tasks();


#endif
