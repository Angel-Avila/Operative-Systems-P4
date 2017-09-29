#include <ucontext.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include "libthreads.h"

extern QUEUE ready;
extern QUEUE waitinginevent[MAXTHREAD];

THANDLER threads[MAXTHREAD];

int currthread=0;
int totthreads=0;
int tcount=0;
int newthread;
int blockevent;
int unblockevent;
FILE *fp;
struct timeval  tv;

struct itimerval old_timer;
double tinic=0.0;

void _printtimethread(int tnum)
{
	double time_in_mill;
	int timeint;
	int i;
	gettimeofday(&tv, NULL);
	time_in_mill = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000 ;
	if(tinic==0.0)
		tinic=time_in_mill;
	timeint=(int)(time_in_mill-tinic);
	fprintf(fp,"Time = %d",timeint);
	fprintf(fp,"\t");
	for(i=0;i<tnum;i++)
		fprintf(fp,"\t");
	fprintf(fp,"%d\n",tnum);
}


void _swapthreads(int old, int new)
{

	currthread=new;
	swapcontext(&threads[old].thread_context, &threads[new].thread_context);

}



void _signal_handler(){

	scheduler(TIMER|currthread);
	_printtimethread(currthread);
}

void _initqueue(QUEUE *q)
{
	q->head=0;
	q->tail=0;
}


void _enqueue(QUEUE *q,int val)
{
	q->elements[q->head]=val;
	// Incrementa al apuntador
	q->head++;
	q->head=q->head%MAXTHREAD;
}


int _dequeue(QUEUE *q)
{
	int valret;
	valret=q->elements[q->tail];
	// Incrementa al apuntador
	q->tail++;
	q->tail=q->tail%MAXTHREAD;
	return(valret);
}

int _emptyq(QUEUE *q)
{
	return(q->head==q->tail);
}


void pause_timer()
{
	struct itimerval zero_timer = { 0 };
	setitimer(ITIMER_PROF,&zero_timer,&old_timer);
}

void resume_timer()
{
	setitimer(ITIMER_PROF,&old_timer,NULL);
}

int Init()
{

	struct itimerval itimer, otimer;
	struct sigaction act;

	_initqueue(&ready);

	// Hilo 0 siempre está en ejecución
	threads[0].status=RUNNING;

	// Inicia el timer
	itimer.it_interval.tv_sec=0;
	itimer.it_interval.tv_usec=TIMESLICE;
	itimer.it_value.tv_sec=0;
	itimer.it_value.tv_usec=TIMESLICE;

	act.sa_handler=_signal_handler;
	sigaddset(&act.sa_mask,SIGALRM);
	act.sa_flags=SA_RESTART;
	sigaction(SIGALRM,&act,0);

	if(setitimer(ITIMER_REAL,&itimer,&otimer)<0)
		perror("Error en el settimer");

}


int CreateThread(void  *function, void *parameters)
{
	ucontext_t return_context;

	pause_timer();
	if(tcount>=MAXTHREAD)
		return(-1);

	tcount++;
	totthreads=tcount;

	threads[tcount].tid=currthread;
	threads[tcount].status=NEW;

   	threads[tcount].thread_context.uc_link          = 0;  // Donde regresa al terminar
    threads[tcount].thread_context.uc_stack.ss_sp   = threads[tcount].iterator_stack; // Stack
    threads[tcount].thread_context.uc_stack.ss_size = sizeof(threads[tcount].iterator_stack);  // Stack tamaño

    getcontext(&threads[tcount].thread_context);
    makecontext(&threads[tcount].thread_context, (void (*)(void)) function, 1, parameters);


	// Invocar al scheduler para meter el thread a la cola
	newthread=tcount;
	scheduler(NEWTHREAD|newthread);
	_printtimethread(currthread);

	resume_timer();
	return(tcount);
}

int GetTid()
{
	return(currthread);
}


int WaitThread(int t)
{
	pause_timer();
	// Si el hilo que espero no ha terminado, guarda mi TID en la cola waiting del hilo t
	if(threads[t].status!=END)
	{

		// Invocar al scheduler para bloquear al hilo en la cola de
		// bloqueados del hilo que espera
		blockevent=t;
		scheduler(BLOCKTHREAD|currthread);
		_printtimethread(currthread);
	}
	resume_timer();

	return(threads[t].retstat);
}

void EndThread(int retstat)
{

	int waitingthread;

	pause_timer();

	if(!_emptyq(&waitinginevent[currthread]))
	{
		waitingthread=_dequeue(&waitinginevent[currthread]);
	}


	scheduler(UNBLOCKTHREAD|waitingthread);
	_printtimethread(currthread);
	totthreads--;		// Disminuye el número de hilos en ejecución
	scheduler(ENDTHREAD|currthread);
	_printtimethread(currthread);
	threads[GetTid()].retstat=retstat;	// Valor de retorno
	resume_timer();


}

void __attribute__ ((constructor)) premain()
{
	fp=fopen("results.txt","w");
	Init();
	sleep(1);
}

void __attribute__ ((destructor)) postmain()
{
	fclose(fp);
}
