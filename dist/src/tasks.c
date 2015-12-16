#include <stdio.h>
#include <stdlib.h>
#include "../include/tasks.h"
#include "../include/conf.h"


//      FUNCTIONS
PtaskList createList(){
    PtaskList l;

    l = malloc(sizeof(TtaskList));
    
    l->size = 0;
    l->head = NULL;
    l->tail = NULL;

    return l;
}


//Não é usada
void emptyList(PtaskList l){


        PtaskListItem it = l->head;
        if(it != NULL){
            while (it->next != NULL){   // frees up the space taken by the old list
                it = it->next;
                free(it->prev);
            }
            free(it);
        }

        l->size = 0;                    // sets the parameters of those of a empty list
        l->head = NULL;
        l->tail = NULL;
}

//Não é usada
/*
TtaskList copyList(TtaskList l){
    int i,count,j;
    unsigned int *dep,Dtmp[50];
    PtaskList tmp = malloc(sizeof(TtaskList));
    PtaskListItem it = l.head;

    tmp->size = 0;                                              // sets to 0 so that the create task can alter it

    for(i=0;i < l.size && it != NULL;i++){          // adds a new task with the same attributes of each on the other list
        count = 0;
        do{
           Dtmp[count] = it->item.taskDependsOn[count];
           count++;
        }while(Dtmp[count-1] != 0);
        dep = calloc(count,sizeof(unsigned int));
        for(j = 0;j < count;j++){
            dep[j] = Dtmp[j];
        }
        pushToList(tmp,createTask(it->item.taskID,it->item.taskType,it->item.taskType,dep),i);
        it = it->next;
    }

    return *tmp;
}
*/

void pushToList(PtaskList l, Ttask t, int pos){
    int i;
    PtaskListItem itm = malloc(sizeof(TtaskListItem));
    PtaskListItem it = l->head;

    itm->item.taskSize = t.taskSize;
    itm->item.taskType = t.taskType;
    itm->item.taskID = t.taskID;
    itm->item.taskDependsOn = t.taskDependsOn;
    
    //MUDEI ISSO AQUIIII ---------------------------------------

    itm->item.cpuTime = t.cpuTime;
    itm->item.gpuTime = t.gpuTime;
    itm->item.micTime = t.micTime;
	
	// --------------------------------------------------------- 


    for(i=0;i < pos && it != NULL;i++){
        it = it->next;
    }

    itm->next = it;

    if(it == NULL) itm->prev = l->tail;
    else itm->prev = it->prev;

    if(itm->next == NULL) l->tail = itm;
    if(itm->prev == NULL) l->head = itm;

    if(itm->next != NULL) itm->next->prev = itm;
    if(itm->prev != NULL) itm->prev->next = itm;

    l->size++;
}

/*
typedef struct {
    Ttask retorno1;
    PtaskListItem retorno2;
} Gambiarra;

//se tiver assim, por ex:
t = pullfromList(undone, 0);
// vai virar
Gambiarra g;
g = pullfromList(undone, 0);
t = g.retorno1;
*/

Ttask pullfromList(PtaskList l, int pos){
    
    
    int i;
    TtaskListItem itm;
    PtaskListItem it = l->head;
    
    //MUDEI AQUI ---------------------------------------------
    if(it == NULL) return createTask(0,EMPTY_ID,0,0,NULL,0,0,0);
    //--------------------------------------------------------

    for(i = 0; i < pos && it != NULL; i++){
        it = it->next;
    }

    if(it == NULL) itm = *(l->tail);
    else itm = *it;

    // AQUI FOI MUDADO DE ITM PARA IT
    if(it->next == NULL && it->prev == NULL){
        emptyList(l);
    } else {
        if(it->next == NULL) l->tail = it->prev;
        if(it->prev == NULL) l->head = it->next;

        if(it->next != NULL) it->next->prev = it->prev;
        if(it->prev != NULL) it->prev->next = it->next;
        free(it);
        //printf("Deu FREE\n");
    }

    //Imprimir(l);
    l->size--;
    return itm.item;
}

int getListSize(TtaskList l){
    return l.size;
}

int getPListSize(PtaskList undone){
    return undone->size;
}

int searchList(TtaskList l,Ttask t){
    int i;
    PtaskListItem it = l.head;

    for(i = 0; i < l.size && it != NULL;i++){
        if(it->item.taskSize == t.taskSize && it->item.taskType == t.taskType && it->item.taskID == t.taskID) return i;
        it = it->next;
    }

    return -1;

}

Ttask createTask(int tid, int type, unsigned int size, unsigned int numDeps, unsigned int *dependencies, double cpuTime, double gpuTime, double micTime){
    
    Ttask cTask;
    
    cTask.taskID = tid;
    cTask.taskSize = size;
    cTask.taskType = type;
    cTask.taskDependsOn = dependencies;
    cTask.numDependencies = numDeps;
    
    //MUDEI AQUIIII ------------------------------------------------
    cTask.cpuTime = cpuTime;
    cTask.gpuTime = gpuTime;
    cTask.micTime = micTime;
    //------------------------------------------------------------

    return cTask;
}

int getTaskType(Ttask t){
    return t.taskType;
}

unsigned int getTaskSize(Ttask t){
    return t.taskSize;
}

void Imprimir(PtaskList lista){
    PtaskListItem aux;
    int temp = 0;
    int i = 0;
    if(lista->head == NULL) printf("LISTA VAZIA.\n");
    else{
        printf("CABEÇA == %d \n", lista->head->item.taskID);
        aux = lista->head->next;
        while(aux != NULL){
            printf("OUTRO == %d \n", aux->item.taskID);
            aux = aux->next;
        }
    }
}
