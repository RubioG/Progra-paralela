#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>


size_t MIN(size_t a,size_t b){
  return (a < b) ? (a):(b);
}

bool is_prime(size_t number)
{
	if ( number < 2 ) return false;
	if ( number == 2 ) return true;
	if ( number % 2 == 0 ) return false;

	for ( size_t i = 3, last = (size_t)(double)sqrt(number); i <= last; i += 2 )
		if ( number % i == 0 )
			return false;

	return true;
}

size_t count_primes(size_t my_start, size_t my_finish)
{
	size_t prime_count = 0;
	size_t current;
	for ( current = my_start; current <= my_finish; ++current )
		if ( is_prime(current) )
			++prime_count;
	return prime_count;
}


int main(int argc, char* argv[])
{
	double start_time = 0.0;
	double end_time = 0.0;

	MPI_Init(&argc, &argv);
	start_time = MPI_Wtime();

	size_t a = 0;
	size_t b = 0;

	int my_rank = -1;
	int process_count = -1;

	char hostname[MPI_MAX_PROCESSOR_NAME]; // forma de ir a la segura
	int len_hostname = -1;

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);
	MPI_Get_processor_name(hostname, &len_hostname);

	if( argc >= 2){
		a = strtoull(argv[1], NULL, 10);
		b = strtoull(argv[2], NULL, 10);
	}
	else if( my_rank == 0){
		scanf("%zu", &a);
		scanf("%zu", &b);
	}
	MPI_Bcast(&a, 1, MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD);
	MPI_Bcast(&b, 1, MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD);

  size_t my_start = (a) + (my_rank * ((b-a)/process_count)) + MIN((size_t)my_rank,(size_t)(b-a) % process_count);
  size_t my_finish = (a) + ((my_rank+1) * ((b-a)/process_count)) + MIN((size_t)(my_rank+1),(size_t)(b-a) % process_count);;

	size_t number_primes = 0;

  number_primes = count_primes(my_start, my_finish);
	size_t total_primes = number_primes;
	MPI_Reduce(&number_primes, &total_primes, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

  if( my_rank == 0){
	end_time = MPI_Wtime();
	printf("%zu primes found in range [%zu, %zu[ in %lfs with %d processes\n", total_primes, a, b, end_time-start_time, process_count);
  }

	MPI_Finalize();
	return 0;
}
