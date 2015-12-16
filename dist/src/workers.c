#include "../include/tasks.h"
#include "../include/workers.h"
#include "../include/conf.h"
#include "../include/escalonador.h"
#include <unistd.h>

int id = 0;

void nsleep(long us){
    struct timespec wait;
    wait.tv_sec = us / (1000 * 1000);
    wait.tv_nsec = (us % (1000 * 1000)) * 1000;
    nanosleep(&wait, NULL);
}

Pworker createWorker(unsigned int type){
    int i;
    Pworker w;
    w = malloc(sizeof(Tworker));
    w->totalTime = 0;
    w->workerType = type;
    w->flag = 1;
    w->id = ++id;
    pthread_create(&(w->thread), NULL, work, (void *)w);
    
    return w;
}

unsigned int getWorkerType(Tworker w){
    return w.workerType;
}


int executeTask(Ttask task, Pworker worker){
	
	if(task.taskType != EMPTY_ID){
		
		worker->flag = 1;
		
		if(worker->workerType == 0){
			fprintf(printTemposThreads, "thread type:%u (%d) running task type:%i (%d) in %lf miliseconds\n",worker->workerType, task.taskID, worker->id, task.taskType, task.cpuTime);
			//printf("thread type:%u (%d) running task type:%i (%d) in %lf miliseconds\n",worker->workerType, task.taskID, worker->id, task.taskType, task.cpuTime);
			worker->totalTime += task.cpuTime;
			//usleep(task.cpuTime);
			nsleep(task.cpuTime);
			//sleep(task.cpuTime/1000000);
			
		}
		if(worker->workerType == 1){
			fprintf(printTemposThreads, "thread type:%u (%d) running task type:%i (%d) in %lf miliseconds\n",worker->workerType, task.taskID, worker->id, task.taskType, task.gpuTime);
			//printf("thread type:%u (%d) running task type:%i (%d) in %lf miliseconds\n",worker->workerType, task.taskID, worker->id, task.taskType, task.gpuTime);
			worker->totalTime += task.gpuTime;
			//usleep(task.gpuTime);
			nsleep(task.gpuTime);
			//sleep(task.gpuTime/1000000);
		}
		if(worker->workerType == 2){
			fprintf(printTemposThreads, "thread type:%u (%d) running task type:%i (%d) in %lf miliseconds\n",worker->workerType, task.taskID, worker->id, task.taskType, task.micTime);
			//printf("thread type:%u (%d) running task type:%i (%d) in %lf miliseconds\n",worker->workerType, task.taskID, worker->id, task.taskType, task.micTime);
			worker->totalTime += task.micTime;
			//usleep(task.micTime);
			nsleep(task.micTime);
			//sleep(task.micTime/1000000);
		}
		
		vetorEst[worker->workerType]++;
		tipoThreadTarefa[worker->workerType][task.taskType]++;
		
		clear(task);
		
		fprintf(printTemposThreads, "thread type:%u (%d) total work time %u miliseconds\n",worker->workerType,worker->id,worker->totalTime);
		//printf("thread type:%d (%d) total work time %u miliseconds\n",worker->workerType,worker->id,worker->totalTime);
		return 1;
	}
	
	return 0;	
}

void *work(void *arg){

    Pworker worker = (Pworker) arg;
    Ttask task;
    while(1){
		task = PopTask(*worker);
		if(task.taskType == TERMINATOR_ID) break;
		if(task.taskType != EMPTY_ID){
			executeTask(task,worker);
		}		
	}
    return NULL;
}
