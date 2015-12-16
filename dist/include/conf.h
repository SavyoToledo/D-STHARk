//         HEADER GUARD
#ifndef SIMCONF_H_INCLUDED
#define SIMCONF_H_INCLUDED

//          INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include "workers.h"
#include "tasks.h"

PtaskList undone;
int numTasks;

//          DEFINES
#define TERMINATOR_ID -1                //id da task de fim de execucao
#define EMPTY_ID -2                     //id da task vazia
#define NULL_ID -3                      //id da task nula

//          GLOBALS
double cpuTime,gpuTime,micTime;

double *transferSpeed;
double **taskTimes;
float minErrorPercentage, maxErrorPercentage;
unsigned int workerTypeCount, taskTypeCount;
unsigned int *workerCount, totalCount;
unsigned int *idDone, countDone;
unsigned int stg, stm;
Pworker *workerList;
FILE *tasksFile;
FILE *printEstastisticasPizza;
FILE *printEstastisticasBarras;
FILE *f;
FILE *total_time;

// Vetor para informar quais unidades de processamento existem
// 1 -> existe
// 0 -> não existe
// Na ordem respectivamente, CPU, GPU e MIC
int *vetorUnidades; 


// Mutex para função "clear"
pthread_mutex_t idDoneMtx; 

//          FUNCTIONS
void parseConf(FILE *);                 // function to parse the conf file
Ttask getTask();                        // function to get the next task from file
double getTiming(int, int);        // function to generate the time for the worker
void clear(Ttask t);                    // function to clear a dependencie
int isClear(Ttask t);                   // function to check if task is clear

#endif
