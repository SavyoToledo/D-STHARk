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
PspeedUpList l;
pthread_mutex_t emtx; 

void *escalonate(void *arg){
    pthread_mutex_init(&emtx, NULL);
    pthread_mutex_lock(&emtx);
    l = malloc(sizeof(TspeedUpList));
    l->h = NULL;
    l->t = NULL;
    l->size = 0;
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
        if(l->h == NULL && l->t == NULL){
            tm->n = l->h;
            tm->p = l->t;
            
            l->h = tm;
            l->t = tm;
                
        }else{
            if(spG < spM){
                search = l->h;
                while(search != NULL && tm->spG < search->spG){
                   search = search->n;
                }
                if(search == NULL){
                    tm->p = l->t;
                    tm->n = NULL;
                    l->t->n = tm;
                    l->t = tm;
               }else{
                    if(search->n == NULL){
                        search->n = tm;
                        tm->n = NULL;
                        tm->p = search;
                        l->t = tm;
                    }else{
                        tm->n = search->n;
                        tm->p = search;
                        search->n = tm;
                    }
               }
            }else{
                search = l->t;
                while(search != NULL && tm->spM < search->spG){
                   search = search->p;
                }
                if(search == NULL){
                    tm->p = NULL;
                    tm->n = l->h;
                    l->h->p = tm;
                    l->h = tm;
               }else{
                    if(search->p == NULL){
                        search->p = tm;
                        tm->p = NULL;
                        tm->n = search;
                        l->h = tm;
                    }else{
                        tm->n = search->n;
                        tm->p = search;
                        search->n = tm;
                    }
               }
            
            }
        }
        l->size += 1;
        pthread_mutex_unlock(&emtx);       
    }else terminate = 1;
}

Ttask popTask(Tworker w){
    Ttask ret;
    int i;
    PspeedUpTask temp = l->h;
    
    if (l->size == 0) return createTask(0, EMPTY_ID, 0, NULL);
    
    pthread_mutex_lock(&emtx);
    switch(w.workerType){
        case 0:
            printf("case 0\n");
            ret = l->h->t;
            l->h = l->h->n;
            l->h->p = NULL;
            break;
        case 1:
            printf("case 1\n");
            for(i=0;i<(l->size)/2;i++){
                temp = temp->n;
            }
            ret = temp->t;
            if(temp->p != NULL)temp->p->n = temp->n;
            else l->t = temp->n;
            if(temp->n != NULL) temp->n->p = temp->p;
            else l->h = temp->p;
            
            break;
        case 2:
            printf("case 2\n");
            ret = l->t->t;
            l->t = l->t->p;
            l->t->n = NULL;
            break;
    }
    l->size -= 1;
    pthread_mutex_unlock(&emtx);
    
    return ret;
}
