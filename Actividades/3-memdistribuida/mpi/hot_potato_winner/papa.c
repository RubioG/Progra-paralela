#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
  int papa = 0;
  MPI_Init(&argc, &argv);
  int my_rank = -1;
  int process_count = -1;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &process_count);
  if(argc >= 2){
      papa = strtoull(argv[1],NULL,10);
  }
  else{
      if(my_rank==0){
	       scanf("%u", &papa);
	       for ( int source = 1; source < process_count; ++source )
	           MPI_Send(&papa, 1, MPI_INT,source, 0, MPI_COMM_WORLD);
	    }
      else{
	       MPI_Recv(&papa, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	    }
  }

  char hostname[MPI_MAX_PROCESSOR_NAME];
  int len_hostname = -1;
  MPI_Get_processor_name(hostname, &len_hostname);

  int next = ((my_rank+1)%process_count);
  int prev = ((my_rank-1)%process_count);
  if(my_rank == 0)
    prev = process_count-1;

  int initial_papa = papa;
  int exploded = 0;
  int players = process_count - 1 ;

  if(my_rank == 0){
      --papa;
      MPI_Send( &papa, 1, MPI_INT, next , 0, MPI_COMM_WORLD );
      MPI_Send( &players, 1, MPI_INT, next , 0, MPI_COMM_WORLD );
      if(papa == 0 || papa == -1){
	       exploded = 1;
	       papa = initial_papa;
	       --players;
	    }
  }
  do{
      MPI_Recv( &papa, 1, MPI_INT, prev , MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Recv( &players, 1, MPI_INT, prev , MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      if(!exploded)
	       --papa;
      if( papa == 0 && !exploded ){
	       exploded = 1;
	       papa = initial_papa;
	       --players;
	    }
      if(exploded){// no hace cambios a la papa.
      	  MPI_Send( &papa, 1, MPI_INT, next , 0, MPI_COMM_WORLD );
      	  MPI_Send( &players, 1, MPI_INT, next , 0, MPI_COMM_WORLD );
	    }
      else{
      	  MPI_Send( &papa, 1, MPI_INT, next , 0, MPI_COMM_WORLD );
      	  MPI_Send( &players, 1, MPI_INT, next , 0, MPI_COMM_WORLD );
	    }
  }while( papa > 0 && players > 0);

  if(!exploded)
    printf("El proceso %d en %s es el ganador.\n",my_rank,hostname);

  MPI_Finalize();
  return 0;
}
