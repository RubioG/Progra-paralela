#include <stdio.h>
#include <omp.h>
#include <stdlib.h>

void hello(void)
{
  int thread_number = omp_get_thread_num();
	int thread_count = omp_get_num_threads();
	printf("Hello from thread %d of %d\n", thread_number, thread_count);
}

int main(int argc, char* argv[])
{
  size_t cant_threads = 0;
  if(argc >= 2){
      cant_threads = atoi(argv[1]);
			#pragma omp parallel num_threads (cant_threads)
      hello();
  }
  else{
      #pragma omp parallel
      hello();
  }
	printf("Hello from main thread\n");
	return 0;
}
