//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2009//
// ashok.s.das@gmail.com       GNU GPL-V2               //
//////////////////////////////////////////////////////////
// Utils                                                //
//                                                      //
//////////////////////////////////////////////////////////
#include "OStream.h"
#include "IStream.h"

int MIN ( int a, int b)
{
	if(a<b)
		return a;
	return b;
}
void hex_dump (const unsigned char *data, int len)
{
        int pos = 0;
	int i,lin=0;
	char ans;
  
	while (pos < len)
	{
		cout.flags(hex);
		cout<< pos<<"  ";

		int chunk = MIN (len - pos, 16);

		for ( i = 0; i < chunk; ++i)
		{
			if (i % 2 == 1)
				cout<<(unsigned short)data[pos + i]<<" ";
			else
				cout<<(unsigned short)data[pos + i];
		}
		for ( i = 0;(unsigned int) i < chunk; ++i)
		{
			cout.flags(dec);
			unsigned char b = data[pos + i];
			if(b<' ')
				cout<<'.';
			else
				cout<<b;
		}
		cout<<'\n';
		pos += chunk;
		lin++;
		if(lin%24==0)
		{ 
			cin>>ans;
			lin=0;
		}
	}
	cout.flags(dec);
}
