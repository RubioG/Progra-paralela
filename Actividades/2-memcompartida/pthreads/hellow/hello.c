#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

//static size_t next_to_print=1;
//static pthread_mutex_t lock;
//static sem_t mutex;

typedef struct info_thread
{
        size_t index;
        size_t size;
        sem_t mutex;
} info_thread;

void* hello(void* data)
{
        info_thread * info = (info_thread*)(data);
        //while(info->index != next_to_print); //espera activa
        //pthread_mutex_lock(&lock);
        sem_wait(&(info->mutex));
        printf("Hello from thread %zu of %zu\n", info->index, info->size);
        //++next_to_print;
        //pthread_mutex_unlock(&lock);
        sem_post(&(info->mutex));
	return NULL;
}

int main(int argc, char* argv[])
{
//        if (pthread_mutex_init(&lock, NULL) != 0)
//        {
//            printf("\n mutex init failed\n");
//            return 1;
//        }
        size_t cant_Threads;
        if(argc <= 2){
            cant_Threads = atoi(argv[1]);
        }
        else{
            cant_Threads = sysconf(_SC_NPROCESSORS_ONLN);
        }

        pthread_t threads[cant_Threads];
        info_thread * vector[cant_Threads];


        for(size_t index = 0; index < cant_Threads ; ++index){
            vector[index] = calloc(1,sizeof(info_thread));
            vector[index]->index = index+1;
            vector[index]->size = cant_Threads;
            sem_init(&(vector[index]->mutex), 0, 1);
            pthread_create( &threads[index], NULL, hello, (void*)vector[index] );
        }

        printf("Hello from main threads\n");

        for(size_t index = 0; index < cant_Threads ; ++index){
            pthread_join(threads[index], NULL);
        }

        //pthread_mutex_destroy(&lock);
        for(size_t index = 0; index < cant_Threads ; ++index){
            sem_destroy(&(vector[index]->mutex));
        }

	return 0;
}
