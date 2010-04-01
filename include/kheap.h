//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2009//
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// kheap header                                         //
//                                                      //
//////////////////////////////////////////////////////////
#ifndef __KHEAP_H__
#define __KHEAP_H__

#ifndef NULL
#define NULL 0
#endif 
#define size_t unsigned int
typedef struct _malloc		/* Turbo C	DJGPP */
{
	size_t size;		/* 2 bytes	 4 bytes */
	struct _malloc *next;	/* 2 bytes	 4 bytes */
	unsigned magic : 15;	/* 2 bytes total 4 bytes total */
	unsigned used : 1;
} malloc_t;		/* total   6 bytes	12 bytes */
/*class Heap
{
	private:
		static unsigned int heap_start;
		static unsigned int heap_end;
		static bool inited;
		static Heap *heap;
	public:
		Heap(){};
		static Heap *get_instance()
		{
			if(!heap)
				heap=new Heap();
			return heap;
		};
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

