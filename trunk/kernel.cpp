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
//#include "fdc.h"
//#include "floppy.h"
#include "ide.h"
#include "drive.h"

extern "C" int kmain(multibootInfo *mb);
//extern struct multibootHeader mboot; //this comes from the loader.asm 
extern void init_tasks();
unsigned int memend; 
unsigned int kend;
char boot_dev[4];

int kmain(multibootInfo *mb)
{
	char ans;	
	memend=mb->memoryUpper*1024+0x100000; //memory end upper memory in bytes +1MB
	kend=mboot.kernel_end;	
	multiboot *m_boot=multiboot::instance();
	construct();
	init_heap();	
	cout<<"Nano OS is booting\n";
	String::strcpy(boot_dev,(const char *)mb->bootDevice);		
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
	m_boot=multiboot::instance();
	m_boot->set_multiboot_info(mb);
	m_boot->set_multiboot_hdr();
	cout<<"===============================\n";
	cout<<"Available Memory : "<<(unsigned int)m_boot->get_mem_avail()/1024<<"\n";
	cout<<"     Used Memory : "<<(unsigned int)m_boot->get_mem_used()/1024<<"\n";
	cout<<"===============================\n";	
		
	cout.flags(hex|showbase);
	//kend=m_boot->get_k_end();
	cout<<"Kernel start "<<(unsigned int)m_boot->get_k_start()<<" Kernel end "<<(unsigned int)m_boot->get_k_end()<<" kernel length ="<<(unsigned int)m_boot->get_k_length()<<"\n";
	dump_heap();
	cout.flags(dec);
	//dump_heap();
	//detect_floppy_cmos();
	init_tasks();
	cout<<"\n\n"<<"Enabling Interrupts\n";
	enable();
	init_disks();
	init_sys_drv();
	//cout<<"Finished init_disks\n";
	//unsigned char read_buf[512];
	//if(disks[0])
	//{
	//	disks[0]->disk_info();
	//}	
	//my_req.buf=read_buf;
	//if(ide0->ide_read_sectors(&my_req))
	//	dump(read_buf,128); 
	//IDE *ide1=new IDE(0x4000,0x1f0,0x3f6,8,1,0xb0);
	//IDE *ide2=new IDE(0x8000,0x170,0x3f6,8,1,0xa0);
	//IDE *ide3=new IDE(0x8000,0x170,0x3f6,8,1,0xb0);
	//cout<<"\n\n"<<"Enabling Interrupts\n";
	//enable();
	cout<<"\n"<<"Dumping IRQ routines \n";
	IRQ::dump_irq_routines();
	cout<<"\n";
	
	
	cout<<"Press any key to start shell";
	cin>>ans;
	cout<<"\nStarting Shell\n";
	shell *myshell =new shell;
	myshell->start();
	
	//cout<<(char *)mb->commandLine<<"\n";
	cout<<"\nReached End of kernel\n shoud not happen \n\nGOODBYE\n";
	/*cout<<"testing new delete" <<"\n";
	int *a,*b,*c;
	dump_heap();
	a = (int *)kmalloc(10*sizeof(int));
	dump_heap();
	b = (int *)kmalloc(20*sizeof(int));
	dump_heap();
	c = (int *)kmalloc(1000*sizeof(int));
	dump_heap();
	kfree(b);
	dump_heap();
	kfree(a);
	dump_heap();
	a = (int *)kmalloc(40*sizeof(int));
	dump_heap;*/
	return 0;	
}

