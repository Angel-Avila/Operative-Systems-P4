#include "scheduler.h"

extern THANDLER threads[MAXTHREAD];
extern int currthread;
extern int blockevent;
extern int unblockevent;

#define QUANTUM 1
#define QUEUE_LEVELS 10

QUEUE ready;
QUEUE waitinginevent[MAXTHREAD];

QUEUE feedback_q[QUEUE_LEVELS];

int quantum = 0;
int threadCount = 0;
int threadsID[MAXTHREAD] = {-1};
int threadsPriority[MAXTHREAD] = {-1};

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
		// _enqueue(&ready,callingthread);
		_enqueue(&feedback_q[0],callingthread);

		int tempCounter, threadIndex;

		for(tempCounter = 0; tempCounter<MAXTHREAD; tempCounter++)
		{
			if(threadsID[tempCounter] == -1)
			{
				threadIndex = tempCounter;
				break;
			}
		}

		threadsID[threadIndex] = callingthread;
		threadsPriority[threadIndex] = 0;
		threadCount++;
	}

	if(event==BLOCKTHREAD)
	{

		threads[callingthread].status=BLOCKED;
		_enqueue(&waitinginevent[blockevent],callingthread);

		changethread=1;
	}

	if(event==ENDTHREAD)
	{
		int tempCounter, threadIndex;

		threads[callingthread].status=END;

		for(tempCounter = 0; tempCounter<MAXTHREAD; tempCounter++)
		{
			if(threadsID[tempCounter] == callingthread)
			{
				threadIndex = tempCounter;
				break;
			}
		}

		threadsID[threadIndex] = -1;
		threadsPriority[threadIndex] = -1;
		threadCount--;
		changethread=1;
	}

	if(event==UNBLOCKTHREAD)
	{
		int tempCounter, threadIndex;
		for(tempCounter = 0; tempCounter<MAXTHREAD; tempCounter++)
		{
			if(threadsID[tempCounter] == callingthread)
			{
				threadIndex = tempCounter;
				break;
			}
		}

		threads[callingthread].status=READY;
		_enqueue(&feedback_q[threadsPriority[threadIndex]],callingthread);
		// _enqueue(&ready,callingthread);

	}

	if(event==TIMER)
	{
		quantum++;
		if(quantum==QUANTUM)
		{
			threads[callingthread].status=READY;
			quantum = 0;
			changethread=1;
		}
	}

	if(changethread)
	{
		int tempCounter, threadIndex;

		old=currthread;

		for(tempCounter = 0; tempCounter<MAXTHREAD; tempCounter++)
		{
			if(threadsID[tempCounter] == callingthread)
			{
				threadIndex = tempCounter;
				break;
			}
		}

		if((threadsPriority[threadIndex] < QUEUE_LEVELS) && (threadCount != 1))
		{
			threadsPriority[threadIndex]++;
		}

		_enqueue(&feedback_q[threadsPriority[threadIndex]],callingthread);

		threadIndex = 0;
		for(tempCounter = 0; tempCounter<MAXTHREAD; tempCounter++)
		{
			if((threadsPriority[threadIndex] < threadsPriority[tempCounter]) && (threads[callingthread].status == READY) && (threadsPriority[threadIndex] != -1))
			{
				threadIndex = tempCounter;
			}
		}

		next=_dequeue(&feedback_q[threadsPriority[threadIndex]]);

		// next=_dequeue(&ready);

		threads[next].status=RUNNING;
		_swapthreads(old,next);
	}

}
