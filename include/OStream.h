//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2009//
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// Ostream header                                       //
//                                                      //
//////////////////////////////////////////////////////////
#ifndef __OSTREAM_H__
#define __OSTREAM_H__

#include "video.h"
#include "string.h"//---------------OStream:------------------------
#define boolalpha	0x0001 /* NYI */
#define showbase	0x0002
#define showpoint	0x0004 /* NYI */
#define showpos		0x0008 /* NYI */
#define skipws		0x0010 /* NYI */
#define unitbuf		0x0020 /* NYI */
#define uppercase	0x0040 /* NYI */
#define dec		0x0080
#define hex		0x0100
#define oct		0x0200
#define bin		0x0400
#define internal	0x0800 /* NYI */
#define left		0x1000 /* NYI */
#define right		0x2000 /* NYI */

class OStream : public video
{
	unsigned int current_flags;
public:
	OStream();
	OStream& operator <<(char *str);
	OStream& operator <<(const char *str);
	OStream& operator <<(char c);
	OStream& operator <<(unsigned char *str);
	OStream& operator <<(unsigned char c);
	OStream& operator <<(int i);
	OStream& operator <<(unsigned int i);
	OStream& operator <<(long long i);
	OStream& operator <<(unsigned long long i);
	void flags(unsigned int f);
};

extern OStream cout;


#endif

