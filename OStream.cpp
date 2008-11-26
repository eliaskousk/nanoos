//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2009//
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// Ostream implementation                               //
//                                                      //
//////////////////////////////////////////////////////////
//stdlib.cpp
#include "OStream.h"


OStream cout;

OStream::OStream()
{
	current_flags = dec;
}

OStream& OStream::operator <<(char *str)
{
	write(str);
	return *this;
}

OStream& OStream::operator <<(char c)
{
	putchar(c);
	return *this;
}

OStream& OStream::operator <<(unsigned char *str)
{
	write((char *)str);
	return *this;
}

OStream& OStream::operator <<(unsigned char c)
{
	putchar((char)c);
	return *this;
}

OStream& OStream::operator <<(int i)
{
	char buf[33];
	
	if (current_flags & dec)
		String::itoa(buf, 10, i);
	else if (current_flags & hex)
		String::itoa(buf, 16, i);
	else if (current_flags & oct)
		String::itoa(buf, 8, i);
	else if (current_flags & bin)
		String::itoa(buf, 2, i);

	if (current_flags & hex && current_flags & showbase)
		write("0x");

	write(buf);
	return *this;
}

OStream& OStream::operator <<(unsigned int i)
{
	*this << (int)i;
	return *this;
}

OStream& OStream::operator <<(long long i)
{
	char buf[65];
	
	if (current_flags & dec)
		String::itoa(buf, 10, i);
	else if (current_flags & hex)
		String::itoa(buf, 16, i);
	else if (current_flags & oct)
		String::itoa(buf, 8, i);
	else if (current_flags & bin)
		String::itoa(buf, 2, i);

	if (current_flags & hex && current_flags & showbase)
		write("0x");

	write(buf);
	return *this;
}

OStream& OStream::operator <<(unsigned long long i)
{
	*this << (long long)i;
	return *this;
}

void OStream::flags(unsigned int f)
{
	current_flags = f;
}

