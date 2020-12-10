#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//#include "pthread_barrier.h"

static size_t worker_count = 0;
static size_t number_count = 0;
static size_t* numbers = NULL;
static size_t current_step = 0;
static size_t max_steps = 10;
static pthread_t* workers = NULL;
static pthread_barrier_t barrier;

typedef struct
{
  size_t actual;
  size_t cant_threads;
}pinfo_t;

int check_ones_in_numbers(size_t start, size_t end) //check if still exist 1 in array numbers. return 1 if true, 0 if no.
{
  int result = 0;
  for(size_t index = start; index < end; ++index){
    if(numbers[index] != 1)
       result = 1;

  }
  return result;
}

void* calculate(void* data)
{
  pinfo_t* info = (pinfo_t*)data;

  size_t div = number_count / worker_count;
  size_t start = info->actual * div;
  size_t end = (info->actual+1) * div;
  if(info->actual+1 == info->cant_threads)
    end = number_count;

  size_t my_id = 0;
  size_t next = 0;
  size_t prev = 0;

  size_t* numbers_temp = (size_t*)malloc(end * sizeof(size_t)); // arreglo para numeros temporales.

  while ( current_step <= max_steps && check_ones_in_numbers(0,number_count)){
			for(size_t index = start; index < end; ++index){
				  my_id = index;
				  next = (my_id + 1) % number_count;
				  prev = (size_t)((long long)my_id - 1) % number_count; // hay problema con este prev,lo calcula mal.
				  if(my_id == 0)
				    prev = number_count-1;

				  numbers_temp[index] = numbers[index];

				  if(check_ones_in_numbers(my_id,my_id+1)){
	      			if ( numbers_temp[my_id]%2 == 0 )
									numbers_temp[my_id] /= 2;
	      			else
									numbers_temp[my_id] = numbers[prev] * numbers[my_id] + numbers[next];
	    		}
			}

      pthread_barrier_wait(&barrier); //se espera que todos terminen de calcular
  		for(size_t index = start; index < end; ++index)
					numbers[index] = numbers_temp[index];

  		if ( info->actual == 0 )
					++current_step;

  		pthread_barrier_wait(&barrier); //para que no empiecen a calcular si no han terminado de actualizar

  }
  free(numbers_temp);
  return NULL;
}

int main(int argc, char* argv[])
{
    printf("\nDigite la cantidad de numeros: ");
    scanf("%zu", &number_count);
    printf("\nDigite la cantidad de threads: ");
    scanf("%zu", &worker_count);

    numbers = malloc( number_count * sizeof(size_t) );
    printf("\nA continuacion digite los %zu numeros\n", number_count);
    for ( size_t index = 0; index < number_count; ++index )
      scanf("%zu", &numbers[index]);

    pthread_barrier_init(&barrier, NULL, (unsigned)worker_count);

    pinfo_t* info = (pinfo_t*) malloc(worker_count * sizeof(pinfo_t));
    workers = malloc(worker_count * sizeof(pthread_t));

    for ( size_t index = 0; index < worker_count; ++index )
      {
				info[index].actual = index;
				info[index].cant_threads = worker_count;
				pthread_create(&workers[index], NULL, calculate, (void*)&info[index]);
      }

    for ( size_t index = 0; index < worker_count; ++index )
      	pthread_join(workers[index], NULL);

    pthread_barrier_destroy(&barrier);

    free(numbers);
    free(workers);
    free(info);

    if ( current_step > max_steps )
      printf("No converge in %zu steps\n", max_steps);
    else
      printf("Converged in %zu steps\n", current_step);

  return 0;
}
