//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008-2010 //
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// Kernel proper                                        //
//                                                      //
//////////////////////////////////////////////////////////

#include "multiboot.h"
#include "runtime.h"
#include "OStream.h"
#include "gdt.h"
#include "idt.h"
#include "irq.h"
#include "timer.h"
#include "kbd.h"
#include "IStream.h"
#include "shell.h"
#include "multiboot.h"
#include "kheap.h"
#include "task.h"
#include "pci.h"
#include "mydrive.h"

extern "C" int kmain(unsigned int magic, multibootInfo *mb);

unsigned int memend; 
unsigned int kend;
char boot_dev[4];

int kmain(unsigned int magic,multibootInfo *mb)
{
	construct(); //construct the global objects	
	char ans;
		
	memend=mb->memoryUpper*1024+0x100000; //memory end upper memory in bytes +1MB
	kend=mboot.kernel_end;	
	multiboot *m_boot;
	
	cout<<"Nano OS is booting\n";
	String::strcpy(boot_dev,(const char *)mb->bootDevice);	
	init_heap();
	
	cout<<"Setting up GDT ";
	GDT::setup();
	cout<<"done\n";
	cout<<"setting up IDT ";
	IDT::setup();
	cout<<"done\n";
	cout<<"setting up IRQ subsystem ";
	IRQ::setup();
	cout<<"done\n";
	cout<<"installing key board \n";
	kbd::setup();
	cout<<"done\n";
	m_boot=multiboot::Instance();
	m_boot->set_multiboot_info(mb);
	m_boot->set_multiboot_hdr();
	cout<<"===============================\n";
	cout<<"Available Memory : "<<(unsigned int)m_boot->get_mem_avail()/1024<<"\n";
	cout<<"     Used Memory : "<<(unsigned int)m_boot->get_mem_used()/1024<<"\n";
	cout<<"===============================\n";	
		
	cout.flags(hex|showbase);
	cout<<"Kernel start "<<(unsigned int)m_boot->get_k_start()<<" Kernel end "<<(unsigned int)m_boot->get_k_end()<<" kernel length ="<<(unsigned int)m_boot->get_k_length()<<"\n";
	
	cout.flags(dec);
	cout<<"installing timer interrupt ";
	TIMER *my_timer =  TIMER::Instance();	
	my_timer->setup();
	cout<<"done\n";
	cout<<"my_timer at "<<(unsigned int)my_timer<<"\n";	
	cout<<"Mboot at "<<(unsigned int)m_boot<<"\n";
	dump_heap();
	cout<<"Scanning PCI...\n";
	pci_bus *sys_pci_bus=pci_bus::Instance();
	sys_pci_bus->scan();
	sys_pci_bus->list_dev();
	init_disks();
	//init_sysdrives();
	cout<<"\n\n"<<"Enabling Interrupts\n";	
	enable();
	cout<<"done\n";
	//dump_heap();
	cout<<"\n"<<"Dumping IRQ routines \n";
	IRQ::dump_irq_routines();
	cout<<"\nDone\n";
	
	cout<<"Initializing tasking ";
	init_tasks();
	cout<<"done\n";
	//shell *s = new shell;
	//s->start();
	
	for(;;);
	cout<<"\nReached End of kernel\n shoud not happen \n\nGOODBYE\n";
	disable();
	halt();	
	
	return 0;	
}

