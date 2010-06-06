//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008-2010 //
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// Mutex Class declarations                             //
//                                                      //
//////////////////////////////////////////////////////////
#ifndef __MUTEX_H__
#define __MUTEX_H__

typedef volatile unsigned int lock_t;

class mutex
{
	private: 
		lock_t lock;
	public:
		mutex();        // init_loc is called from constructor
		~mutex();
		void try_lock(); // spin until lock
		void unlock();   // unlock
		
};
#endif
