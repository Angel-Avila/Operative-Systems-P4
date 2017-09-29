#include "scheduler.h"

extern THANDLER threads[MAXTHREAD];
extern int currthread;
extern int blockevent;
extern int unblockevent;

#define QUANTUM 1

QUEUE ready;
QUEUE waitinginevent[MAXTHREAD];

int quantum = 0;

void scheduler(int arguments)
{
	int old,next;
	int changethread=0;
	int waitingthread=0;

	int event=arguments & 0xFF00;
	int callingthread=arguments & 0xFF;

	if(event==NEWTHREAD)
	{
		// Un nuevo hilo va a la cola de listos
		threads[callingthread].status=READY;
		_enqueue(&ready,callingthread);
	}

	if(event==BLOCKTHREAD)
	{

		threads[callingthread].status=BLOCKED;
		_enqueue(&waitinginevent[blockevent],callingthread);

		changethread=1;
	}

	if(event==ENDTHREAD)
	{
		threads[callingthread].status=END;
		changethread=1;
	}

	if(event==UNBLOCKTHREAD)
	{
			threads[callingthread].status=READY;
			_enqueue(&ready,callingthread);
	}

	if(event==TIMER)
	{
		quantum++;
		if(quantum==QUANTUM)
		{
			threads[callingthread].status=READY;
			_enqueue(&ready,callingthread);
			quantum = 0;
			changethread=1;
		}
	}

	if(changethread)
	{
		old=currthread;
		next=_dequeue(&ready);

		threads[next].status=RUNNING;
		_swapthreads(old,next);
	}

}
