#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>

size_t MIN(size_t a,size_t b){
  return (a < b) ? (a):(b);
}

int is_prime(size_t number){
  if(number < 2) return 0;

  if(number == 2) return 1;

  if(number % 2 == 0) return 0;

  for(size_t i = 3, last = sqrt(number); i <= last ; i +=2)
    if(number % i == 0)
      return 0;
  return 1;
}

int main(int argc, char* argv[])
{
  size_t a = 0;
  size_t b = 0;
  MPI_Init(&argc, &argv);
  int my_rank = -1;
  int process_count = -1; //o world_size
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &process_count);
  if(argc >= 3){
    a = strtoull(argv[1],NULL,10);
    b = strtoull(argv[2],NULL,10);
  }else{
    if(my_rank==0){
      scanf("%zu", &a);
      scanf("%zu", &b);
      for ( int source = 1; source < process_count; ++source ){
      	MPI_Send(&a, 1, MPI_UNSIGNED_LONG_LONG,source, 0, MPI_COMM_WORLD);
      	MPI_Send(&b, 1, MPI_UNSIGNED_LONG_LONG,source, 0, MPI_COMM_WORLD);
      }
    }else{
      MPI_Recv(&a, 1, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      MPI_Recv(&b, 1, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    }
  }

  size_t total_range = b-a;
  size_t my_a = (a) + (my_rank * (total_range/process_count)) + MIN((size_t)my_rank,(size_t)total_range % process_count);
  size_t my_b = (a) + ((my_rank+1) * (total_range/process_count)) + MIN((size_t)(my_rank+1),(size_t)total_range % process_count);;

  char hostname[MPI_MAX_PROCESSOR_NAME]; //buffer
  int len_hostname = -1;
  MPI_Get_processor_name(hostname, &len_hostname);

  double start = 0.0;
  double end = 0.0;
  size_t primes_found = 0;
  MPI_Barrier( MPI_COMM_WORLD );

  start = MPI_Wtime();

  for(size_t start = my_a; start < my_b; ++start){
    if(is_prime(start)){
      primes_found += 1;
    }
  }

  end = MPI_Wtime();

  double time = end-start;
  size_t temp_data = 0;
  double temp_time = 0;
  double total_time = time;

  if ( my_rank == 0 ){

    for ( int source = 1; source < process_count; ++source ){
      MPI_Recv(&temp_data, 1, MPI_UNSIGNED_LONG_LONG, source, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      primes_found += temp_data;
      MPI_Recv(&temp_time, 1, MPI_DOUBLE, source, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      if(temp_time > total_time)
	       total_time = temp_time;
    }
  }else{
    MPI_Send(&primes_found, 1, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD);
    MPI_Send(&time, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
  }

  if(my_rank == 0){
    printf("%ld primes found in range [%ld, %ld[ in %f s with %d processes\n",primes_found,a,b,time,process_count);
  }

  MPI_Finalize();
  return 0;
}
