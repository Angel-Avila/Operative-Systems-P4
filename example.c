#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "libthreads.h"

#define NTHREADS 3

void loop1(void *par)
{
	int i,j;

	int ini;
	int num=*((int *) par);
	ini=GetTid()*10;
	for (i=ini; i < ini+10; ++i) {
		printf("Ejecución de loop1 i=%d, hilo=%d, parámetro=%d\n",i,GetTid(),num);
		for(j=0;j<100000000;j++);
		sleep(1);
	}

	EndThread(0);
}


void loop2()
{
	int i,j;

	for (i=10; i < 15; ++i) {
		printf("Ejecución de loop2 i=%d, hilo=%d\n",i,GetTid());
		for(j=0;j<100000000;j++);
		sleep(1);
	}

	EndThread(0);
}


int main()
{
	int i,j;
	int n,m;
	int t1,t2,t3;
	int stat;

	for(i=0;i<100000000;i++);

	n=1;
	t1=CreateThread(loop1, (void *) &n);
	t2=CreateThread(loop2, NULL);
	m=3;



   /*for(i=100; i < 110; ++i) {
		printf("Ejecución en main i=%d, hilo=%d\n",i,GetTid());
		for(j=0;j<200000000;j++);
    } */

	t3=CreateThread(loop1, (void *) &m);
	stat=WaitThread(t2);
	printf("Termina hilo %d con status = %d\n",t2,stat);
	stat=WaitThread(t1);
	printf("Termina hilo %d con status = %d\n",t1,stat);
	stat=WaitThread(t3);
	printf("Termina hilo %d con status = %d\n",t3,stat);
}
