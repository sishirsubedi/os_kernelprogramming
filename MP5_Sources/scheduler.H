/* 
    Author: R. Bettati, Joshua Capehart
            Department of Computer Science
            Texas A&M University
			
	    A thread scheduler.

*/
// updated mp5
#ifndef SCHEDULER_H
#define SCHEDULER_H

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "thread.H"
//#include "queue.H"
#include "utils.H"
#include "console.H"

/*--------------------------------------------------------------------------*/
/* !!! IMPLEMENTATION HINT !!! */
/*--------------------------------------------------------------------------*/
/*
    One way to proceed is to implement the FIFO scheduling policy inside
    class 'Scheduler'. 

    If you plan to implement a Round-Robin Scheduler, derive it from class
    'Scheduler', say as class 'RRScheduler'. The class 'RRScheduler' is 
    really just a FIFO scheduler with THREE MODIFICATIONS:
    1. It manages a timer, which fires at the end-of-quantum (EOQ). 
    (For details on how to set up a timer and how to handle timer interrupts 
    see the 1-second timer in 'kernel.C'.)  The timer is set up in the
    constructor.
    2. It uses an additional function, the EOQ handler. This function gets
    called whenever an EOQ timer event fires. The EOQ handler forces the 
    current thread to call the scheduler's 'yield' function.
    3. The 'yield' function must be modified to account for unused quantum
    time. If a thread voluntarily yields, the EOQ timer must be reset in order
    to not penalize the next thread.
 
    (Note that this qualifies as programming at about the level of a baboon.
     Much better woudl be to have the abstract class 'Scheduler' implement 
     the basic scheduling MECHANISMS and provide abstract funtions to define
     the queue management POLICIES in derived classes, 
     such as 'FIFOScheduler'.)
    
 */

//create a separate node to keep thread along with next and previous threads
template<class Thread>

class thread_node{
	// no private everthing is public
public:
    // holds current thread
	Thread current_thread;
    // points to previous thread
	thread_node<Thread>* next_thread;
    // points to next thread
	thread_node<Thread>* prev_thread;

   // default constructor
	thread_node()
	{
		current_thread = NULL;
		next_thread = NULL;
		prev_thread = NULL;
	}

	// second constructor to hold new thread
	thread_node( Thread new_thread)
	{
		current_thread = new_thread;
		next_thread = NULL;
		prev_thread = NULL;
	}
	

};

// now each thread_node is kept as a list called as queue
// this list has head and tail to point to first and last thread nodes


template <class Thread>

class Queue{

private:
	// size and head and tail are private
	// current size counts the number of thread nodes in the queue

	int current_size;

	thread_node<Thread> *head;

	thread_node<Thread> *tail;


public:
	
	Queue()
{ // initilize to null pointers 
	current_size= 0;
	head= NULL;
	tail = NULL;
}

int get_queuesize()
{
	return current_size; // returns current number of threads in the queue
}

void add_thread(Thread newthread)
{
	if(current_size==0) // if this is first thread in the queue 
	{	
		head= new thread_node<Thread>(newthread);
		
		head->prev_thread= NULL; head->next_thread= NULL;
		
		tail=head;
		
		current_size++;
	}
	else // if there are already threads in the queue
	{
		thread_node<Thread> *temp_thread_node= new thread_node<Thread>(newthread);

		temp_thread_node->prev_thread= tail;

		tail->next_thread = temp_thread_node;

		tail= temp_thread_node;

		tail->next_thread= NULL;

		current_size++;
	}	
 }


// gets the oldest thread out of queue
Thread get_thread()
{
	if(current_size=0)
	{  
		Console::puts("There are no threads in the queue, error !! \n");
		abort();
	}
	else
	{
		thread_node<Thread> *temp_thread_node= head;

		head=head->next_thread;

		head->prev_thread=NULL;

		current_size--;

		return temp_thread_node->current_thread;
	}
}


// removes selected thread from the queue
void remove_thread(Thread t)
{ 
	thread_node<Thread> *thread_pointer = head;

	while (thread_pointer->next_thread != NULL)
	{
		if( thread_pointer->current_thread == t)
		{
			break;
		}
		else
		{
			thread_pointer= thread_pointer->next_thread;
		}
	}
	if(thread_pointer->current_thread==t)
	{
		thread_pointer->prev_thread->next_thread=thread_pointer->next_thread;

		thread_pointer->next_thread->prev_thread=thread_pointer->prev_thread;

		current_size--;
	}

}
};



/*--------------------------------------------------------------------------*/
/* SCHEDULER */
/*--------------------------------------------------------------------------*/

class Scheduler {

  /* The scheduler may need private members... */

Queue<Thread*> ready_queue;
// scheduler has queue to track threads
//Thread mythread;
  
public:

   Scheduler();
   /* Setup the scheduler. This sets up the ready queue, for example.
      If the scheduler implements some sort of round-robin scheme, then the 
      end_of_quantum handler is installed in the constructor as well. */

   /* NOTE: We are making all functions virtual. This may come in handy when
            you want to derive RRScheduler from this class. */
  
   virtual void yield();
   /* Called by the currently running thread in order to give up the CPU. 
      The scheduler selects the next thread from the ready queue to load onto 
      the CPU, and calls the dispatcher function defined in 'Thread.H' to
      do the context switch. */

   virtual void resume(Thread * _thread);
   /* Add the given thread to the ready queue of the scheduler. This is called
      for threads that were waiting for an event to happen, or that have 
      to give up the CPU in response to a preemption. */

   virtual void add(Thread * _thread);
   /* Make the given thread runnable by the scheduler. This function is called
      after thread creation. Depending on implementation, this function may 
      just add the thread to the ready queue, using 'resume'. */

   virtual void terminate(Thread * _thread);
   /* Remove the given thread from the scheduler in preparation for destruction
      of the thread. 
      Graciously handle the case where the thread wants to terminate itself.*/
  
};
	
	

#endif
