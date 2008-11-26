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
inline static void enable()
{
	__asm__ __volatile__ ("sti": : );
}

inline static void disable()
{
	__asm__ __volatile__ ("cli" : :	);
}

inline static void halt()
{
	__asm__ __volatile__ ("hlt" : : );
} /*Halts cpu*/

#endif

