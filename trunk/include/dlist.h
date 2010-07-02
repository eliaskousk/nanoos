////////////////////////////////////
// doubly linked list template    //
// (C) ashok.s.das@gmail.com 2010 //
// GNU GPL V2 or higher license   //
// <http://www.gnu.org/licenses/> //
////////////////////////////////////

#ifndef __DLIST_H__
#define __DLIST_H__
#include "OStream.h"
using namespace std;
template <class T>
struct dnode
{
	T *data;
	dnode *prev,*next;
};
template <class T> class dlist
{
	private:
		dnode<T> *curr;
		dnode<T> *head,*tail;
		unsigned int num_nodes;
	public:
		dlist()
		{
			curr=NULL;
			head=NULL;
			tail=NULL;
			num_nodes=0;
		}
		~dlist(){};
		// Adds a node at head after creating it.
		// takes a pointer to data as input
		void add_at_head(T *x)
		{
			dnode<T> *temp=new dnode<T>;
			temp->data=x;
			if((head!=NULL)&&(tail!=NULL))
			{			
				temp->prev=NULL;
				temp->next=head;
				head->prev=temp;
				head=temp;
			}
			else
			{
				temp->prev=NULL;
				temp->next=NULL;
				head=tail=temp;
			}
			num_nodes++;
			if(curr == NULL)
				curr=head;
		};
		// Adds a node at tail after creating it
		// takes a pointer to data as input
		void add_at_tail(T *x)
		{
			dnode<T> *temp=new dnode<T>;
			temp->data=x;
			if((head!=NULL)&&(tail!=NULL))
			{
				temp->prev=tail;
				temp->next=NULL;
				tail->next=temp;
				tail=temp;
			}
			else
			{
				temp->prev=NULL;
				temp->next=NULL;
				head=tail=temp;
			}
			num_nodes++;
			if(curr == NULL)
				curr=head;
		};
		// returns number of nodes present
		unsigned int get_num_nodes()
		{ return num_nodes;};
		// traverses the list
		// takes whence a Boolean type to say from head or tail
		// takes a pointer to function to dump the data 
		void traverse(bool whence,void (*f)(T*))// whence=0 tail 
		{
			dnode<T> *temp;
			T *data;
			if(whence)
				temp=head;
			else
				temp=tail;
			while(temp)
			{
				data=temp->data;
				if(f)
					f(data);
				else
					cout<<*data;
				if(whence)
					temp=temp->next;
				else
					temp=temp->prev;
			}
		};
		// deletes the nodes having data=x
		// takes the value of data
		void remove(T x)
		{
			dnode<T> *temp;
			for(temp=head; temp; temp=temp->next)
			{
				if(*temp->data == x)
				{
					if(temp->prev)
						temp->prev->next=temp->next;
					if(temp==head)
						head=temp->next;
					if(temp->next)
						temp->next->prev=temp->prev;
					if(temp==tail)
						tail=temp->prev;
					num_nodes--;
					delete temp;
				}
			}
		};
		// removes the nodes containing the data 
		void remove(T *x)
		{
			dnode<T> *temp;
			for(temp=head; temp; temp=temp->next)
			{
				if(*temp->data == *x)
				{
					if(temp->prev)
						temp->prev->next=temp->next;
					if(temp==head)
						head=temp->next;
					if(temp->next)
						temp->next->prev=temp->prev;
					if(temp==tail)
						tail=temp->prev;
					num_nodes--;
					delete temp;
				}
			}
		};
		// returns pointer to data in current node
		T *get_curr()
		{
			if(curr==NULL)
				curr=head;
			if(curr->data !=NULL)
				return curr->data;
			else
				return NULL;			
		};
		//returns pointer to data in next node and sets curr to next node
		T *get_next()
		{
			if(curr->next == NULL)
				curr = head;
			else
				curr=curr->next;
			if(curr->data !=NULL)
				return curr->data;
			else
				return NULL;
		};
		// returns pointer to data in prev node and changes the curr node
		T *get_prev()
		{
			if(curr->prev == NULL)
				curr = tail;
			else
				curr=curr->prev;
			if(curr->data !=NULL)
				return curr->data;
			else
				return NULL;		
		};
};
#endif

