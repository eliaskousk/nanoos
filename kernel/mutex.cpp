//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008-2010 //
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// Mutex Implementation                                 //
// Depends on lock.asm 
//////////////////////////////////////////////////////////
#include "mutex.h"
extern "C" void init_lock(lock_t *l);
extern "C" unsigned int test_and_set(unsigned int val,lock_t *l);

mutex::mutex()
{
	init_lock(&lock); // sets lock = 0
}
mutex::~mutex()  // before destruction resets it to 0, is it needed???
{
	test_and_set(0,&lock);
}
void mutex::try_lock() // spins until set
{
	// test_and_set returns old value of lock until it sets 
	while(test_and_set(1,&lock)==1); //loop here indefinitely
}
void mutex::unlock() // unlock , lock=0
{
	test_and_set(0,&lock);
}
