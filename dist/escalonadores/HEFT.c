#include "../include/escalonador.h"

int terminate = 0;
PtaskList *cpuList;
PtaskList *gpuList;
PtaskList *micList;
pthread_mutex_t global_mutex;
pthread_mutex_t *cpuList_mutex;
pthread_mutex_t *gpuList_mutex;
pthread_mutex_t *micList_mutex; 

void InitializeScheduler(){
    int i;
    pthread_mutex_init(&global_mutex, NULL);
    pthread_mutex_lock(&global_mutex);
    
    if(workerCount[0] != 0){
		cpuList = calloc(workerCount[0]+1,sizeof(PtaskList));
		cpuList_mutex = calloc(workerCount[0]+1,sizeof(pthread_mutex_t));
		for(i = 1; i <= workerCount[0]; i++){
			cpuList[i] = createList();
			pthread_mutex_init(&cpuList_mutex[i],NULL);
		}
	}

	if(workerCount[1] != 0){
		gpuList = calloc(workerCount[1]+1,sizeof(PtaskList));
		gpuList_mutex = calloc(workerCount[1]+1,sizeof(pthread_mutex_t));
		for(i = 1; i <= workerCount[1]; i++){
			gpuList[i] = createList();
			pthread_mutex_init(&gpuList_mutex[i],NULL);
		}
	}

	if(workerCount[2] != 0){
		micList = calloc(workerCount[2]+1,sizeof(PtaskList));
		micList_mutex = calloc(workerCount[2]+1,sizeof(pthread_mutex_t));
		for(i = 1; i <= workerCount[2]; i++){
			micList[i] = createList();
			pthread_mutex_init(&micList_mutex[i],NULL);
		}
	}
 
    pthread_mutex_unlock(&global_mutex);
}

void PushTask(Ttask t){
    int i,j,k,l = 0;
    double cpuTime = 999999999999;
    double gpuTime = 999999999999;
    double micTime = 999999999999;
    int minCPUworker = -1;
    int minGPUworker = -1;
    int minMICworker = -1;
    double cpuTime_tmp = t.cpuTime;
    double gpuTime_tmp = t.gpuTime;
    double micTime_tmp = t.micTime;

	//printf("task %d para ser inserida\n",t.taskID);
    if(t.taskType != TERMINATOR_ID){
		
		if(workerCount[0] != 0){
			for(l = 1; l <= workerCount[0]; l++){
				pthread_mutex_lock(&cpuList_mutex[l]);	
				PtaskListItem it = cpuList[l]->head;
				for(i = 0; i < cpuList[l]->size && it != NULL;i++){
					cpuTime_tmp += it->item.cpuTime;
					it = it->next;
				}
				pthread_mutex_unlock(&cpuList_mutex[l]);
				if(cpuTime_tmp < cpuTime){
					cpuTime = cpuTime_tmp;
					minCPUworker = l;
				}
				cpuTime_tmp = t.cpuTime;
			}
		}

		if(workerCount[1] != 0){
			for(l = 1; l <= workerCount[1]; l++){
				pthread_mutex_lock(&gpuList_mutex[l]);	
				PtaskListItem it = gpuList[l]->head;
				for(i = 0; i < gpuList[l]->size && it != NULL;i++){
					gpuTime_tmp += it->item.gpuTime;
					it = it->next;
				}
				pthread_mutex_unlock(&gpuList_mutex[l]);
				if(gpuTime_tmp < gpuTime){
					gpuTime = gpuTime_tmp;
					minGPUworker = l;
				}
				gpuTime_tmp = t.gpuTime;		
			}
		}
	
		if(workerCount[2] != 0){			
			for(l = 1; l <= workerCount[2]; l++){
				pthread_mutex_lock(&micList_mutex[l]);	
				PtaskListItem it = micList[l]->head;
				for(i = 0; i < micList[l]->size && it != NULL;i++){
					micTime_tmp += it->item.micTime;
					it = it->next;
				}
				pthread_mutex_unlock(&micList_mutex[l]);
				if(micTime_tmp < micTime){
					micTime = micTime_tmp;
					minMICworker = l;
				}
				micTime_tmp = t.micTime;
				
			}	
		}
	//printf("Task %d cpu %f gpu %f mic %f\n", t.taskID, cpuTime, gpuTime, micTime);
	if(gpuTime < micTime && gpuTime < cpuTime && workerCount[1] != 0){
		pthread_mutex_lock(&gpuList_mutex[minGPUworker]);
		pushToList(gpuList[minGPUworker],t, gpuList[minGPUworker]->size-1);
		//printf("task %d inserida em GPU list\n",t.taskID);
		pthread_mutex_unlock(&gpuList_mutex[minGPUworker]);
	}
	else{
		if(micTime < gpuTime && micTime < cpuTime && workerCount[2] != 0){
			pthread_mutex_lock(&micList_mutex[minMICworker]);
			pushToList(micList[minMICworker],t, micList[minMICworker]->size-1);
			//printf("task %d inserida em MIC list\n",t.taskID);
			pthread_mutex_unlock(&micList_mutex[minMICworker]);
		}
		else{
			if(workerCount[0] != 0){
				pthread_mutex_lock(&cpuList_mutex[minCPUworker]);
				pushToList(cpuList[minCPUworker],t, cpuList[minCPUworker]->size-1);
				//printf("task %d inserida em CPU list\n",t.taskID);
				pthread_mutex_unlock(&cpuList_mutex[minCPUworker]);
			}
		}
	}
        
   }else terminate = 1;
    
}

Ttask PopTask(Tworker w){
    Ttask ret;
    int i, j, maxI = 0, maxJ = 0;
    double maxT = 0;
    PtaskListItem it;
     
    if(getWorkerType(w) == 0){
	pthread_mutex_lock(&cpuList_mutex[w.id]);
	ret = pullfromList(cpuList[w.id], 0);
	pthread_mutex_unlock(&cpuList_mutex[w.id]);
    }

    if(getWorkerType(w) == 1){
	int workerIndex = w.id-workerCount[0];
	pthread_mutex_lock(&gpuList_mutex[workerIndex]);
	ret = pullfromList(gpuList[workerIndex], 0);
	pthread_mutex_unlock(&gpuList_mutex[workerIndex]);
    }

    if(getWorkerType(w) == 2){
	int workerIndex = w.id-(workerCount[0]+workerCount[1]);
	pthread_mutex_lock(&micList_mutex[workerIndex]);
	ret = pullfromList(micList[workerIndex], 0);
	pthread_mutex_unlock(&micList_mutex[workerIndex]);
    }
    
    if(terminate && ret.taskType == EMPTY_ID) ret = createTask(0, TERMINATOR_ID, 0,0, NULL,0,0,0);
    return ret;
}





