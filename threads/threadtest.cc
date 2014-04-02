// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create several threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustrate the inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.
//
// Parts from Copyright (c) 2007-2009 Universidad de Las Palmas de Gran Canaria
//

#include "copyright.h"
#include "system.h"
#include "synch.h"
#include "thread.h"
#include "puerto.h"

//----------------------------------------------------------------------
// SimpleThread
// 	Loop 10 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"name" points to a string with a thread name, just for
//      debugging purposes.
//----------------------------------------------------------------------

void
SimpleThread(void* name)
{
    // Reinterpret arg "name" as a string
    char* threadName = (char*)name;
    
    // If the lines dealing with interrupts are commented,
    // the code will behave incorrectly, because
    // printf execution may cause race conditions.
    for (int num = 0; num < 10; num++) {
        IntStatus oldLevel = interrupt->SetLevel(IntOff);
	printf("*** thread %s looped %d times\n", threadName, num);
	interrupt->SetLevel(oldLevel);
        currentThread->Yield();
    }
    IntStatus oldLevel = interrupt->SetLevel(IntOff);
    printf(">>> Thread %s has finished\n", threadName);
    interrupt->SetLevel(oldLevel);
}

//----------------------------------------------------------------------
// ThreadTest
// 	Set up a ping-pong between several threads, by launching
//	ten threads which call SimpleThread, and finally calling 
//	SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest()
{
    DEBUG('t', "Entering SimpleTest");

    for ( int k=1; k<=5; k++) {
      char* threadname = new char[100];
      sprintf(threadname, "Hilo %d", k);
      	Thread* newThread = new Thread (threadname);
      	newThread->Fork (SimpleThread, (void*)threadname);
		if(k==3) newThread->priority=3;
    }
    
    SimpleThread( (void*)"Hilo 0");
}


/*
////Probando sync.h y sync.cc /////	
	
int total=0;
char* threadname2 = new char[100];
	Lock* c= new Lock("cerrojo");
	Semaphore *s= new Semaphore("S", 3);
	Condition* cond=new Condition("condicion", c);

void sumo(void *name){

	c->Acquire();

	total= total+1;

	while(total<3) cond->Wait();
	if(!(c->isHeldByCurrentThread())) 	// recupero el lock si el thread se fue a dormir
		 c->Acquire();
	
	currentThread->Yield(); 	//trata de cambiar a otro thread suma pero no puede por el lock
		cond->Broadcast();	  //pongo en ready a todos los theads esperando
		printf("S total %d\n", total);
	c->Release();
	
}

void multiplico(void* name){
	c->Acquire();
	total=total +total;
	printf("M total %d\n", total);
	currentThread->Yield(); 	//
	c->Release();
}



void SynTest()	
{
	//currentThread->calljoin=true;

	for ( int k=1; k<=5; k++) {
      char* threadname = new char[100];
      sprintf(threadname, "Hilo %d S", k);
	 char* threadnamew = new char[100];
      sprintf(threadnamew, "Hilo %d M", k);
      	Thread* newThread = new Thread (threadname);
	      	Thread* newThreadw = new Thread (threadnamew);

      	newThread->Fork (sumo, (void*)threadname);
	
//	if(currentThread->calljoin)
//		currentThread->Join(newThread);
//	DEBUG('t', "PASO EL JOIN");
	newThreadw->Fork (multiplico, (void*)threadnamew);

    }
	sumo((void*)"sumo");
	printf("total %d", total);
	//currentThread->Yield();
	
}


/// fin de la prueba de sync ////
*/

/*
//***********************
///probando puertos ////

Lock *cerrojo=new Lock("cerrojo");
Condition *condSend= new Condition("condicionSend", cerrojo);
Condition *condReceive= new Condition("condicionReceive", cerrojo);

Puerto *port= new Puerto("puerto", cerrojo, condSend, condReceive);	

/*
	int i=5;

void Sender(void *tname)
{
	DEBUG('t', "ENTRO A sender");


	port->Send(i++);
}


void Receiver(void *tname)
{
	int m;
	port->Receive(m);
}



void PortTest()	
{

	for ( int k=1; k<=2; k++) {
      char* threadname = new char[100];
      sprintf(threadname, "Hilo %d S", k);
	 char* threadnamew = new char[100];
      sprintf(threadnamew, "Hilo %d R", k);
      	Thread* newThread = new Thread (threadname);
	      	Thread* newThreadw = new Thread (threadnamew);

      	newThread->Fork (Sender, (void *)threadname);
	
	newThreadw->Fork (Receiver,(void *) threadnamew);

    }
	
	Sender((void*)"send");

	//currentThread->Yield();
	
}

*/


/*
////test de join


void Joineado2(void*)
{
	DEBUG('t', "estoy en nieto");
}


void Joineado(void*name){
	DEBUG('t', "estoy en joineado\n");

	Thread *newThread2= new Thread("nieto");
	
	newThread2->Fork(Joineado2,(void*)"nieto");
	newThread2->calljoin=true;



	Thread *newThread22= new Thread("nieto2");
	
	newThread22->Fork(Joineado2,(void*)"nieto2");
	newThread22->calljoin=true;



	currentThread->Yield();
	DEBUG('t', "implemento join\n");
	currentThread->Join(newThread2);
	DEBUG('t', "terminaron nietoS y joineado\n");
}


void JoinTest(){
	
	Lock *cerrojo = new Lock("cerrojo");
	Condition *condSend= new Condition("condicionSend", cerrojo);
	Condition *condReceive= new Condition("condicionReceive", cerrojo);

	Puerto *p=new Puerto("portownR", cerrojo, condSend, condReceive);

	Lock *cerrojo2 = new Lock("cerrojo2");
	Condition *condSend2= new Condition("condicionSend2", cerrojo2);
	Condition *condReceive2= new Condition("condicionReceive2", cerrojo2);

	Puerto *j=new Puerto("portownJ", cerrojo2, condSend2, condReceive2);

	currentThread->portOwn= p;
	currentThread->portRoot= p;


	char *threadname= new char[10];
	threadname="Joineado";
	Thread *newThread= new Thread(threadname);
	
	newThread->Fork(Joineado,(void*)threadname);
	newThread->calljoin=true;						// Lineas necesarias para 
	newThread->portOwn=j;							// que la funcion join
													// pueda actuar correctamente
	
	currentThread->Yield();
	DEBUG('t', "implemento join\n");
	currentThread->Join(newThread);
	DEBUG('t', "terminaron ambos\n");
}

///fin test de join
///////////////////////////

*/

/*
////test de join 1 solo hijo


void Joineado2(void*)
{
	DEBUG('t', "estoy en nieto");
}


void Joineado(void*name){
	DEBUG('t', "estoy en joineado\n");



	Lock *cerrojo2 = new Lock("cerrojo2");
	Condition *condSend2= new Condition("condicionSend2", cerrojo2);
	Condition *condReceive2= new Condition("condicionReceive2", cerrojo2);

	Puerto *j2=new Puerto("portownJ", cerrojo, condSend, condReceive);

	Thread *newThread2= new Thread("nieto");
	
	newThread2->Fork(Joineado2,(void*)"nieto");
	newThread2->calljoin=true;
	newThread2->portOwn=j2;	
	


	Thread *newThread22= new Thread("nieto2");
	
	newThread22->Fork(Joineado2,(void*)"nieto2");


	currentThread->Yield();
	DEBUG('t', "implemento join\n");
	currentThread->Join(newThread2);
	DEBUG('t', "terminó nieto y joineado\n");
}


void JoinTest(){
	
	Lock *cerrojo = new Lock("cerrojo");
	Condition *condSend= new Condition("condicionSend", cerrojo);
	Condition *condReceive= new Condition("condicionReceive", cerrojo);

	Puerto *j=new Puerto("portownR", cerrojo, condSend, condReceive);

	
	char *threadname= new char[10];
	threadname="Joineado";
	Thread *newThread= new Thread(threadname);
	
	newThread->Fork(Joineado,(void*)threadname);
	newThread->calljoin=true;						// Lineas necesarias para 
	newThread->portOwn=j;							// que la funcion join
													// pueda actuar correctamente
	
	currentThread->Yield();
	DEBUG('t', "implemento join\n");
	currentThread->Join(newThread);
	DEBUG('t', "terminaron ambos\n");
}

///fin test de join 1 solo hijo
///////////////////////////
*/
