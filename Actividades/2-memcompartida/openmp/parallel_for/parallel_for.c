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
  size_t cant_Threads = 0;
  if(argc >= 2){
      cant_Threads = atoi(argv[1]);
      #pragma omp parallel for num_threads(cant_Threads) \
        default(none) shared (cant_Threads)
      for(int index = 0; index < cant_Threads ; ++index){
        hello();
      }
  }
  else{
      /* Ya que hay dos variables en el for, si dejo el default(none)
      no me permite compilar, interpreta 2 variables en la region paralelizada*/
      //#pragma omp parallel for num_threads(cant_Threads) //default(none)
      #pragma omp parallel for num_threads(cant_Threads) \
        default(none) shared (cant_Threads)
      for(int index = 0; index < cant_Threads ; ++index){
        hello();
      }
  }
	printf("Hello from main thread\n");
	return 0;
}
