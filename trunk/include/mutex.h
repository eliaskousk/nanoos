//mutex
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
