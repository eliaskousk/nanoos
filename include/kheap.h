//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2012//
// ashok.s.das@gmail.com          GNU GPL-V2            //
//////////////////////////////////////////////////////////
// kheap header                                         //
//                                                      //
//////////////////////////////////////////////////////////
#ifndef __KHEAP_H__
#define __KHEAP_H__
#include "singleton.h"
#ifndef NULL
#define NULL 0
#endif 
#define size_t unsigned int
extern unsigned int mem_avail;
extern unsigned int times_allocated,times_deallocated;
typedef struct _malloc		/* Turbo C	DJGPP */
{
	size_t size;		/* 2 bytes	 4 bytes */
	struct _malloc *next;	/* 2 bytes	 4 bytes */
	unsigned magic : 15;	/* 2 bytes total 4 bytes total */
	unsigned used : 1;
} malloc_t;		/* total   6 bytes	12 bytes */
/*class Heap : public Singleton<Heap>
{
	private:
		unsigned int heap_start;
		unsigned int heap_end;
		bool inited;
	public:
		Heap(){};
		void init();
		bool is_inited(){ return inited;};
		void *alloc(size_t size);
		void free(void *blk);
		void *realloc(void *blk,size_t size);
		void dump_heap();
};
*/
void dump_heap();
void init_heap();
//static void *kbrk(int *delta);
void *kmalloc(size_t size);
void kfree(void *blk);
void *krealloc(void *blk, size_t size);

//unsigned int kmalloc(unsigned int sz, unsigned int * phys, bool align = false);
//unsigned int kmalloc(unsigned int sz, bool align = false);
//void kfree(unsigned int p);
//inline void dump_stats(){};
#endif

