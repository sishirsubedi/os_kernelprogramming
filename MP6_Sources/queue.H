
#ifndef _QUEUE_H
#define _QUEUE_H


#include "utils.H"
#include "console.H"
//create a separate node to keep T along with next and previous Ts
template<typename T>

class T_node{
	// no private everthing is public
public:
    // holds current T
	T current_T;
    // points to previous T
	T_node<T>* next_T;
    // points to next T
	T_node<T>* prev_T;

   // default constructor
	T_node()
	{
		current_T = NULL;
		next_T = NULL;
		prev_T = NULL;
	}

	// second constructor to hold new T
	T_node( T new_T)
	{
		current_T = new_T;
		next_T = NULL;
		prev_T = NULL;
	}
	

};

// now each T_node is kept as a list called as queue
// this list has head and tail to point to first and last T nodes


template <typename T>

class Queue{

private:
	// size and head and tail are private
	// current size counts the number of T nodes in the queue

	int current_size;

	T_node<T> *head;

	T_node<T> *tail;


public:
	
	Queue()
{ // initilize to null pointers 
	current_size= 0;
	head= NULL;
	tail = NULL;
}


bool is_empty(){
	if (head==NULL){
		return true;
	}
	else{
		return false;
	}
}

int get_queuesize()
{
	return current_size; // returns current number of Ts in the queue
}

void add_T(T newT)
{
	if(current_size==0) // if this is first T in the queue 
	{	
		head= new T_node<T>(newT);
		
		head->prev_T= NULL; head->next_T= NULL;
		
		tail=head;
		
		current_size++;
	}
	else // if there are already Ts in the queue
	{
		T_node<T> *temp_T_node= new T_node<T>(newT);

		temp_T_node->prev_T= tail;

		tail->next_T = temp_T_node;

		tail= temp_T_node;

		tail->next_T= NULL;

		current_size++;
	}	
 }


// gets the oldest T out of queue
T get_T()
{
	if(current_size=0)
	{  
		Console::puts("There are no Ts in the queue, error !! \n");
		abort();
	}
	else
	{
		T_node<T> *temp_T_node= head;

		head=head->next_T;

		head->prev_T=NULL;

		current_size--;

		return temp_T_node->current_T;
	}
}


// removes selected T from the queue
void remove_T(T t)
{ 
	T_node<T> *T_pointer = head;

	while (T_pointer->next_T != NULL)
	{
		if( T_pointer->current_T == t)
		{
			break;
		}
		else
		{
			T_pointer= T_pointer->next_T;
		}
	}
	if(T_pointer->current_T==t)
	{
		T_pointer->prev_T->next_T=T_pointer->next_T;

		T_pointer->next_T->prev_T=T_pointer->prev_T;

		current_size--;
	}

}
};	

#endif
