#include <ucontext.h>
#include "commondata.h"

#define TIMESLICE 10000

void scheduler(int event);
int Init();
int CreateThread(void  *function, void *parameters);
int GetTid();
int WaitThread(int t);
void EndThread(int retstat);


