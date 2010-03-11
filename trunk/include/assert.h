//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2009//
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// assert: defines assert and associated macros         //
//                                                      //
//////////////////////////////////////////////////////////
#ifndef __ASSERT_H__
#define __ASSERT_H__
#include "OStream.h"

#define __symbol2value( x ) #x
#define __symbol2string( x ) __symbol2value( x )

#undef assert
#define assert( expression ) ( ( expression ) ? (void)0: \
        cout<<"Assertion Failed "<< #expression <<'\n');
#endif

