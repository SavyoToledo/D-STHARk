#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "../include/conf.h"
#include "../include/tasks.h"
#include "../include/workers.h"
#include "../include/escalonador.h"
#include "../include/balanceador.h"



int ConfigureSimulator(){
	int i , j ;
	f = fopen("conf_tasks/conf.txt","r");
	
    parseConf(f);

	pthread_mutex_init(&idDoneMtx, NULL);
	countDone = 0;
	idDone = (unsigned int *) calloc(numTasks, sizeof(unsigned int));
	
	undone = calloc(1,sizeof(PtaskList));
	undone = createList();
	
	
}

int PrepareStatistics(){
    printEstastisticasPizza = fopen("EstatisticasPizza.txt", "a");
    printEstastisticasBarras = fopen("EstatisticasBarras.txt", "a");
	printTemposThreads = fopen("TemposThreads.txt", "a");
	total_time = fopen("TotalTime.txt","w");
}

void printList(TtaskList l){
    PtaskListItem it = l.head;
    printf("items: %d\n",l.size);
    if(it != NULL) printf("head: %d - size: %u MB\n",l.head->item.taskType,l.head->item.taskSize);
    if(it != NULL) printf("tail: %d - size: %u MB\n",l.tail->item.taskType,l.tail->item.taskSize);
    while (it != NULL){
        printf("%d - size: %u MB\n",it->item.taskType,it->item.taskSize);
        it = it->next;
    }
    printf("\n");
}

void printTask(Ttask t){
    printf("%d - size: %u MB\n",t.taskType,t.taskSize);
}

typedef struct timeval Time;

int main(){
	

    Time start,end;
	int i,j;
    double total_execution_time;
    srand(time(NULL));

    gettimeofday(&start,0);


//======================================================================
//================= Configuração do SImulador ==========================
//======================================================================
    
    ConfigureSimulator();

	
//======================================================================
//================== Variáveis para estatísticas =======================
//======================================================================
    
    PrepareStatistics();
    
    vetorEst = (int *) calloc(workerTypeCount, sizeof(int));
    tipoThreadTarefa = (int **) calloc(workerTypeCount, sizeof(int *));
    for(i = 0; i < workerTypeCount; i++) tipoThreadTarefa[i] = (int *) calloc(taskTypeCount, sizeof(int));
    
    for(i = 0; i < workerTypeCount; i++) vetorEst[i] = 0;
    for(i = 0; i < workerTypeCount; i++) 
		for(j = 0; j < taskTypeCount; j++) tipoThreadTarefa[i][j] = 0;
		
    float estPercentage[workerTypeCount];
    int *somaColuna = (int *) calloc(taskTypeCount, sizeof(int));;
        
//======================================================================
//================= Carregando tarefas do file =========================
//======================================================================
    Ttask task;
    int contador = 0;
    
    task = getTask();
    while(task.taskType != EMPTY_ID){
        if(isClear(task)){
            PushTask(task);
        }else{
            pushToList(undone, task, 0);
        }
        contador++;
        task = getTask();  
    }

//======================================================================
//=============== Carregando da lista de Undone ========================
//======================================================================
    
    // Variável que determina quando os workers podem começar a trabalhar.
    task = pullfromList(undone, 0);
    
    while(task.taskType != EMPTY_ID){
        if(isClear(task)){
            PushTask(task);
        }else{
            pushToList(undone, task, 9999);    
        }
        task = pullfromList(undone, 0);
    }
    
//======================================================================
//==================== Finalizando Execução ============================
//======================================================================

    // Esperando todos trabalhadores finalizarem suas tarefas 
    while(countDone < numTasks);
    
    PushTask(createTask(0,TERMINATOR_ID,0,0,NULL,0,0,0));
    
    for(i=0; i < totalCount; i++){
        pthread_join(workerList[i]->thread,NULL);
    }
	
//======================================================================
//================= Imprimindo as estatísticas =========================
//======================================================================

    for (i = 0; i < workerTypeCount; ++i){
        estPercentage[i] = (float)(vetorEst[i]*100)/countDone;
        fprintf(printEstastisticasPizza, "%.2f\n", estPercentage[i]);
    }

    
    for(i = 0; i < taskTypeCount; ++i){
        for(j = 0; j < workerTypeCount; ++j){
            somaColuna[i] += tipoThreadTarefa[j][i];
        }
    }

    for (i = 0; i < workerTypeCount; ++i){
        for (j = 0; j < taskTypeCount; ++j){
            if(somaColuna[j] > 0) fprintf(printEstastisticasBarras, "%f ", (float) tipoThreadTarefa[i][j] * 100 / somaColuna[j]);
            else fprintf(printEstastisticasBarras, "0.0");
        }
        fprintf(printEstastisticasBarras, "\n");
    }
    
    gettimeofday(&end,0);
    total_execution_time = (end.tv_sec + end.tv_usec/1000000.0) - (start.tv_sec + start.tv_usec/1000000.0);
    printf("Tempo gasto em segundos %lf\n",total_execution_time);
    fprintf(total_time,"%lf",total_execution_time);

    printf("CONTADOR == %d\n", contador);
    
    fprintf(printTemposThreads, "\nFIM\n\n");
    fclose(printEstastisticasPizza);
    fclose(printEstastisticasBarras);
    fclose(printTemposThreads);
    fclose(f);
    fclose(tasksFile);
    fclose(total_time);
    return 0;

}
