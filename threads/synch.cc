// synch.cc 
//	Routines for synchronizing threads.  Three kinds of
//	synchronization routines are defined here: semaphores, locks 
//   	and condition variables (the implementation of the last two
//	are left to the reader).
//
// Any implementation of a synchronization routine needs some
// primitive atomic operation.  We assume Nachos is running on
// a uniprocessor, and thus atomicity can be provided by
// turning off interrupts.  While interrupts are disabled, no
// context switch can occur, and thus the current thread is guaranteed
// to hold the CPU throughout, until interrupts are reenabled.
//
// Because some of these routines might be called with interrupts
// already disabled (Semaphore::V for one), instead of turning
// on interrupts at the end of the atomic operation, we always simply
// re-set the interrupt state back to its original value (whether
// that be disabled or enabled).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synch.h"
#include "system.h"
#include "puerto.h" /////



//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	Initialize a semaphore, so that it can be used for synchronization.
//
//	"debugName" is an arbitrary name, useful for debugging.
//	"initialValue" is the initial value of the semaphore.
//----------------------------------------------------------------------

Semaphore::Semaphore(const char* debugName, int initialValue)
{
    name = debugName;
    value = initialValue;
    queue = new List<Thread*>;
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	De-allocate semaphore, when no longer needed.  Assume no one
//	is still waiting on the semaphore!
//----------------------------------------------------------------------

Semaphore::~Semaphore()
{
    delete queue;
}

//----------------------------------------------------------------------
// Semaphore::P
// 	Wait until semaphore value > 0, then decrement.  Checking the
//	value and decrementing must be done atomically, so we
//	need to disable interrupts before checking the value.
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
//----------------------------------------------------------------------

void
Semaphore::P()
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts
    
    while (value == 0) { 			// semaphore not available
	queue->Append(currentThread);		// so go to sleep
	currentThread->Sleep();
    } 
    value--; 					// semaphore available, 
						// consume its value
    interrupt->SetLevel(oldLevel);		// re-enable interrupts
}

//----------------------------------------------------------------------
// Semaphore::V
// 	Increment semaphore value, waking up a waiter if necessary.
//	As with P(), this operation must be atomic, so we need to disable
//	interrupts.  Scheduler::ReadyToRun() assumes that threads
//	are disabled when it is called.
//----------------------------------------------------------------------

//Parece que esto es codigo viejo
void
Semaphore::V()
{
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    thread = queue->Remove();
    if (thread != NULL)	   // make thread ready, consuming the V immediately
	scheduler->ReadyToRun(thread);
    value++;
    interrupt->SetLevel(oldLevel);
}

// Dummy functions -- so we can compile our later assignments 
// Note -- without a correct implementation of Condition::Wait(), 
// the test case in the network assignment won't work!
Lock::Lock(const char* debugName) {
	name = debugName;
  	owner =NULL;
	sem= new Semaphore("semLock",1);
}

Lock::~Lock() {
	sem->~Semaphore();
	owner=NULL;
}

void Lock::Acquire() {
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

	ASSERT(!isHeldByCurrentThread());
    
    	DEBUG('t', "%s intenta adquirir el lock %s\n", currentThread->getName(),this->name); 

	sem->P();

	owner= currentThread;
	
	DEBUG('t', "%s posee el lock %s\n", currentThread->getName(),this->name); 
    interrupt->SetLevel(oldLevel);
}

void Lock::Release() {
    IntStatus oldLevel = interrupt->SetLevel(IntOff);
	ASSERT(isHeldByCurrentThread());
    
   	DEBUG('t', "%s va a dejar el lock %s\n", currentThread->getName(),this->name); 
    
    sem->V();	
	owner=NULL;
	DEBUG('t', "%s ya no posee el lock %s\n", currentThread->getName(),this->name); 
	    interrupt->SetLevel(oldLevel);
	
}

bool Lock::isHeldByCurrentThread() {
	return (owner==currentThread);	
}



Condition::Condition(const char* debugName, Lock* conditionLock) {
	name = debugName;
	cerrojo= conditionLock;
	queue = new List<Thread*>;
 }
Condition::~Condition() {
	delete queue;
	cerrojo->~Lock();
 }

void Condition::Wait() { 					//el mismo thread despertado
	IntStatus oldLevel = interrupt->SetLevel(IntOff);	//debe encargarse de recuperar el lock
	
	ASSERT(cerrojo->isHeldByCurrentThread());
	DEBUG('t',"NO CUMPLE CONDICION %s \n", this->name);
	
	queue->Append(currentThread);
	cerrojo->Release();
	currentThread->Sleep();


	interrupt->SetLevel(oldLevel);
}

void Condition::Signal() {
	Thread *thread;
	IntStatus oldLevel = interrupt->SetLevel(IntOff);
	ASSERT(cerrojo->isHeldByCurrentThread());
	
	thread = queue->Remove();
    	if (thread != NULL){
			scheduler->ReadyToRun(thread);
   	}			    
	interrupt->SetLevel(oldLevel);
 }

void Condition::Broadcast() {
	Thread *thread;
	IntStatus oldLevel = interrupt->SetLevel(IntOff);

	DEBUG('t',"Todos los hilos serán Puestos en Ready\n");

	ASSERT(cerrojo->isHeldByCurrentThread());
	thread = queue->Remove();
	while(thread!=NULL){
		scheduler->ReadyToRun(thread);
		thread = queue->Remove();
	}
	interrupt->SetLevel(oldLevel);
 }






