//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2012//
// ashok.s.das@gmail.com        GNU GPL-V2              //
//////////////////////////////////////////////////////////
// Singleton header                                     //
//                                                      //
//////////////////////////////////////////////////////////
#ifndef __SINGLETON_H__
#define __SINGLETON_H__
#include<OStream.h>
#ifndef NULL
#define NULL 0
#endif
//template for a singleton class
//advan tage of this instead of makeing classes singleton
// inherit the singleton property and enjoy ;)
template <class dType> class singleton
{
	private:
		static dType *object;
	public:
		static dType *instance()
		{
			if(!singleton::object)
				singleton::object= new dType;
			if(!singleton::object)
				cout<<"Horrible Error\n";
			return object;
		}
};
template <class dType> dType* singleton<dType>::object=NULL;
//stolen from OsDynaMix as my singleton class gave me problem and corrupt
//heap
template<typename T> class Singleton
{
private:
	Singleton<T> operator=(const Singleton<T>&) {}
	Singleton(const Singleton<T>&) {}

protected:
	Singleton() {}
	~Singleton() {}

public:
	static T *Instance()
	{
		static T Object;
		return &Object;
	}
};
#endif //singleton		
