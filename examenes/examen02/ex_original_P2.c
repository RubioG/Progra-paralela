#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <unistd.h>

int main(int argc, char* argv[])
{

	MPI_Init(&argc, &argv);

	int my_rank = -1;
	int process_count = -1;
	string texto = "";
	string * vector = NULL;
	size_t size_vector = -1;
	size_t max_chars = -1;

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);

	size_vector = strtoull(argv[1], NULL, 10);
	max_chars = strtoull(argv[2], NULL, 10);

	if(my_rank == 0){
		scanf("%s", &texto);
		vector = getVector(texto, size_vector);
		for(int source = 1; source < process_count ; ++source){
			MPI_Send(vector, size_vector, MPI_STRING, source, 0, MPI_COMM_WORLD);
		}
	}
	else{
		MPI_Recv(vector, size_vector, MPI_STRING, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}

	size_t my_a = ( my_rank * (size_vector/process_count) + MIN( (size_t)my_rank, (size_t)(size_vector % process_count)));
	size_t my_b = ( (my_rank+1) * (size_vector/process_count) + MIN( (size_t)(my_rank+1), (size_t)(size_vector % process_count)));

	for(size_t line = my_a; line < my_b; ++line){
		string pivote = vector[line];
		#pragma omp parallel for num_threads(sysconf(_SC_NPROCESSORS_ONLN)) default(none) shared(vector, pivote, size_vector) schedule(dynamic)
		for(size_t index = 0; index < size_vector ; ++index){
			string temp = vector[index];
			size_t distance = levenshtein((const)&pivote.toArray() , (const)&temp.toArray());
			printf("%zu", distance);
		}
		prinft(" \n");
	}
	MPI_Finalize();
	return 0;
}

string * getVector( string texto, size_t size_vector){
	string * vector = malloc(size_vector);
	size_t contador = 0;
	for(size_t index = 0; index < size_vector; ++index){
		char actual = 'a';
		while(actual != ','  || actual != '/0'){
			vector[index] = (texto[contador] + " ");
			++contador;
		}
	}

	return vector;
}

size_t MIN(size_t a, size_t b){
	if(a < b){
		return a;
	}
	return b;
}

/*
		Me equivoque muy gravemente con lo de string y el scanf, ademas pense que habia
		que implementar la solucion para levenshtein a lo que dedique tiempo valioso del
		examen, debido a que no es parte de la solucion no lo voy a agregar al archivo.
		Pido disculpas por las fallas cometidas.
*/
