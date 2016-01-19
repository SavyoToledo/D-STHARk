#include "../include/escalonador.h"
int k = 0,terminate = 0;
PtaskList global_list;
pthread_mutex_t global_mutex; 

void InitializeScheduler(){
    int i;
    pthread_mutex_init(&global_mutex, NULL);
    pthread_mutex_lock(&global_mutex);
    global_list = calloc(1,sizeof(PtaskList));
    global_list = createList();
    pthread_mutex_unlock(&global_mutex);

}

void PushTask(Ttask t){
    if(t.taskType!=TERMINATOR_ID){
        pthread_mutex_lock(&global_mutex);
        pushToList(global_list,t, global_list->size-1);
        pthread_mutex_unlock(&global_mutex);
   }else terminate = 1;
}

Ttask PopTask(Tworker w){
    Ttask ret;
    pthread_mutex_lock(&global_mutex);
    ret = pullfromList(global_list, 0);
    pthread_mutex_unlock(&global_mutex);
    //if(terminate && ret.taskType == EMPTY_ID) ret = createTask(0, TERMINATOR_ID, 0, NULL);
    if(terminate && ret.taskType == EMPTY_ID) ret = createTask(0, TERMINATOR_ID, 0,0, NULL,0,0,0);
    return ret;
}

