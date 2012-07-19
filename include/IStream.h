//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2012//
// ashok.s.das@gmail.com         GNU GPL-V2             //
//////////////////////////////////////////////////////////
// IStream header                                       //
//                                                      //
//////////////////////////////////////////////////////////
#ifndef __ISTREAM_H__
#define __ISTREAM_H__

#include "OStream.h"
#include "kbd.h"


//---------------IStream:------------------------

	const int BUFFSIZE = 256;

	class IStream 
	{
		public:
			IStream& operator >> (char *cp);
			IStream& operator >> (char &c);
			IStream& operator >> (unsigned char *ucp);
			IStream& operator >> (unsigned char &uc);
			IStream& operator >> (unsigned int &i);
			char *getstring();
			
		private:
			char buffer[BUFFSIZE];

			unsigned int getString();
			void getWord(char *str1, char *p1);
	};

	extern IStream cin;


#endif

