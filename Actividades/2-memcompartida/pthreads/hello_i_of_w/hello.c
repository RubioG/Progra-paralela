#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

typedef struct info_thread
{
        size_t index;
        size_t size;
        sem_t mutex;
} info_thread;

void* hello(void* data)
{
        info_thread * info = (info_thread*)(data);
        printf("Hello from thread %zu of %zu\n", info->index, info->size);
	return NULL;
}

int main(int argc, char* argv[])
{

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
            pthread_create( &threads[index], NULL, hello, (void*)vector[index] );
        }

        printf("Hello from main threads\n");

        for(size_t index = 0; index < cant_Threads ; ++index){
            pthread_join(threads[index], NULL);
        }

        for(size_t index = 0; index < cant_Threads ; ++index){
            free(vector[index]);
        }

	return 0;
}
