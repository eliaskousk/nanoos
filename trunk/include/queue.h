// Queue template

#ifndef __QUEUE_H__
#define __QUEUE_H__
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
	public:
		que() :head(NULL), tail(NULL), num_nodes(0){};

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
		};
		void remove(node<T> *n)
		{
			node<T> *temp;
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
		};
		
				
		T *get()
		{
			node<T> *temp;
			T *dat;
			temp=head;
			head=head->next;
			dat=temp->data;
			delete(temp);
			num_nodes--;
			return (dat);
		};
			
};
#endif

