// Queue template

#ifndef __QUEUE_H__
#define __QUEUE_H__
#include "mutex.h"
using namespace std;

template <class T>
struct node
{
	T *data;
	node *next;
};

template <class T> class que
{
	private:
		node<T> *head,*tail;
		unsigned int num_nodes;
		mutex *q_m;
	public:
		que() :head(NULL), tail(NULL), num_nodes(0){q_m=new mutex(); };

		bool is_empty()
		{ 
			if(num_nodes) 
				return 1;
			else
				return 0;
		};
		unsigned int get_num_nodes()
		{
			return (num_nodes);
		};
		void put(T *dat)
		{
			//q_m->try_lock();
			node<T> *temp=new node<T>;
			temp->data=dat;
			temp->next=NULL;
			if(tail==NULL)
			{
				tail=temp;
				head=tail;
			}
			else
			{
				tail->next=temp;
				tail=temp;
			}
			num_nodes++;
			//while(q_m->unlock());
		};
		void remove(node<T> *n)
		{
			node<T> *temp;
			//q_m->try_lock();
			if (n==head)
			{
				temp=head;
				head=head->next;
				delete(temp);
				num_nodes--;
			}
			else
			{
				temp=head;
				while(temp->next != n)
				{
					temp=temp->next;
				}
				node<T> temp1;
				temp1=temp->next;
				temp->next=temp1->next;
				delete (temp1);
				num_nodes--;
			}
			//while(q_m->unlock());
		};
		
		void remove( T *x)
		{
			node<T> *temp1,*temp2;
			//q_m->try_lock();
			temp1=head;
			if(temp1->data==x)
			{
				head=head->next;
				delete temp1;
			}
			else
			{
				while(temp1)
				{
					temp2=temp1;
					temp1=temp1->next;
					if(temp1->data==x)
					{
						temp2->next=temp1->next;
						delete temp1;
					}
				}
			}
			//while(q_m->unlock());
		};
				
		T *get()
		{
			node<T> *temp;
			T *dat;
			//q_m->try_lock();
			temp=head;
			head=head->next;
			dat=temp->data;
			delete(temp);
			num_nodes--;
			//while(q_m->unlock());
			return (dat);
		};
			
};
#endif

