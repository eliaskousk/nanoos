//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008-2010 //
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// CMOS read/write routines (low level)                 //
//                                                      //
//////////////////////////////////////////////////////////
#include "low-io.h"
#include "OStream.h"
#define BCD_INT(bcd) (((bcd & 0xf0) >> 4) * 10 + (bcd &0x0f))
//read the register reg and return the value 
unsigned char read_cmos(unsigned char reg)
{
	unsigned char val;
	//enble cmos reg for reading
	outportb(0x70,reg); //send reg to CMOS command port
	val=inportb(0x71);  //read the reg value from data port
	outportb(0x70,0);   //close the command port
	return val;
}

void get_cmos_date_time()
{
	int year,month,date,day,hour,minute,second;
	const char *week_day[7]={"Sun","Mon","Tues","Wednes","Thurs","Fri","Satur"};
	second=BCD_INT(read_cmos(0));
	minute=BCD_INT(read_cmos(2));
	hour=BCD_INT(read_cmos(4));
	day=BCD_INT(read_cmos(6));
	date=BCD_INT(read_cmos(7));
	month=BCD_INT(read_cmos(8));
	year=BCD_INT(read_cmos(9));
	if(year>80)
		year+=1900;
	else
		year+=2000;
	cout<<"Current date "<<date<<" : "<<month<<" : "<<year<<"  "<<week_day[day]<<"day\n";
	cout<<"Current time "<<hour<<" : "<<minute<<" : "<<second<<"\n";
}

