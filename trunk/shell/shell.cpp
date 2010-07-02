//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008-2010 //
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// basic shell                                          //
//                                                      //
//////////////////////////////////////////////////////////

#include "shell.h"
#include "cmos.h"
#include "gcpu.h"
#include "multiboot.h"
#include "pci.h"

extern void display_ide();
shell::shell()
{
	cout.clear();	
	cout.SetTextColour(BRIGHTBLUE);
	for(int i=0;i<6;i++)	
	cout<<logo[i]<<"\n";
	cout.SetTextColour(WHITE);
}
void shell::about()
{
	cout.SetColour(BLUE,BLACK,1);
	cout<<"NanOS";
	cout.SetColour(WHITE,BLACK,0);
	cout<<" an hobby and experimental OS kernel Written in C++(g++) and NASM\n";
	cout<<"Authour: Ashok Shankar Das (ashok.s.das@gmail.com)\n";
}
void shell::start()
{
	cout<<"Starting shell\n";
	
	while(1)
	{
		char *cmd=new char[128]  ;		
		cout<<"NanOS-#>";
		cin>>cmd;
		cout<<" \nCommand : "<<cmd<<"\n";
		if(String::strncmp((const char*)cmd,"about",5)==0)
			about();
		else if(String::strncmp((const char*)cmd,"help",4)==0)
			help();
		else if(String::strncmp((const char*)cmd,"clear",5)==0)
			cout.clear();
		else if(String::strncmp((const char*)cmd,"date",4)==0)
			get_cmos_date_time();
		else if(String::strncmp((const char*)cmd,"hello",5)==0)
			cout<<"Hi, how do you do???"<<"\n";
		else if(String::strncmp((const char*)cmd,"cpuinfo",7)==0)
			cpuinfo();
		else if(String::strncmp((const char*)cmd,"meminfo",7)==0)
			meminfo();
		else if(String::strncmp((const char*)cmd,"pci",3)==0)
		{
			pci_bus *sys_pci=pci_bus::Instance();
			sys_pci->list_dev();
		}	
		else if(String::strncmp((const char*)cmd,"bootdev",7)==0)
			{
				extern char *boot_dev;
				cout<<(char *)boot_dev<<"\n";
			}
		
		else if(String::strncmp((const char*)cmd,"tasks",5)==0)
			{
				all_tasks();	
			}
		else if(String::strncmp((const char*)cmd,"ide",3)==0)
			{
				display_ide();	
			}
		else
			cout<<"Unknown Command\n For available commands type help\n";
		
	}
}
const char *shell::logo[6]={
"'|.   '|'                   ..|''||    .|'''.|  ",
" |'|   |   ....   .. ...   .|'    ||   ||..  '  ",
" | '|. |  '' .||   ||  ||  ||      ||   ''|||.  ",
" |   |||  .|' ||   ||  ||  '|.     || .     '|| ",
".|.   '|  '|..'|' .||. ||.  ''|...|'  |'....|'  ",
"================================================\n"};
void shell::meminfo()
{
	extern unsigned int memend;
	multiboot *mbt=multiboot::Instance();
	cout.flags(hex|showbase);
	cout<<"Kernel starts at "<<(unsigned int)mbt->get_k_start()<<"\n";
	cout<<"Kernel Ends at   "<<(unsigned int)mbt->get_k_end()<<"\n";
	cout.SetColour(RED,GREEN,0);
	cout<<"Memory Used =    "<<(unsigned int)mbt->get_mem_used()<<"\n";
	cout<<"Memory Avail =   "<<(unsigned int)mbt->get_mem_avail()<<"\n";
	cout.SetColour(WHITE,BLACK,0);
	cout.flags(dec);
	cout<<"Total Memory     "<<(memend/0x100000)+1<<" MB\n";
}
void shell::help()
{
	cout<<"\n\tAvailable Commands ...\n\n";
	cout<<"\tabout    -> about the NanOS\n";
	cout<<"\tclear    -> clears the screen\n";
	cout<<"\tdate     -> displays current date and time\n";
	cout<<"\thelp     -> displays this help screen\n";
	cout<<"\thello    -> displays hi string\n";
	cout<<"\tcpuinfo  -> displays cpu info\n";
	cout<<"\tmeminfo  -> displays memory info\n";
	
	cout<<"\ttasks    -> displays Tasks currently in memory\n";
}
shell *myshell;
void init_shell(void *)
{
	myshell =new shell;
	myshell->start();
	//return 1;
}
