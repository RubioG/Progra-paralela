#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <unistd.h>

size_t min(size_t a, size_t b){
    return ((a) < (b) ?  (a) : (b) );
}

int main(int argc, char* argv[])
{
		if(argc >= 3){
					MPI_Init(&argc, &argv);

					int my_rank = -1;
					int process_count = -1;
					size_t cant_threads = omp_get_max_threads();

					MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
					MPI_Comm_size(MPI_COMM_WORLD, &process_count);

					size_t range_x = atoi(argv[1]);
					size_t range_y = atoi(argv[2]);

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
					printf("%s:%zu: range [%zu,%zu[ size %zu\n", hostname, my_rank , my_a , my_b , my_b - my_a);
					size_t range1 = (my_b - my_a);
					#pragma omp parallel num_threads (cant_threads) default(none) shared(cant_threads,my_rank,range1, my_a, my_b, hostname)
					{
						int thread_number = omp_get_thread_num();
						size_t partition1 = range1/cant_threads;
						size_t mod1 = range1%cant_threads;
						size_t my_a1 = (my_a + thread_number*partition1 + min((size_t)thread_number, mod1));
						size_t my_b1 = (my_a + (thread_number+1)*partition1 + min((size_t)thread_number+1 , mod1));
						printf("\t%s:%zu:%i: range [%zu,%zu[ size %zu\n", hostname, my_rank,thread_number , my_a1, my_b1, (my_b1 - my_a1));
					}

					MPI_Finalize();
			}
	return 0;
}
