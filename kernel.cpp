//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2009//
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// Kernel proper                                        //
//                                                      //
//////////////////////////////////////////////////////////
//Kernel.cpp

#include "multiboot.h"
#include "runtime.h"
#include "video.h"
#include "OStream.h"
#include "gdt.h"
#include "idt.h"
#include "irq.h"
#include "timer.h"
#include "kbd.h"
#include "IStream.h"
#include "shell.h"
#define OS_NAME "NanOS"

extern "C" int kmain(multibootInfo *mb);
char *logo[6]={"'|.   '|'                   ..|''||    .|'''.|  ",
" |'|   |   ....   .. ...   .|'    ||   ||..  '  ",
" | '|. |  '' .||   ||  ||  ||      ||   ''|||.  ",
" |   |||  .|' ||   ||  ||  '|.     || .     '|| ",
".|.   '|  '|..'|' .||. ||.  ''|...|'  |'....|'  ",
"================================================\n"};

int kmain(multibootInfo *mb)
{
	construct();
	cout.SetTextColour(BRIGHTBLUE);
	//cout.SetBackColour(WHITE);
	for(int i=0;i<6;i++)	
	cout<<logo[i]<<"\n";
	cout.SetTextColour(RED);
	//cout.SetBackColour(WHITE);
	cout<<" is booting\n";
	/*cout<<"    )               )  (   "<<"\n";
	cout<<" ( /(            ( /(  )\\ )"<<"\n";
	cout<<" )\\())   )       )\\())(()/("<<"\n";
	cout<<"((_)\\ ( /(  (   ((_)\\  /(_))"<<"\n";
	cout<<" _((_))(_)) )\\ )  ((_)(_))"<<"\n";
	cout<<"| \\| ((_)_ _(_/( / _ \\/ __|"<<"\n";
	cout<<"| .` / _` | ' \\)) (_) \\__ \\"<<"\n";
	cout<<"|_|\\_\\__,_|_||_| \\___/|___/"<<"\n";*/
	//disable();
	cout.SetTextColour(WHITE);	
	cout<<"Setting up GDT ";
	GDT::setup();
	cout<<"done\n";
	
	cout<<"setting up IDT ";
	IDT::setup();
	cout<<"done\n";
	cout<<"setting up IRQ subsystem ";
	IRQ::setup();
	cout<<"done\n";
	cout<<"installing timer interrupt ";
	my_timer = new TIMER;	
	my_timer->setup();
	cout<<"done\n";
	
	cout<<"installing key board \n";
	kbd::setup();
	cout<<"done\n";
	
	
	//cout<<"\n\n"<<"You can satisfy your fingers by typing few keys\n";
	enable();
	cout<<"TESTING:sleeping for 10ms \n";
	my_timer->sleep(1000);
	cout<<" gotup from sleeping\n";
	int i;
	char name[100];
	cout<<" TESTING cin \n";	
	cout<<" Please enter your name ";	
	cin>>name;
	cout.SetTextColour(RED);
	cout<<"hello "<<name<<" Welcome to NanOS \n";
	cout.SetTextColour(WHITE);
	cout<<"\n"<<"Dumping IRQ routines \n";
	//int a=1/0; divide by 0 working.
	IRQ::dump_irq_routines();
	cout<<"\n";
	//IDT::dump_isr_routines();
	
	shell *myshell =new shell;
	myshell->start();
	cout<<"reached End of kernel\n GOODBYE\n";
	return 0;	
}

