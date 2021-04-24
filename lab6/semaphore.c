#include <unistd.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include "logger.h"

// Semaphore
sem_t sem;
int shared_resources = 0;

void *resource_manager(void *custom_logger)
{
    int tid = gettid();
    fprintf(((struct logger *)custom_logger)->info, "Iniciando thread %d\n", tid);
    fprintf(((struct logger *)custom_logger)->info, "Esperando threads\n");
    fprintf(((struct logger *)custom_logger)->info, "%d - Semaforo abierto con exito\n", tid);

    for (int i = 0; i < ITER_COUNT; i++)
    {
        fprintf(((struct logger *)custom_logger)->info, "Iniciando iteracion %d\n", i + 1);
        sem_wait(&sem);

        shared_resources++;
        fprintf(((struct logger *)custom_logger)->info, "%d - (!) Recurso tomado\n", tid);
        fprintf(((struct logger *)custom_logger)->info, "%d - Buenos dias! Recurso usado\n", tid);

        sem_post(&sem);
        sleep(0.5);
        fprintf(((struct logger *)custom_logger)->info, "%d - Recurso devuelto :)\n", tid);
    }

    fprintf(((struct logger *)custom_logger)->info, "%d - Semaforo muerto\n", tid);
    fprintf(((struct logger *)custom_logger)->info, "Terminando thread %d\n", tid);
    pthread_exit(0);
}

int main(void)
{
    // Log file
    FILE *file;
    file = fopen("semaphore_log.txt", "wt");
    struct logger *log_file = (struct logger *)malloc(sizeof(struct logger));
    log_file->info = file;

    printf("LOG - Generating log_semaphore.txt ...\n");
    fprintf(file, "Iniciando programa\n");
    fprintf(file, "Creando threads\n");

    pthread_t thread[THREAD_COUNT];
    sem_init(&sem, 0, 1);

    for (int i = 0; i < THREAD_COUNT; i++)
        pthread_create(&thread[i], NULL, *resource_manager, (void *)log_file);

    for (int j = 0; j < THREAD_COUNT; j++)
        pthread_join(thread[j], NULL);

    fclose(file);
    return 0;
}
