//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2009//
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// shell header                                         //
//                                                      //
//////////////////////////////////////////////////////////
#include "OStream.h"
#include "IStream.h"
#include "string.h"
class shell
{
	public:
		shell();
		
		~shell(){};
		void help();
		void about();
		void start();
	private:
		static char *logo[6];

		int get_num_args(const char *str);
};

