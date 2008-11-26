//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2009//
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// basic shell                                          //
//                                                      //
//////////////////////////////////////////////////////////

#include "shell.h"
#include "cmos.h"
void shell::about()
{
	cout.SetColour(BLUE,BLACK,1);
	cout<<"NanOS";
	cout.SetColour(WHITE,BLACK,0);
	cout<<" an hobby and experimental OS kernel Written in C++(g++)\n";
	cout<<"Authour: Ashok Shankar Das (ashok.s.das@gmail.com)\n";
}
void shell::start()
{
	cout<<"Starting shell\n";
	while(1)
	{
		char *cmd=NULL;		
		cout<<"NanOS-#>";
		cin>>cmd;
		if(String::strcmp(cmd,"about")==0)
			about();
		else if(String::strcmp(cmd,"help")==0)
			help();
		else if(String::strcmp(cmd,"clear")==0)
			cout.clear();
		else if(String::strcmp(cmd,"date")==0)
			get_cmos_date_time();
		else
			cout<<"Unknown Command\n";
	}
}
void shell::help()
{
	cout<<"\n\tAvailable Commands ...\n\n";
	cout<<"\tabout -> about the NanOS\n";
	cout<<"\tclear -> clears the screen\n";
	cout<<"\tdate  -> displays current date and time\n";
	cout<<"\thelp  -> displays this help screen\n";
	
}

