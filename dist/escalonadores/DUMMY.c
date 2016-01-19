#include "../include/escalonador.h"
int i = 0,terminate = 0;
PtaskList l;
pthread_mutex_t emtx; 

void InitializeScheduler(){
    pthread_mutex_init(&emtx, NULL);
    pthread_mutex_lock(&emtx);
    l = createList();
    pthread_mutex_unlock(&emtx);

}

void PushTask(Ttask t){
    if(t.taskType != TERMINATOR_ID){
        pthread_mutex_lock(&emtx);
        pushToList(l,t, 0);
        pthread_mutex_unlock(&emtx);
   }else terminate = 1;
}

Ttask PopTask(Tworker w){
    Ttask ret;
    pthread_mutex_lock(&emtx);
    ret = pullfromList(l,0);
    pthread_mutex_unlock(&emtx);
    //if(terminate && ret.taskType == EMPTY_ID) ret = createTask(0, TERMINATOR_ID, 0, NULL);
    if(terminate && ret.taskType == EMPTY_ID) ret = createTask(0, TERMINATOR_ID, 0,0, NULL,0,0,0);
    return ret;
}
