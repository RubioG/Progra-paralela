#include <pthread.h>
#include <stdio.h>

void* hello(void* data)
{
	size_t thread_number = (size_t)data;
	printf("Hello from thread %zu\n", thread_number);
	return NULL;
}

int main(void)
{
	pthread_t thread;
	pthread_create( &thread, NULL, hello, (void*)1 );
	printf("Hello from main thread\n");
	pthread_join(thread, NULL);
	return 0;
}
