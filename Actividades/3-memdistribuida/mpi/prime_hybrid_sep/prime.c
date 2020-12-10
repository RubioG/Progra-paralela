#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>

size_t min(size_t a, size_t b){
    return ((a) < (b) ?  (a) : (b) );
}

bool is_prime(size_t number)
{
    if ( number < 2 ) return false;

    for ( size_t index = 2, last = sqrt(number); index <= last; ++index )
        if ( number % index == 0 )
            return false;

    return true;
}

int main(int argc, char* argv[])
{
    size_t range_x = 0;
    size_t range_y = 0;

		MPI_Init(&argc, &argv);

		int my_rank = -1;
		int process_count = -1;
		size_t cant_threads = omp_get_max_threads();

		MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
		MPI_Comm_size(MPI_COMM_WORLD, &process_count);

    if(argc >= 3){
      range_x = strtoull(argv[1],NULL,10);
      range_y = strtoull(argv[2],NULL,10);
    }else{
        if(my_rank == 0){
            scanf("%zu",&range_x);
            scanf("%zu",&range_y);
            for(int index = 1; index < process_count; ++index){
              MPI_Send(&range_x, 1 , MPI_UNSIGNED_LONG_LONG, index, 0,  MPI_COMM_WORLD);
              MPI_Send(&range_y, 1 , MPI_UNSIGNED_LONG_LONG, index, 0,  MPI_COMM_WORLD);
            }
            //int MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest,int tag, MPI_Comm comm)
        }
        else{
          	MPI_Recv(&range_x, 1, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&range_y, 1, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            //int MPI_Recv(void *buf, int count, MPI_Datatype datatype,int source, int tag, MPI_Comm comm, MPI_Status *status)
        }
    }

		size_t range = range_y - range_x;
		size_t mod = range%process_count;
		size_t partition= range/process_count;

		size_t my_a = 0;
		size_t my_b = 0;


		char hostname[MPI_MAX_PROCESSOR_NAME];
		int len_hostname = -1;
		MPI_Get_processor_name(hostname, &len_hostname);
		my_a = (range_x + my_rank*partition+ min((size_t)my_rank, mod));
		my_b = (range_x + (my_rank+1)*partition+ min((size_t)my_rank+1 , mod));

    size_t cant_primes = 0;
    for(size_t number = my_a ; number < my_b ; ++number){
        if(is_prime(number))
            ++cant_primes;
    }

    printf("\tProcess %zu on %s found %zu primes in range [%zu,%zu[ in %zu s with %zu threads\n", my_rank ,hostname, cant_primes , my_a, my_b , 0, cant_threads);

		MPI_Finalize();
	  return 0;
}
