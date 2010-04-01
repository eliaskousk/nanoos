//myheap.cpp
#include "kheap.h"
#include "OStream.h"
#include "low-io.h"

#define	MALLOC_MAGIC	0x6D92	/* must be < 0x8000 */
extern unsigned int kend;
extern unsigned int memend;
static unsigned char *heap_bot,*heap_top;
/*unsigned int Heap::heap_start=kend;
unsigned int Heap::heap_end=memend;
bool Heap::inited=0;
Heap* Heap::heap=NULL;
void Heap::init()
{
	if(!inited)
	{
		cout<<"Initializing System Heap\n";
		heap_start=kend;
		heap_end=memend;
		malloc_t *m=(malloc_t*)heap_start;
		if(m==NULL)
		{
			cout.SetColour(RED,BLACK,0);
			cout<<"Could not initialize Heap\n halting the kernel\n";
			cout.SetColour(WHITE,BLACK,0);		
			disable();
			halt();
		}
		else
		{
			//claim all the available memory after kernel end. 
			m->magic= MALLOC_MAGIC;
			m->next=NULL; //nothing after this block
			m->size=heap_top-heap_bot-sizeof(malloc_t); //total memory available
			m->used=0;    //not used free/available
		}
		inited=1;		
		cout<<"Heap initialized\n";
	}
	else
		cout<<"Heap already initialized\n";
}
void *Heap::alloc(size_t size)
{
	size_t required;
	malloc_t* n;
	required=size+sizeof(malloc_t);	
	malloc_t* m=(malloc_t*)heap_start;
	if(m)
	{
		//Ok
		if(m->magic!=MALLOC_MAGIC)
		{
			//corrupt or error or un-initialized
			cout.SetColour(RED,BLACK,0);
			cout<<"Corrupted Heap\n";
			cout.SetColour(WHITE,BLACK,0);
			return NULL;
		}
		while(m)
		{
			if(m->used==1)
			{
				m=m->next;
				continue;
			}
			if(m->size<required)
			{
				m=m->next;
				continue;
			}
			//we can allocate
			
			n=m+sizeof(malloc_t)+size;
			n->magic=MALLOC_MAGIC;
			n->used=0;
			n->next=m->next;
			n->size=m->size-size-sizeof(malloc_t);
			m->used=1;
			m->next=n;
			m->size=size;
			return ((char*)m+sizeof(malloc_t));
		}
	}
	//m is NULL
	cout.SetColour(RED,BLACK,0);
	cout<<"No Memory for allocation\n";
	cout.SetColour(WHITE,BLACK,0);
	return NULL;
}
void Heap::free(void *blk)
{
	malloc_t *m;
	m = (malloc_t *)((char *)blk - sizeof(malloc_t));
	if(((unsigned int)blk< heap_start)||((unsigned int)blk>heap_end))
	{
		cout.SetColour(RED,BLACK,0);		
		cout<<"Illegal memory area\n";
		cout.SetColour(WHITE,BLACK,0);
		goto fini_free;
	}
	//so p is within our limit
	if(m->magic!=MALLOC_MAGIC)
	{
		cout.SetColour(RED,BLACK,1);		
		cout<<"*** Bad Magic / corrupted heap\n";
		cout<<"*** Go consult a scientist :{\n";
		cout.SetColour(WHITE,BLACK,0);
		goto fini_free;
	}
	
	if(m->used==0)
	{
		//unused memory
		cout<<"***Trying to free unused Unused memory\n";
		goto fini_free;
	}
	//if we reached here means we are to mark the block as free
	m->used=0;
	fini_free:		
	//try unfragment the heap
	m=(malloc_t*)heap_start;
	for(;m!=NULL;m=m->next)
	{	
		while(!m->used && m->next != NULL && !m->next->used)
		{
// resize this block 
			m->size += sizeof(malloc_t) + m->next->size;
// merge with next block 
			m->next = m->next->next;
		}
	}
}
void *Heap::realloc(void *blk,size_t size)
{
	void *new_blk;
	malloc_t *m;

// size == 0: free block 
	if(size == 0)
	{
		if(blk != NULL)
			free(blk);
		new_blk = NULL;
	}
	else
	{
// allocate new block 
		new_blk = alloc(size);
// if allocation OK, and if old block exists, copy old block to new 
		if(new_blk != NULL && blk != NULL)
		{
			m = (malloc_t *)((char *)blk - sizeof(malloc_t));
			if(m->magic != MALLOC_MAGIC)
//				panic("attempt to krealloc() block at "
//					"0x%p with bad magic value", blk);
			{
				cout.flags(hex|showbase);
				cout<<"*** attempt to realloc() block at "<<(unsigned int)blk<<" with bad magic value\n";
				cout.flags(dec);
				return NULL;
			}
// copy minimum of old and new block sizes 
			if(size > m->size)
				size = m->size;
			String::memcpy(new_blk, blk, size);
// free the old block 
			free(blk);
		}
	}
	return new_blk;
}
void Heap::dump_heap()
{
	unsigned blks_used = 0, blks_free = 0;
	size_t bytes_used = 0, bytes_free = 0;
	malloc_t *m;
	unsigned int total;
	cout.flags(hex|showbase);
	cout<<"===============================================\n";
	for(m = (malloc_t *)heap_start; m != NULL; m = m->next)
	{
		cout<<"blk "<<(unsigned int)m<<": "<<m->size<<" bytes";
		if(m->used)
		{
			blks_used++;
			bytes_used += m->size;
			cout<<" used\n";
		}
		else
		{
			blks_free++;
			bytes_free += m->size;
			cout<<" free\n";
		}
	}
	cout<<"blks:  "<<blks_used<<" used, "<<blks_free<<" free, "<<blks_used + blks_free<<" total\n";
	cout<<"bytes: "<<bytes_used<<" used, "<<bytes_free<<" free, "<<bytes_used + bytes_free<<" total\n";
	//cout<<"heap_bot="<<(unsigned int)heap_bot<<" , c_brk="<<(unsigned int)c_brk<<" , heap_top="<<(unsigned int)heap_top<<"\n";
	total = (bytes_used + bytes_free) +
			(blks_used + blks_free) * sizeof(malloc_t);
	if(total != heap_end - heap_start)
		cout<<"*** some heap memory is not accounted for\n";
	cout<<"===============================================\n";
	cout.flags(dec);
}*/
void init_heap()
{
	heap_bot=(unsigned char*)kend;
	heap_top=(unsigned char*)memend;
	malloc_t* m=(malloc_t*)heap_bot;
	if(m==NULL)
	{
		cout.SetColour(RED,BLACK,0);
		cout<<"Could not initialize Heap\n halting the kernel\n";
		cout.SetColour(WHITE,BLACK,0);		
		__asm__("cli");
		__asm__("hlt");
	}
	else
	{
		//claim all the available memory after kernel end. 
		m->magic= MALLOC_MAGIC;
		m->next=NULL; //nothing after this block
		m->size=heap_top-heap_bot-sizeof(malloc_t); //total memory available
		m->used=0;    //not used free/available
	}
	cout<<"Heap initialized\n";
}
void *kmalloc(size_t size)
{
	size_t required;
	malloc_t* n;
	required=size+sizeof(malloc_t);	
	malloc_t* m=(malloc_t*)heap_bot;
	if(m)
	{
		//Ok
		if(m->magic!=MALLOC_MAGIC)
		{
			//corrupt or error or un-initialized
			cout.SetColour(RED,BLACK,0);
			cout<<"Corrupted Heap\n";
			cout.SetColour(WHITE,BLACK,0);
			return NULL;
		}
		while(m)
		{
			if(m->used==1)
			{
				m=m->next;
				continue;
			}
			if(m->size<required)
			{
				m=m->next;
				continue;
			}
			//we can allocate
			
			n=m+sizeof(malloc_t)+size;
			n->magic=MALLOC_MAGIC;
			n->used=0;
			n->next=m->next;
			n->size=m->size-size-sizeof(malloc_t);
			m->used=1;
			m->next=n;
			m->size=size;
			return ((char*)m+sizeof(malloc_t));
		}
	}
	//m is NULL
	cout.SetColour(RED,BLACK,0);
	cout<<"No Memory for allocation\n";
	cout.SetColour(WHITE,BLACK,0);
	return NULL;	
}
void kfree(void *p)
{
	malloc_t *m;
	m = (malloc_t *)((char *)p - sizeof(malloc_t));
	if(((unsigned char*)p< heap_bot)||((unsigned char *)p>heap_top))
	{
		cout<<"Illegal memory area\n";
		return;
	}
	//so p is within our limit
	if(m->magic!=MALLOC_MAGIC)
	{
		cout<<"*** Bad Magic / corrupted heap\n";
		cout<<"*** Go consult a scientist :{\n";
		return;
	}
	
	if(m->used==0)
	{
		//unused memory
		cout<<"***Trying to free unused Unused memory\n";
		return;
	}
	//if we reached here means we are to mark the block as free
	m->used=0;		
	//try unfragment the heap
	m=(malloc_t*)heap_bot;
	for(;m!=NULL;m=m->next)
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
void dump_heap()
{
	unsigned blks_used = 0, blks_free = 0;
	size_t bytes_used = 0, bytes_free = 0;
	malloc_t *m;
	int total;
	cout.flags(hex|showbase);
	cout<<"===============================================\n";
	for(m = (malloc_t *)heap_bot; m != NULL; m = m->next)
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
	//cout<<"heap_bot="<<(unsigned int)heap_bot<<" , c_brk="<<(unsigned int)c_brk<<" , heap_top="<<(unsigned int)heap_top<<"\n";
	total = (bytes_used + bytes_free) +
			(blks_used + blks_free) * sizeof(malloc_t);
	if(total != heap_top - heap_bot)
		cout<<"*** some heap memory is not accounted for\n";
	cout<<"===============================================\n";
	cout.flags(dec);
}

