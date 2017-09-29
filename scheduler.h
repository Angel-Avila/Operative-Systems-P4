#include <ucontext.h>
#include "commondata.h"

void _enqueue(QUEUE *q,int val);
int _dequeue(QUEUE *q);
int _emptyq(QUEUE *q);
void _swapthreads(int old, int new);
int Init();
int CreateThread(void  *function, void *parameters);
int GetTid();
int WaitThread(int t);
void EndThread(int retstat);
