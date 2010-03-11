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
			if(!object)
				object= new dType;
			if(!object)
				cout<<"Horrible Error\n";
			return object;
		}
};
template <class dType> dType* singleton<dType>::object=NULL;

#endif //singleton		
