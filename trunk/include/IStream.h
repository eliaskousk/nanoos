//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2009//
// ashok.s.das@gmail.com                                //
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

		private:
			char buffer[BUFFSIZE];

			unsigned int getString();
			void getWord(char *str1, char *p1);
	};

	extern IStream cin;


#endif

