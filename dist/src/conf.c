#include "../include/conf.h"
pthread_mutex_t emtx; 

double fRand(double fMin, double fMax)
{
    fMin /= 100;
    fMax /= 100;
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

//          FUNCTIONS
void parseConf(FILE *f){
    int i,j,k = 0;
    char taskname[100];
    
    vetorUnidades = calloc(3, sizeof(int));

    fscanf(f, "%d %d %d", &vetorUnidades[0], &vetorUnidades[1], &vetorUnidades[2]);
    fscanf(f, "%u %u %u", &workerTypeCount, &taskTypeCount, &numTasks);
    fscanf(f, "%f %f", &minErrorPercentage,&maxErrorPercentage);

    transferSpeed = calloc(workerTypeCount,sizeof(double));
    taskTimes = calloc(taskTypeCount,sizeof(double *));
    
    for(i=0;i<taskTypeCount;i++) taskTimes[i] = calloc(workerTypeCount,sizeof(double));
    for(i=0;i<workerTypeCount;i++) fscanf(f,"%lf ",&transferSpeed[i]);
    for(i=0;i<taskTypeCount;i++) for(j=0;j<workerTypeCount; j++) fscanf(f,"%lf ",&taskTimes[i][j]);

    workerCount = calloc(workerTypeCount,sizeof(unsigned int));
    for(i = 0; i < workerTypeCount; i++){
        fscanf(f,"%u",&workerCount[i]);
        totalCount += workerCount[i];
    }
    
    workerList = calloc(totalCount,sizeof(Pworker));

	InitializeScheduler();
    for(i = 0; i < workerTypeCount; i++){
        for(j = 0; j < workerCount[i]; j++){
            workerList[k] = createWorker((unsigned int) i);
            k++;
        }
    }
    

    idDone = calloc(0,sizeof(int));
    countDone = 0;
    
    tasksFile = fopen("conf_tasks/tasks.txt","r");
}


Ttask getTask(){

    int type,tid,count = 0,i;


    double errorPercentage;
    unsigned int size,tmp[50],*dep,numDependencies;
    double cpuTime,gpuTime,micTime;
    int worker_position = 0;
        
    // Load Task
    if(fscanf(tasksFile,"%d %d %u",&tid,&type,&size) == EOF){
		return createTask(0,EMPTY_ID,0,0,NULL,0,0,0);
	}
    else{
        
        if(vetorUnidades[0] != 0){
            cpuTime = taskTimes[type][0];
            worker_position++;
        }
             
        if(vetorUnidades[1] != 0){  
            gpuTime = taskTimes[type][worker_position];
            stg =transferSpeed[worker_position];        
            worker_position++;
        }
        
        if (vetorUnidades[2] != 0){
            micTime = taskTimes[type][worker_position];
		    stm =transferSpeed[worker_position];
        }
    }
     
    //Calculating Error
    errorPercentage = fRand(minErrorPercentage,maxErrorPercentage);
    
    if(errorPercentage != 0){
		cpuTime = fRand( (cpuTime - (cpuTime*errorPercentage)), (cpuTime + (cpuTime*errorPercentage)));
		gpuTime = fRand( (gpuTime - (gpuTime*errorPercentage)), (gpuTime + (gpuTime*errorPercentage)));
		micTime = fRand( (micTime - (micTime*errorPercentage)), (micTime + (micTime*errorPercentage)));
	}

    //Load Dependencies
	do{
		fscanf(tasksFile,"%u",&tmp[count]);          
		count++;
	}while(tmp[count-1] != 0);
	
	dep = (unsigned int *)calloc(count,sizeof(unsigned int));
	for(i = 0;i < count;i++){
		dep[i] = tmp[i];
	}
    numDependencies = count-1;
    return createTask(tid,type,size,numDependencies,dep,cpuTime,gpuTime,micTime);
}



double getTiming(int taskType, int workerType){
    if(taskType >= taskTypeCount || workerType >= workerTypeCount || workerType < 0 || taskType < 0) return 0;
    return taskTimes[taskType][workerType];
}


void clear(Ttask t){
	
	pthread_mutex_lock(&idDoneMtx);
    countDone++;
    idDone[countDone-1] = t.taskID;
    pthread_mutex_unlock(&idDoneMtx);
}

int isClear(Ttask t){
    int i, j, tam = 0, cont = 0;

    //printf("Tarefa ID == %d // Depende de == %d\n", t.taskID, t.taskDependsOn[0]);

    if(t.taskDependsOn[0] == 0){
        return 1;
    }else{
        if(countDone == 0){
            return 0;
        } else {
            while(t.taskDependsOn[tam] != 0) tam++;
            for(i = 0; i < tam; i++){
                for(j = 0; j < countDone; j++){
                    if(idDone[j] == 0) break;
                    if(t.taskDependsOn[i] == idDone[j]) cont++;                
                    if(cont == tam) {
                        return 1;
                    }
                }
            }
            return 0;
        }
    }
}

