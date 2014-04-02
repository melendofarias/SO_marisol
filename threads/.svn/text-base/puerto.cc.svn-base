#include "system.h"
#include "puerto.h"
#include "synch.h"

Puerto::Puerto(const char *debugName, Lock * pcerrojo, Condition *pScond, Condition *pRcond)
{
	name=debugName;

	int mensaje;	
	cerrojo = pcerrojo;
	condSend=pScond;
	condReceive=pRcond;
	//Lock *cerrojo = new Lock("cerrojo");
	//Condition *condSend= Condition("condicionSend", cerrojo);
	//Condition *condReceive= new Condition("condicionReceive", cerrojo);
}

Puerto::~Puerto()
{
	condSend->~Condition();
	condReceive->~Condition();
}

void
Puerto::Send(int m)
{	DEBUG('t', "DESEO ADQUIRIR EL CERROJO \n");   
	cerrojo->Acquire();

	condReceive->Signal();			//despierto a algun receptor,si lo hay
	condSend->Wait();  			//espero a que el receptor confirme
						//la recepcion del mensaje m
	if(!(cerrojo->isHeldByCurrentThread()))
		cerrojo->Acquire();
	mensaje=m;
	DEBUG('t', "mensaje= %d \n",mensaje);
	condReceive->Signal();			//despierto a algun receptor,si lo hay
	cerrojo->Release();
}

void
Puerto::Receive(int m)
{							//m guardará todos los mensajes
	cerrojo->Acquire();				//recibidos por los distintos
							//receptores, es global	
	condSend->Signal();
	 condReceive->Wait();

	if(!(cerrojo->isHeldByCurrentThread()))		
		cerrojo->Acquire();
	
	m=mensaje;								//guardo en m el mensaje recibido
	DEBUG('t', "\n\n %s recibió %d en el puerto %s \n", currentThread->getName(), mensaje, name);	//confirmo la recepcion
	
	mensaje=NULL;								
		
	cerrojo->Release();
}		







