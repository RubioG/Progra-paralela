#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
  int papa = 0;
  size_t start_process;
  MPI_Init(&argc, &argv);
  int my_rank = -1;
  int process_count = -1; //o world_size
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &process_count);
  if(argc >= 3){
      papa = strtoull(argv[1],NULL,10);
      start_process = strtoull(argv[2],NULL,10);
  }
  else{
      //printf("Soy %i y estoy esperando \n", my_rank);
      if(my_rank==0){
    	  scanf("%u", &papa);
    	  scanf("%zu", &start_process);
    	  for ( int source = 1; source < process_count; ++source ){
    	      MPI_Send(&papa, 1, MPI_INT,source, 0, MPI_COMM_WORLD);
    	      MPI_Send(&start_process, 1, MPI_INT,source, 0, MPI_COMM_WORLD);
    	  }
	    }
      else{
    	  MPI_Recv(&papa, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    	  MPI_Recv(&start_process, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	    }
  }
  //printf("Soy %i y logre pasar \n", my_rank);
  char hostname[MPI_MAX_PROCESSOR_NAME]; //buffer
  int len_hostname = -1;
  MPI_Get_processor_name(hostname, &len_hostname);

  int next = ((my_rank+1)%process_count);
  int prev = ((my_rank-1)%process_count);
  if(my_rank == 0)
    prev = process_count-1;

  int initial_papa = papa;
  int exploded = 0;
  int players = process_count - 1;

  if((size_t)my_rank == start_process){
      if(papa % 2 == 0)
	       papa /= 2;
      else
	       papa = 3 * papa + 1;

      MPI_Send( &papa, 1, MPI_INT, next , 0, MPI_COMM_WORLD );
      MPI_Send( &players, 1, MPI_INT, next , 0, MPI_COMM_WORLD );
      if(papa == 1 || papa == 0){
    	  exploded = 1;
    	  papa = initial_papa;
    	  --players;
	    }
  }

  do{
      MPI_Recv( &papa, 1, MPI_INT, prev , MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Recv( &players, 1, MPI_INT, prev , MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      if(exploded != 0){
        if ( papa % 2 == 0 ){
          papa /= 2;
        }
	      else{
          papa = 3 * papa + 1;
        }
      }
      if( papa == 1 && !exploded ){
    	  exploded = 1;
    	  papa = initial_papa;
    	  --players;
	    }
      if(exploded){
    	  MPI_Send( &papa, 1, MPI_INT, next , 0, MPI_COMM_WORLD );
    	  MPI_Send( &players, 1, MPI_INT, next , 0, MPI_COMM_WORLD );
	    }
      else{
    	  MPI_Send( &papa, 1, MPI_INT, next , 0, MPI_COMM_WORLD );
    	  MPI_Send( &players, 1, MPI_INT, next , 0, MPI_COMM_WORLD );
	    }
  }while( papa > 0 && players > 0);

  if(exploded != 0)
    printf("El proceso %d en %s es el ganador.\n",my_rank,hostname);

  MPI_Finalize();
  return 0;
}
