//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2012//
// ashok.s.das@gmail.com        GNU GPL-V2              //
//////////////////////////////////////////////////////////
// shell header                                         //
//                                                      //
//////////////////////////////////////////////////////////
#ifndef __SHELL_H__
#define __SHELL_H__
#include "OStream.h"
#include "IStream.h"
#include "string.h"
#include "gcpu.h"
#include "task.h"

class shell
{
	public:
		shell();
		
		~shell(){};
		void help();
		void about();
		void start();
		void meminfo();
	private:
		static const char *logo[6];

		int get_num_args(const char *str);
};
void init_shell(void *);
#endif
