
#include "thread.h"
#include "list.h"
#include "synch.h"

class Puerto{
	public:
		Puerto(const char *debugName, Lock * cerrojo, Condition *condS, Condition *condR);
		~Puerto();
		void Send(int m);
		void Receive(int m);

		Lock *cerrojo;
		Condition *condSend;
		Condition *condReceive;

		int mensaje; 			//mensajes que llegaron al puerto


	private:
		const char* name;
};		

	



