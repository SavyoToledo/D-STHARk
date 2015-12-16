#include "../include/escalonador.h"

int terminate = 0;
PtaskList *l;
pthread_mutex_t emtx; 

void InitializeScheduler(){
    int i;
    pthread_mutex_init(&emtx, NULL);
    pthread_mutex_lock(&emtx);
    l = calloc(workerTypeCount,sizeof(PtaskList));
    for(i = 0; i < workerTypeCount; i++) l[i] = createList();
    pthread_mutex_unlock(&emtx);
    escReady = 1;

}

void PushTask(Ttask t){
    int i, min = 0;
        double minT;



        if(t.taskType!=TERMINATOR_ID){
    	    //t.gpuTime += t.taskSize / stg;
			//t.micTime += t.taskSize / stm;   

            if (vetorUnidades[0]){
                if (vetorUnidades[1]){
                    if (vetorUnidades[2]){
                        // CPU, GPU, MIC
                        if ((t.cpuTime < t.gpuTime) && (cpuTime < micTime)) min = 0;
                        if ((t.gpuTime < t.cpuTime) && (gpuTime < micTime)) min = 1;
                        if ((t.micTime < t.cpuTime) && (micTime < gpuTime)) min = 2;
                    }else{
                        // CPU, GPU
                        if (t.cpuTime < t.gpuTime) min = 0;
                        else min = 1;
                    }
                }else{
                        // CPU MIC
                    if (vetorUnidades[2]){
                        if (t.cpuTime < t.micTime) min = 0;
                        else min = 1;
                    }else{
                        // CPU
                        min = 0;
                    }
                }
            }else{
                if (vetorUnidades[1]){          
                    if (vetorUnidades[2]){
                        // GPU MIC
                        if (t.gpuTime < t.micTime) min = 0;
                        else min = 1;
                    }else{
                        // GPU
                        min = 0;
                    }
                }else{
                    if (vetorUnidades[2]){
                        // MIC
                        min = 0;
                    }
                }   
            }










            /*for(i = 1;i < workerTypeCount; i++){
                if(getTiming(getTaskType(t),i) + (t.taskSize) / transferSpeed[i] < minT){
                    min = i;
                    minT = getTiming(getTaskType(t) + (t.taskSize) / transferSpeed[i],i);
            }
            */
        
            
            pthread_mutex_lock(&emtx);
            pushToList(l[min],t, i);
            pthread_mutex_unlock(&emtx);

        }else terminate = 1;
}

Ttask PopTask(Tworker w){
    Ttask ret;
    int i, j, maxI = 0, maxJ = 0;
    double maxT = 0;
    PtaskListItem it;
     
    pthread_mutex_lock(&emtx);
    ret = pullfromList(l[getWorkerType(w)], 0);
    if(ret.taskType == EMPTY_ID){
       for(i = 0;i < workerTypeCount; i++){
            j = 0;
            it = l[i]->head;
            while (it != NULL){
                if(getTiming(getTaskType(it->item),i) + (it->item.taskSize) / transferSpeed[i] > maxT){
                    maxT = getTiming(getTaskType(it->item),i) + (it->item.taskSize) / transferSpeed[i];
                    maxI = i;
                    maxJ = j;
                }
                it = it->next;
                j++;
            }          
       }
       ret = pullfromList(l[maxI], maxJ);
   } 
    pthread_mutex_unlock(&emtx);
    
    //MUDEI AQUI -------------------------------------------------------------------------------
    if(terminate && ret.taskType == EMPTY_ID) ret = createTask(0, TERMINATOR_ID,0, 0, NULL,0,0,0);
    //------------------------------------------------------------------------------------------
    return ret;
}
