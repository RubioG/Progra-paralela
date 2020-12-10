#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <unistd.h>

int main(int argc, char* argv[])
{

	MPI_Init(&argc, &argv);

	int my_rank = -1;
	int process_count = -1;

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);

	char hostname[MPI_MAX_PROCESSOR_NAME];
	int len_hostname = -1;
	MPI_Get_processor_name(hostname, &len_hostname);

	printf("Hello from process %d of %d on %s\n", my_rank, process_count, hostname);

	size_t cant_threads = omp_get_max_threads();
	#pragma omp parallel num_threads (cant_threads)
	{
		int thread_number = omp_get_thread_num();
		int thread_count = omp_get_num_threads();
		printf("Hello from thread %d of %d on process %u\n", thread_number, thread_count, my_rank);
	}

	MPI_Finalize();
	return 0;
}
