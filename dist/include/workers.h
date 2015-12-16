//         HEADER GUARD
#ifndef SIMWORK_H_INCLUDED
#define SIMWORK_H_INCLUDED

//          INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "tasks.h"

int *vetorEst, **tipoThreadTarefa;
FILE *printTemposThreads;

//          TYPES
typedef struct Sworker{                                 // define a worker
    unsigned int workerType,totalTime;                  // the type of the worker ( 0 to the number defined in the conf file )
    int flag;                                           // flag to output
    int id;                                             // thread ID
    PtaskList tasks;                                    // task list of the worker
    pthread_t thread;                                   // thread of the worker
    pthread_mutex_t mtx;                                // thread mutex of the worker
}Tworker,*Pworker;

//          FUNCTIONS
Pworker createWorker(unsigned int);                     // function to create a worker
unsigned int getWorkerType(Tworker);                    // function to get the type of a worker
TtaskList getWorkerTasks(Tworker);                      // function to get the tasks of the worker
void addWorkerTask(Pworker,Ttask);                      // function to add a task to the worker
Ttask removeWorkerTask(Pworker,int);                    // function to remove the Nth task of the worker
void *work(void *);                                     // main worker function
void nsleep(long us);									// nanosleep function

#endif

