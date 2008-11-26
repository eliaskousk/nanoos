//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2009//
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// CMOS read/write routines (low level) declarations    //
//                                                      //
//////////////////////////////////////////////////////////

#ifndef __CMOS_H__
#define __CMOS_H__

unsigned char read_cmos(unsigned char reg);
void get_cmos_date_time();

#endif
