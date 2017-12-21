/*
 File: scheduler.C
 
 Author:
 Date  :
 
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "scheduler.H"
#include "thread.H"
//#include "queue.H"
#include "console.H"
#include "utils.H"
#include "assert.H"
#include "simple_keyboard.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   S c h e d u l e r  */
/*--------------------------------------------------------------------------*/

Scheduler::Scheduler() {
  //assert(false);

  ready_queue = Queue<Thread*>();
  //Console::puts("Constructed Scheduler.\n");
  blocked_queue = Queue<Thread*>();

}

void Scheduler::yield() {
  //assert(false);

// when thread wants to give up the CPU..it calls yield of scheduler
// then scheduler will get new thread from queue and give it to CPU

   // Console::puts("I am yeilding to another thread.\n");


	Thread * nextthread = ready_queue.get_T();

    // we may need to put calling thread back to ready queue or not??
    // if so then we need to call add thread here !!!
	// ready_queue.add_thread();

	// but in lecture prof said you should not put back calling thread in readyqueue

	Thread::dispatch_to(nextthread);
	// extra stuff -- you can add idle thread if threre is no other threads


  //Console::puts("yielded !\n");
	return;
}

void Scheduler::resume(Thread * _thread) {

 // if a thread is blocked then when its ready then you need to put 
 // this thread to the end of ready queue    
     ready_queue.add_T(_thread);

//first check if blocked queue is empty and then check if its is ready
// if blocked queue is not empty and disk is ready then take one thread 
// out of blocked queue and add this to ready queue as disk has completed 
//reading or writing task.

     if(!blocked_queue.is_empty())
     {if (!(Machine::inportb(0x1F7) & 0x08)){return;}
      Thread * temp_thread = blocked_queue.get_T();
        ready_queue.add_T(temp_thread);
     }

//Console::puts("resumed !\n");
  //assert(false);
}

void Scheduler::add(Thread * _thread) {

	// here when new thread is ready for running then you want to add this to 
	// ready queue ... this may be very similar to the resume function

  //assert(false);

	ready_queue.add_T(_thread);

  //Console::puts("added !\n");
}

void Scheduler::terminate(Thread * _thread) {
  //assert(false);
// when you want to remove a thread from ready queue and then delete it

	// take this thread out of readyqueue

	if(_thread == Thread::CurrentThread()){
		Scheduler::yield();
	}else{

	ready_queue.remove_T(_thread);
     }
	//ready_queue.remove_thread(_thread);

     //Console::puts("terminated !\n");

 }

 void Scheduler::blocked(Thread * _thread){

 	blocked_queue.add_T(_thread);
 }

