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
//#include "rtl8139.h"
extern void detect_netdev();
extern void test_req_arp();
extern "C" int kmain(unsigned int magic, multibootInfo *mb);

extern class nic *sys_nic0;

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

	// Before we do any thing we should initialize our Heap based memory allocator( Thanks to Chris Giese )
	init_heap();

	// Setup our Descriptor tables GDT and IDT
	cout<<"Setting up GDT ";
	GDT::setup();
	cout<<"done\n";
	cout<<"setting up IDT ";
	IDT::setup();
	cout<<"done\n";

	// After our IDT is loaded We should install our interrupt
	cout<<"setting up IRQ subsystem ";
	IRQ::setup();
	cout<<"done\n";

	// Now, as our IRQ subsystem is set we should install keyboard sothat, our system will be interractive.
	cout<<"installing key board \n";
	kbd::setup();
	cout<<"done\n";

	// well now show the world we have managed or Memory ;)
	m_boot=multiboot::Instance();
	m_boot->set_multiboot_info(mb);
	m_boot->set_multiboot_hdr();
	cout<<"===============================\n";
	cout<<"Available Memory : "<<(unsigned int)m_boot->get_mem_avail()/1024<<"\n";
	cout<<"     Used Memory : "<<(unsigned int)m_boot->get_mem_used()/1024<<"\n";
	cout<<"===============================\n";	
		
	cout.flags(hex|showbase);
	cout<<"Kernel start "<<(unsigned int)m_boot->get_k_start()<<" Kernel end "<<(unsigned int)m_boot->get_k_end() \
		<<" kernel length ="<<(unsigned int)m_boot->get_k_length()<<"\n";
	
	cout.flags(dec);
	
	// The operating system and the digital computers are worthless if there is no timer
	// so install and initialize the timer
	cout<<"installing timer interrupt ";
	TIMER *my_timer =  TIMER::Instance();	
	my_timer->setup();
	cout<<"done\n";
	cout<<"my_timer at "<<(unsigned int)my_timer<<"\n";	
	cout<<"Mboot at "<<(unsigned int)m_boot<<"\n";

	// So all the basic systems are in place now 
	// We should initialize the PCI subsystem
	cout<<"Scanning PCI...\n";
	pci_bus *sys_pci_bus=pci_bus::Instance();
	sys_pci_bus->scan();
	sys_pci_bus->list_dev();

	// now check if we have any PCI IDE
	//cout<<"Initilizing storage susbsystem(PCI-IDE)\n";
	//init_disks();
	detect_netdev();
	// now we can start our interrupt system
	cout<<"\n\n"<<"Enabling Interrupts\n";	
	enable();
	//cout<<"done\n";
	
	
	//cout<<"\nDone\n";
	//here let us try our Network ethernet device setup
	
	//sys_nic0->send_arp_request();
	cout<<"net init complete\n";
	//cout<<"sending\n";
		//for(int i=0;i<10;i++)
			test_req_arp();	
	// show which IRQs are installed not necessary but it comes handy while debugging the ISRs.
	cout<<"\n"<<"Dumping IRQ routines \n";
	IRQ::dump_irq_routines();
	//asm("int $43");
	// Our tasks are thread implemented in kernel and it depends on timer interrupt
	// so now we can start the tasking subsystem
	//cout<<"Initializing tasking ";
	//init_tasks();
	// bellow this we should not see anything.. why? because in the tasks we started 2 threads 
	// one idle thread and other is our Shell	
	//cout<<"done\n";
		
	for(;;);
	cout<<"\nReached End of kernel\n shoud not happen \n\nGOODBYE\n";
	disable();
	halt();	
	
	return 0;	
}

