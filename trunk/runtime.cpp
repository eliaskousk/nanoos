//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2009//
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// C++ runtime implementation                           //
//                                                      //
//////////////////////////////////////////////////////////
// Very Basic C++ Runtime support - Based on code from OSdev.org Wiki

#include "kheap.h"
#include "OStream.h"

extern "C" void __cxa_pure_virtual()
{
    //vid->write("\nWarning: A thread tried to execute an illegal action");
	cout<< "\nWarning: A thread tried to execute an illegal action\n";
}

extern "C"
        {
        int __cxa_atexit(void (*f)(void *), void *p, void *d);
        void __cxa_finalize(void *d);
        };

void *__dso_handle; /*only the address of this symbol is taken by gcc*/

struct object
{
        void (*f)(void*);
        void *p;
        void *d;
} object[32] = {0,0,0};

unsigned int iObject = 0;

int __cxa_atexit(void (*f)(void *), void *p, void *d)
{
        if (iObject >= 32) return -1;
        
        object[iObject].f = f;
        object[iObject].p = p;
        object[iObject].d = d;
	++iObject;
 
	return 0;
}

/* This currently destroys all objects */
void __cxa_finalize(void *d)
{
        unsigned int i = iObject;
        for (; i > 0; --i)
        {
                --iObject;
                object[iObject].f(object[iObject].p);
        }
}

void construct()
{
    //Walk and call the constructors in the ctor_list

    //the ctor list is defined in the linker script
    extern void (*__CTOR_LIST__)();

    //hold current constructor in list
    void (**constructor)() = &__CTOR_LIST__ ;

    //the first int is the number of constructors
    int total = *(int *)constructor;

    //increment to first constructor
    constructor++;

    while(total)
    {
        (*constructor)();
        total--;
        constructor++;
    }
}

void destruct()
{
    //Walk and call the deconstructors in the dtor_list

    //the dtor list is defined in the linker script
    extern void (*__DTOR_LIST__)();

    //hold current deconstructor in list
    void (**deconstructor)() = &__DTOR_LIST__;

    //the first int is the number of deconstructors
    int total = *(int *)deconstructor;

    //increment to first deconstructor
    deconstructor++;

    while(total)
    {
        (*deconstructor)();
        total--;
        deconstructor++;
    }
}

//overload the operator "new"
void* operator new (unsigned int size)
{
	/*Heap *mheap= Heap::get_instance();
	if(!mheap->is_inited())
		mheap->init();
	return ((void*)mheap->alloc(size));*/
	return ((void*)kmalloc(size));
}

//overload the operator "new[]"
void* operator new[] (unsigned int size)
{
	/*Heap *mheap= Heap::get_instance();
	if(!mheap->is_inited())
		mheap->init();
	return ((void *)mheap->alloc(size));*/
	return ((void*)kmalloc(size));
}

//overload the operator "delete"
void operator delete (void * p)
{
	/*Heap *mheap= Heap::get_instance();
	if(!mheap->is_inited())
		mheap->init();
	mheap->free(p);*/
	kfree(p);
	//fixme("kfree not implemented yet");
	//cout<< "kfree not implemented yet\n";
	// WARNING: This could cause a segfault if it the video_textmode object (vid)
	// is not deleted last
}

//overload the operator "delete[]"
void operator delete[] (void * p)
{
	//fixme("kfree not implemented yet");
	//cout << "kfree not implemented yet\n";
	/*Heap *mheap= Heap::get_instance();
	if(!mheap->is_inited())
		mheap->init();
	mheap->free(p);*/
	 // FIXME: This needs to be implemented
	kfree(p);
	 
}
