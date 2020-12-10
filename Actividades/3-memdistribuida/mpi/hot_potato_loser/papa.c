#include <mpi.h>

int main(int argc, char* argv[])
{
  int papa = 0;
  MPI_Init(&argc, &argv);
  int my_rank = -1;
  int process_count = -1;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &process_count);
  if(argc >= 2){
    if(my_rank==0){
	     papa = strtoull(argv[1],NULL,10);
    }else{
	     papa = -1;
    }
  }else{
    if(my_rank==0){
       scanf("%zu", &papa);
     }
  }

  char hostname[MPI_MAX_PROCESSOR_NAME]; //buffer
  int len_hostname = -1;
  MPI_Get_processor_name(hostname, &len_hostname);

  int next = ((my_rank+1)%process_count);
  int prev = ((my_rank-1)%process_count);

  if(my_rank == 0){
      prev = process_count-1;
      --papa;
      MPI_Send( &papa, 1, MPI_INT, next , 0, MPI_COMM_WORLD );
      if(papa == 0 || papa == -1){
        printf("La papa ha explotado, el perdedor es : %d  en el host %s.\n",my_rank,hostname);
      }
  }
  do{

    MPI_Recv( &papa, 1, MPI_INT, prev , MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    --papa;
    if(papa == 0 ){
      printf("La papa ha explotado, el perdedor es : %d  en el host %s.\n",my_rank,hostname);
    }
    MPI_Send( &papa, 1, MPI_INT, next , 0, MPI_COMM_WORLD );

  }while( papa > 0);

  MPI_Finalize();
  return 0;
}
