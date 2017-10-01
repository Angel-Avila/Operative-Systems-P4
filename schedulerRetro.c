#include "scheduler.h"
#include <stdio.h>

extern THANDLER threads[MAXTHREAD];
extern int currthread;
extern int blockevent;
extern int unblockevent;

#define QUANTUM 1
#define QUEUE_LEVELS 10 // Niveles de prioridades para las colas de retro

typedef struct queue { // Estructura para colas de retro
	int elements[MAXTHREAD];
	int head;
	int tail;
} queue_t;

// Funciones para uso de la estructura
void init_q(queue_t *q);
void push_t(queue_t *q,int val);
int pop_t(queue_t *q);
int isempty_q(queue_t *q);

void init_q(queue_t *q) // Inicial la cola
{
	q->head=0;
	q->tail=0;
}

void push_t(queue_t *q,int val) // Se guarda un valor en la cola
{
	q->elements[q->tail]=val;
	q->tail++;
	q->tail=q->tail%MAXTHREAD;
}

int pop_t(queue_t *q) // Se quita un valor de la cola
{
	int valret;
	valret=q->elements[q->head];
	q->head++;
	q->head=q->head%MAXTHREAD;
	return(valret);
}

QUEUE ready;
QUEUE waitinginevent[MAXTHREAD];

queue_t feedback_q[QUEUE_LEVELS]; // Colas de reto

int quantum = 0;
int threadCount = 0;
int threadsPriority[MAXTHREAD]; // vector que guarda la prioridad de los procesos
int init_flag = 1; // Bandera para inicialización única

void scheduler(int arguments)
{
	int old,next;
	int changethread=0;
	int waitingthread=0;

	int event=arguments & 0xFF00;
	int callingthread=arguments & 0xFF;

	if(init_flag == 1) // Se inicializan las colas y valores de prioridades una sola vez
	{
		int counter;
		init_flag == 0;
		for(counter = 0; counter<QUEUE_LEVELS; counter++)
		{
			init_q(&feedback_q[counter]);
			threadsPriority[counter] = QUEUE_LEVELS;
		}
	}

	if(event==NEWTHREAD)
	{
		// Un nuevo hilo va a la cola de listos
		threads[callingthread].status=READY;
		threadsPriority[callingthread] = 0; // Proceso inicia con prioridad 0
		threadCount++;	// Se aumenta la cantidad de procesos en ejecucción
		_enqueue(&ready,callingthread);
		printf("%d\n", callingthread);
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
		threadsPriority[callingthread] = QUEUE_LEVELS; // Se reinicializa la prioridad del proceso
		threadCount--; // Decremanta la cantidad de procesos en ejecucción
		changethread=1;
	}

	if(event==UNBLOCKTHREAD)
	{
			threads[callingthread].status=READY;
			_enqueue(&ready,callingthread);
	}

	if(event==TIMER)
	{
		quantum++; // Contador de quantums
		if(quantum==QUANTUM)
		{
			if(threadsPriority[callingthread] < QUEUE_LEVELS) // Se aumenta el nivel de prioridad
			{
				threadsPriority[callingthread]++;
			}
			if(threadCount == 1) // Se deja en 0 si sólo hay un proceso
			{
				threadsPriority[callingthread] = 0;
			}

			push_t(&feedback_q[threadsPriority[callingthread]], callingthread); // Se guarda el proceso actual en la cola que le corresponde de acuerdo a la prioridad

			int counter, threadIndex = 0;
			for(counter = QUEUE_LEVELS-1; counter>=0; counter--) // Se busca la prioridad más alta
			{
				if((threadsPriority[counter] <= threadsPriority[threadIndex] && (threads[counter].status==READY || threads[counter].status==RUNNING)))
				{
					threadIndex = counter;
				}
			}
			_enqueue(&ready,pop_t(&feedback_q[threadsPriority[threadIndex]])); // Se guarda en la cola de listos el proceso con la mayor prioridad

			quantum = 0; // Se reinicia el contador de quantums
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
