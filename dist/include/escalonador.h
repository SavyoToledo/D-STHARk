#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "workers.h"
#include "tasks.h"
#include "conf.h"

int escReady;
//PtaskList *l;


void InitializeScheduler();   // mains escalonator's fuction
void PushTask(Ttask t);     // function to submit a task for it to be scalonated
Ttask PopTask(Tworker w);     // function that asks a task for the worker
