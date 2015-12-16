#include "include/escalonador.h"

typedef struct SspeedUpTask{
    Ttask t;
    float spG,spM;
    struct SspeedUpTask *n,*p;
}TspeedUpTask,*PspeedUpTask;

typedef struct SspeedUpList{
    PspeedUpTask h,t;
    int size;
}TspeedUpList,*PspeedUpList;

int terminate = 0;
PspeedUpList listaSpeedUp;
pthread_mutex_t emtx; 

void *escalonate(void *arg){
    pthread_mutex_init(&emtx, NULL);
    pthread_mutex_lock(&emtx);
    listaSpeedUp = malloc(sizeof(TspeedUpList));
    listaSpeedUp->h = NULL;
    listaSpeedUp->t = NULL;
    listaSpeedUp->size = 0;
    pthread_mutex_unlock(&emtx);
    escReady = 1;
    return NULL;
}

void submitTask(Ttask t){
    int i;
    float spG,spM;
    PspeedUpTask search,tm = malloc(sizeof(TspeedUpTask));
    if(t.taskType!=TERMINATOR_ID){
        spG = getTiming(t.taskType,0)/getTiming(t.taskType,1); 
        spM = getTiming(t.taskType,2)/getTiming(t.taskType,1);
        
        tm->t = t;
        tm->spG = spG;
        tm->spM = spM;
        
        pthread_mutex_lock(&emtx);
        if(listaSpeedUp->h == NULL && listaSpeedUp->t == NULL){
            tm->n = listaSpeedUp->h;
            tm->p = listaSpeedUp->t;
            
            listaSpeedUp->h = tm;
            listaSpeedUp->t = tm;
                
        }else{
            if(spG < spM){
                search = listaSpeedUp->h;
                while(search != NULL && tm->spG < search->spG){
                   search = search->n;
                }
                if(search == NULL){
                    tm->p = listaSpeedUp->t;
                    tm->n = NULL;
                    listaSpeedUp->t->n = tm;
                    listaSpeedUp->t = tm;
               }else{
                    if(search->n == NULL){
                        search->n = tm;
                        tm->n = NULL;
                        tm->p = search;
                        listaSpeedUp->t = tm;
                    }else{
                        tm->n = search->n;
                        tm->p = search;
                        search->n = tm;
                    }
               }
            }else{
                search = listaSpeedUp->t;
                while(search != NULL && tm->spM < search->spG){
                   search = search->p;
                }
                if(search == NULL){
                    tm->p = NULL;
                    tm->n = listaSpeedUp->h;
                    listaSpeedUp->h->p = tm;
                    listaSpeedUp->h = tm;
               }else{
                    if(search->p == NULL){
                        search->p = tm;
                        tm->p = NULL;
                        tm->n = search;
                        listaSpeedUp->h = tm;
                    }else{
                        tm->n = search->n;
                        tm->p = search;
                        search->n = tm;
                    }
               }
            
            }
        }
        listaSpeedUp->size += 1;
        pthread_mutex_unlock(&emtx);       
    }else terminate = 1;
}

Ttask popTask(Tworker w){
    Ttask ret;
    int i;
    PspeedUpTask temp = listaSpeedUp->h;
    
    if (listaSpeedUp->size == 0) return createTask(0, EMPTY_ID, 0, NULL);
    
    pthread_mutex_lock(&emtx);
    switch(w.workerType){
        case 0:
            printf("case 0\n");
            ret = listaSpeedUp->h->t;
            listaSpeedUp->h = listaSpeedUp->h->n;
            listaSpeedUp->h->p = NULL;
            break;
        case 1:
            printf("case 1\n");
            for(i=0;i<(listaSpeedUp->size)/2;i++){
                temp = temp->n;
            }
            ret = temp->t;
            if(temp->p != NULL)temp->p->n = temp->n;
            else listaSpeedUp->t = temp->n;
            if(temp->n != NULL) temp->n->p = temp->p;
            else listaSpeedUp->h = temp->p;
            
            break;
        case 2:
            printf("case 2\n");
            ret = listaSpeedUp->t->t;
            listaSpeedUp->t = listaSpeedUp->t->p;
            listaSpeedUp->t->n = NULL;
            break;
    }
    listaSpeedUp->size -= 1;
    pthread_mutex_unlock(&emtx);
    
    return ret;
}
