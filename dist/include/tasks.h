#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

//         HEADER GUARD
#ifndef SIMTASK_H_INCLUDED
#define SIMTASK_H_INCLUDED

//          TYPES

typedef struct Stask{                   // define a task
    int taskType;                       // the type of the task ( 0 to the number defined in the conf file, plus special cases )
    int taskID;                         // task ID
    
    //MUDEI ISSO AQUI -----------------------------------------------------
    double cpuTime;
    double gpuTime;
    double micTime;
    // --------------------------------------------------------------------

    unsigned int taskSize;              // size of the task ( MB )
    unsigned int *taskDependsOn;        // tasks that this depends
    unsigned int numDependencies;
}Ttask,*Ptask;

typedef struct StaskListItem{           // define a item from a task list
    Ttask item;                         // the task itself
    struct StaskListItem *next, *prev;  // pointers forward and backward
}TtaskListItem,*PtaskListItem;

typedef struct StaskList{               // define a task list
    unsigned int size;                  // number of list items
    PtaskListItem head,tail;            // next and previous items
}TtaskList,*PtaskList;

//          FUNCTIONS
PtaskList createList();                 // function to create an empty list
void emptyList(PtaskList);              // function to empty a list
TtaskList copyList(TtaskList);          // function to copy a list
void pushToList(PtaskList, Ttask, int); // function to add a task to a list in a position
Ttask pullfromList(PtaskList, int);     // function to remove a task from a position in a list
int getListSize(TtaskList);             // function to get the size of a list
int searchList(TtaskList,Ttask);        // function to get position of a task in the list
void Imprimir(PtaskList lista);			// Imprime os itens da lista
int getPListSize(PtaskList undone);


//MUDEI AQUI -----------------------------------------------------------------------------------------------
Ttask createTask(int tid, int type, unsigned int size, unsigned int numDeps, unsigned int *dependencies, double cpuTime, double gpuTime, double micTime);    // function to create a task
//----------------------------------------------------------------------------------------------------------

int getTaskType(Ttask);                 // function to get the type of the task
unsigned int getTaskSize(Ttask);        // function to get the size of the task

#endif
