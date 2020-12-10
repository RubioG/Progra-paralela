#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>

typedef struct info_thread
{
        size_t index;
        size_t size;
        sem_t * mutex_vector;
} info_thread;

void* hello(void* data)
{
        info_thread * info = (info_thread*)(data);
        sem_wait(&(info->mutex_vector[info->index]));
        printf("Hello from thread %zu of %zu\n", info->index+1, info->size);
        if(info->index < (info->size - 1)){
          sem_post(&(info->mutex_vector[info->index+1]));
        }
	return NULL;
}

int main(int argc, char* argv[])
{
        time_t start = time(NULL);
        size_t cant_threads=sysconf(_SC_NPROCESSORS_ONLN);
        if(argc <= 2){
            cant_threads = atoi(argv[1]);
        }

        pthread_t threads[cant_threads];
        info_thread * vector_info[cant_threads];
        sem_t sem_vector[cant_threads];

        sem_init(&(sem_vector[0]), 0, 1);
        for(size_t index = 1; index < cant_threads ; ++index){
            sem_init(&(sem_vector[index]), 0, 0);
        }

        for(size_t index = 0; index < cant_threads ; ++index){
            vector_info[index] = calloc(1,sizeof(info_thread));
            vector_info[index]->index = index;
            vector_info[index]->size = cant_threads;
            vector_info[index]->mutex_vector = sem_vector;
            pthread_create( &threads[index], NULL, hello, (void*)vector_info[index] );
        }

        //printf("Hello from main threads\n");

        for(size_t index = 0; index < cant_threads ; ++index){
            pthread_join(threads[index], NULL);
        }

        for(size_t index = 0; index < cant_threads ; ++index){
            free(vector_info[index]);
            sem_destroy(&(sem_vector[index]));
        }

        time_t end = time(NULL);
        printf("Took %f seconds\n", difftime(end, start));

	return 0;
}
