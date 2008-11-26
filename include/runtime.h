//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2009//
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// runtime header                                       //
//                                                      //
//////////////////////////////////////////////////////////
//runtime.h
#ifndef __RUNTIME_H__
#define __RUNTIME_H__

extern "C"
{
        int __cxa_atexit(void (*f)(void *), void *p, void *d);
        void __cxa_finalize(void *d);

        void shutdown();
};

void construct();
void destruct();

#endif

