/*****************************************************************************
Simple malloc
Chris Giese <geezer@execpc.com>	http://my.execpc.com/~geezer
Release date: Oct 30, 2002
This code is public domain (no copyright).
You can do whatever you want with it.

Features:
- First-fit
- free() coalesces adjacent free blocks
- Uses variable-sized heap, enlarged with kbrk()/sbrk() function
- Does not use mmap()
- Can be easily modified to use fixed-size heap
- Works with 16- or 32-bit compilers

Build this program with either of the two main() functions, then run it.
Messages that indicate a software error will contain three asterisks (***).
*****************************************************************************/
#include "string.h" /* memcpy(), memset() */
#include "OStream.h" /* printf() */
#include "kheap.h"
#define	MALLOC_MAGIC	0x6D92	/* must be < 0x8000 */


static char *g_heap_bot, *g_kbrk, *g_heap_top;
extern unsigned int kend;
extern unsigned int memend;
/*****************************************************************************
*****************************************************************************/
void dump_heap()
{
	unsigned blks_used = 0, blks_free = 0;
	size_t bytes_used = 0, bytes_free = 0;
	malloc_t *m;
	int total;
	cout.flags(hex|showbase);
	cout<<"===============================================\n";
	for(m = (malloc_t *)g_heap_bot; m != NULL; m = m->next)
	{
		cout<<"blk "<<(unsigned int)m<<": "<<m->size<<" bytes";
		if(m->used)
		{
			blks_used++;
			bytes_used += m->size;
			cout<<"used\n";
		}
		else
		{
			blks_free++;
			bytes_free += m->size;
			cout<<"free\n";
		}
	}
	cout<<"blks:  "<<blks_used<<" used, "<<blks_free<<" free, "<<blks_used + blks_free<<" total\n";
	cout<<"bytes: "<<bytes_used<<" used, "<<bytes_free<<" free, "<<bytes_used + bytes_free<<" total\n";
	cout<<"g_heap_bot="<<(unsigned int)g_heap_bot<<" , g_kbrk="<<(unsigned int)g_kbrk<<" , g_heap_top="<<(unsigned int)g_heap_top<<"\n";
	total = (bytes_used + bytes_free) +
			(blks_used + blks_free) * sizeof(malloc_t);
	if(total != g_kbrk - g_heap_bot)
		cout<<"*** some heap memory is not accounted for\n";
	cout<<"===============================================\n";
	cout.flags(dec);
}
/*****************************************************************************
POSIX sbrk() looks like this
	void *sbrk(int incr);
Mine is a bit different so I can signal the calling function
if more memory than desired was allocated (e.g. in a system with paging)
If your kbrk()/sbrk() always allocates the amount of memory you ask for,
this code can be easily changed.

			int brk(	void *sbrk(		void *kbrk(
function		 void *adr);	 int delta);		 int *delta);
----------------------	------------	------------		-------------
POSIX?			yes		yes			NO
return value if error	-1		-1			NULL
get break value		.		sbrk(0)			int x=0; kbrk(&x);
set break value to X	brk(X)		sbrk(X - sbrk(0))	int x=X, y=0; kbrk(&x) - kbrk(&y);
enlarge heap by N bytes	.		sbrk(+N)		int x=N; kbrk(&x);
shrink heap by N bytes	.		sbrk(-N)		int x=-N; kbrk(&x);
can you tell if you're
  given more memory
  than you wanted?	no		no			yes
*****************************************************************************/
static void *kbrk(int *delta)
{
	//static char heap[HEAP_SIZE];
/**/
	char *new_brk, *old_brk;

/* heap doesn't exist yet */
	if(g_heap_bot == NULL)
	{
		g_heap_bot = g_kbrk =(char *) kend;
		g_heap_top = (char *)memend;//g_heap_bot + HEAP_SIZE;
	}
	new_brk = g_kbrk + (*delta);
/* too low: return NULL */
	if(new_brk < g_heap_bot)
		return NULL;
/* too high: return NULL */
	if(new_brk >= g_heap_top)
		return NULL;
/* success: adjust brk value... */
	old_brk = g_kbrk;
	g_kbrk = new_brk;
/* ...return actual delta... (for this sbrk(), they are the same)
	(*delta) = (*delta); */
/* ...return old brk value */
	return old_brk;
}
/*****************************************************************************
kmalloc() and kfree() use g_heap_bot, but not g_kbrk nor g_heap_top
*****************************************************************************/
void *kmalloc(size_t size)
{
	unsigned total_size;
	malloc_t *m, *n;
	int delta;

	if(size == 0)
		return NULL;
	total_size = size + sizeof(malloc_t);
/* search heap for free block (FIRST FIT) */
	m = (malloc_t *)g_heap_bot;
/* g_heap_bot == 0 == NULL if heap does not yet exist */
	if(m != NULL)
	{
		if(m->magic != MALLOC_MAGIC)
//			panic("kernel heap is corrupt in kmalloc()");
		{
			cout.flags(hex|showbase);			
			cout<<"*** kernel heap is corrupt in kmalloc()\n";
			cout.flags(dec);
			return NULL;
		}
		for(; m->next != NULL; m = m->next)
		{
			if(m->used)
				continue;
/* size == m->size is a perfect fit */
			if(size == m->size)
				m->used = 1;
			else
			{
/* otherwise, we need an extra sizeof(malloc_t) bytes for the header
of a second, free block */
				if(total_size > m->size)
					continue;
/* create a new, smaller free block after this one */
				n = (malloc_t *)((char *)m + total_size);
				n->size = m->size - total_size;
				n->next = m->next;
				n->magic = MALLOC_MAGIC;
				n->used = 0;
/* reduce the size of this block and mark it used */
				m->size = size;
				m->next = n;
				m->used = 1;
			}
			return (char *)m + sizeof(malloc_t);
		}
	}
/* use kbrk() to enlarge (or create!) heap */
	delta = total_size;
	n =(malloc_t *)kbrk(&delta);
/* uh-oh */
	if(n == NULL)
		return NULL;
	if(m != NULL)
		m->next = n;
	n->size = size;
	n->magic = MALLOC_MAGIC;
	n->used = 1;
/* did kbrk() return the exact amount of memory we wanted?
cast to make "gcc -Wall -W ..." shut the hell up */
	if((int)total_size == delta)
		n->next = NULL;
	else
	{
/* it returned more than we wanted (it will never return less):
create a new, free block */
		m = (malloc_t *)((char *)n + total_size);
		m->size = delta - total_size - sizeof(malloc_t);
		m->next = NULL;
		m->magic = MALLOC_MAGIC;
		m->used = 0;

		n->next = m;
	}
	return (char *)n + sizeof(malloc_t);
}
/*****************************************************************************
*****************************************************************************/
void kfree(void *blk)
{
	malloc_t *m, *n;

/* get address of header */
	m = (malloc_t *)((char *)blk - sizeof(malloc_t));
	if(m->magic != MALLOC_MAGIC)
//		panic("attempt to kfree() block at 0x%p "
//			"with bad magic value", blk);
	{
		cout.flags(hex|showbase);
		cout<<"*** attempt to kfree() block at "<<(unsigned int)blk<<" with bad magic value\n";
		cout.flags(dec);
		return;
	}
/* find this block in the heap */
	n = (malloc_t *)g_heap_bot;
	if(n->magic != MALLOC_MAGIC)
//		panic("kernel heap is corrupt in kfree()");
	{
		cout<<"*** kernel heap is corrupt in kfree()\n";
		return;
	}
	for(; n != NULL; n = n->next)
	{
		if(n == m)
			break;
	}
/* not found? bad pointer or no heap or something else? */
	if(n == NULL)
//		panic("attempt to kfree() block at 0x%p "
//			"that is not in the heap", blk);
	{
		cout.flags(hex|showbase);
		cout<<"*** attempt to kfree() block at "<<(unsigned int)blk<<" that is not in the heap\n";
		cout.flags(dec);
		return;
	}
/* free the block */
	m->used = 0;
/* coalesce adjacent free blocks
Hard to spell, hard to do */
	for(m = (malloc_t *)g_heap_bot; m != NULL; m = m->next)
	{
		while(!m->used && m->next != NULL && !m->next->used)
		{
/* resize this block */
			m->size += sizeof(malloc_t) + m->next->size;
/* merge with next block */
			m->next = m->next->next;
		}
	}
}
/*****************************************************************************
*****************************************************************************/
void *krealloc(void *blk, size_t size)
{
	void *new_blk;
	malloc_t *m;

/* size == 0: free block */
	if(size == 0)
	{
		if(blk != NULL)
			kfree(blk);
		new_blk = NULL;
	}
	else
	{
/* allocate new block */
		new_blk = kmalloc(size);
/* if allocation OK, and if old block exists, copy old block to new */
		if(new_blk != NULL && blk != NULL)
		{
			m = (malloc_t *)((char *)blk - sizeof(malloc_t));
			if(m->magic != MALLOC_MAGIC)
//				panic("attempt to krealloc() block at "
//					"0x%p with bad magic value", blk);
			{
				cout.flags(hex|showbase);
				cout<<"*** attempt to krealloc() block at "<<(unsigned int)blk<<" with bad magic value\n";
				cout.flags(dec);
				return NULL;
			}
/* copy minimum of old and new block sizes */
			if(size > m->size)
				size = m->size;
			String::memcpy(new_blk, blk, size);
/* free the old block */
			kfree(blk);
		}
	}
	return new_blk;
}

