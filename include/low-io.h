//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2009//
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// LowIO routines                                       //
//                                                      //
//////////////////////////////////////////////////////////
/* Low level IO routines defined in low-io.asm
   These are C callable functions
*/
#ifndef _LOW_IO_H_
#define _LOW_IO_H_

inline static unsigned char inportb (unsigned short _port)
	{
		unsigned char rv;
		__asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
		return rv;
	}

inline static void outportb (unsigned short _port, unsigned char _data)
	{
		__asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
	}

inline static unsigned short inportw(unsigned short _port)
	{
		unsigned short rv;
		__asm__ __volatile__ ("inw %1, %0" : "=a" (rv) : "dN" (_port));
		return rv;
	}

inline static void outportw(unsigned short _port, unsigned short _data)
	{
		__asm__ __volatile__ ("outw %1, %0" : : "dN" (_port), "a" (_data));
	}
inline static unsigned int inportl(unsigned short _port)
{
	register unsigned int val;
	__asm__ __volatile__ ("inl %%dx, %%eax" : "=a" (val) : "d" (_port));
        return( val );
}
inline static void outportl(unsigned short _port, unsigned int _data)
{
	__asm__ __volatile__ ("outl %%eax, %%dx" : : "d" (_port), "a" (_data));
}
inline static void insw(short port, unsigned short *data, unsigned count)
{
	for(; count != 0; count--)
	{
		*data = inportw(port);
		data++;
	}
}
inline static void outsw(short port, unsigned short *data, unsigned count)
{
	for(; count != 0; count--)
	{
		outportw(port, *data);
		data++;
	}
}
static volatile unsigned char enabled=0;
inline static void enable()
{
	if(!enabled){
	__asm__ __volatile__ ("sti": : );
	enabled=1;
	}
}

inline static void disable()
{
	if(enabled){
	__asm__ __volatile__ ("cli" : :	);
	enabled=0;
	}
}

inline static void halt()
{
	__asm__ __volatile__ ("hlt" : : );
} /*Halts cpu*/

#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif

