#include <mpi.h>
#include <stdio.h>

#define LEN_MESSAGE 1024

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

	char message[LEN_MESSAGE];

	if ( my_rank == 0 )
	{
		// Process 0 collects messages from other processes in their rank order
		for ( int source = 1; source < process_count; ++source )
		{
			MPI_Recv(message, LEN_MESSAGE, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			printf("Process %d says: %s\n", source, message);
		}
	}
	else
	{
		// Other process send a hello-message to process 0
		sprintf(message, "Hello from process %d of %d on %s", my_rank, process_count, hostname);
		MPI_Send(message, LEN_MESSAGE, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
	}

	MPI_Finalize();
	return 0;
}
