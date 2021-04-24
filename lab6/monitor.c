#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include "logger.h"

// Resources
int available_resources = 64;
int shared_resources = 0;

pthread_cond_t condition;
pthread_mutex_t mutex;

void delay(int number_of_seconds) 
{ 
    // Converting time into milli_seconds 
    int milli_seconds = 1000 * number_of_seconds; 
    // Stroing start time 
    clock_t start_time = clock();   
    // looping till required time is not acheived 
    while (clock() < start_time + milli_seconds); 
}; 


// TAKEN FROM THE LAB EXAMPLE
/* decrease available resources by resources_taken */
int decrease_count(int resources_taken, void *custom_logger)
{
    fprintf(((struct logger *)custom_logger)->info, "Iniciando decrease_count\n");
    fprintf(((struct logger *)custom_logger)->info, "Recursos suficientes disponibles, consumiendo...\n");
    // Decrease resources
    available_resources -= resources_taken;
    delay(200);
    fprintf(((struct logger *)custom_logger)->info, "Terminando decrease_count\n");
    pthread_mutex_unlock(&mutex);
}

/* increase available resources by resources_taken */
int increase_count(int resources_taken, void *custom_logger)
{
    fprintf(((struct logger *)custom_logger)->info, "Iniciando increase_count\n");
    pthread_mutex_lock(&mutex);
    if (available_resources <= resources_taken)
        fprintf(((struct logger *)custom_logger)->info, "Sí hay! Recursos actuales: %d\n", available_resources);
    pthread_cond_signal(&condition);

    // Increase resources
    available_resources += resources_taken;
    fprintf(((struct logger *)custom_logger)->info, "Terminando increase_count\n");
    return 0;
}

void *resource_manager(void *custom_logger)
{
    // Resources taken...
    int resources_taken = 16;
    int tid = gettid();

    fprintf(((struct logger *)custom_logger)->info, "Iniciando thread %d\n", tid);

    for (int i = 0; i < ITER_COUNT; i++)
    {
        fprintf(((struct logger *)custom_logger)->info, "Iniciando iteracion %d\n", i + 1);
        fprintf(((struct logger *)custom_logger)->info, "Se consumiran %d recursos\n", resources_taken);

        // We need to lock our mutex to perform this two operations
        pthread_mutex_lock(&mutex);

        shared_resources++;
        fprintf(((struct logger *)custom_logger)->info, "%d - (!) Recurso tomado\n", tid);
        fprintf(((struct logger *)custom_logger)->info, "%d - Buenos dias! Recurso usado\n", tid);

        // Decrease & increase operation
        fprintf(((struct logger *)custom_logger)->info, "Mutex adquirido, entrando al monitor\n");
        decrease_count(resources_taken, (void *)((struct logger *)custom_logger));
        increase_count(resources_taken, (void *)((struct logger *)custom_logger));

        // Finally, we unlock our mutex
        pthread_mutex_unlock(&mutex);
        fprintf(((struct logger *)custom_logger)->info, "Mutex liberado\n");
    }
    pthread_exit(0);
}

int main(void)
{
    // Log file
    FILE *file;
    file = fopen("monitor_log.txt", "wt");
    struct logger *log_file = (struct logger *)malloc(sizeof(struct logger));
    log_file->info = file;

    printf("LOG - Generating monitor_log.txt...\n");

    fprintf(file, "Iniciando programa\n");
    fprintf(file, "Creando threads\n");

    // Pthread creation
    pthread_t thread[THREAD_COUNT];

    for (int i = 0; i < THREAD_COUNT; i++)
        pthread_create(&thread[i], NULL, *resource_manager, (void *)log_file);

    for (int j = 0; j < THREAD_COUNT; j++)
        pthread_join(thread[j], NULL);

    fclose(file);
    return 0;
}
