#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <unistd.h>

#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

//tomada de https://en.wikibooks.org/wiki/Algorithm_Implementation/chars/Levenshtein_distance#C para efectos de una sol serial
long levenshtein(const char *text1, const char *text2) {
    long x, y, text1_len, text2_len;
    text1_len = strlen(text1);
    text2_len = strlen(text2);
    long matrix[text2_len+1][text1_len+1];
    matrix[0][0] = 0;
    for (x = 1; x <= text2_len; x++)
        matrix[x][0] = matrix[x-1][0] + 1;
    for (y = 1; y <= text1_len; y++)
        matrix[0][y] = matrix[0][y-1] + 1;
    for (x = 1; x <= text2_len; x++)
        for (y = 1; y <= text1_len; y++)
            matrix[x][y] = MIN3(matrix[x-1][y] + 1, matrix[x][y-1] + 1, matrix[x-1][y-1] + (text1[y-1] == text2[x-1] ? 0 : 1));

    return(matrix[text2_len][text1_len]);
}

char * getVector( char * texto, size_t size_vector){
	char ** vector = malloc(size_vector);
	size_t contador = 0;
	for(size_t index = 0; index < size_vector; ++index){
		char actual = 'a';
		while(actual != ','  || actual != '/0'){
			vector[index] = texto[contador];
			++contador;
		}
	}

	return vector;
}

int main(int argc, char* argv[])
{

	MPI_Init(&argc, &argv);

	int my_rank = -1;
	int process_count = -1;
	char * texto = NULL;
	char ** vector = NULL;
	size_t size_vector = -1;
	size_t max_chars = -1;

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);

	size_vector = strtoull(argv[1], NULL, 10);
	max_chars = strtoull(argv[2], NULL, 10);

	*vector = malloc(size_vector);
	for(size_t index = 0; index < size_vector : ++index){
		**vector = malloc(max_chars);
	}

	if(my_rank == 0){
		scanf("%s", texto);
		vector = getVector(texto, size_vector);
		for(int source = 1; source < process_count ; ++source){
			MPI_Send(vector, size_vector, MPI_char, source, 0, MPI_COMM_WORLD);
		}
	}
	else{
		MPI_Recv(vector, size_vector, MPI_char, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}

	size_t my_a = ( my_rank * (size_vector/process_count) + MIN( (size_t)my_rank, (size_t)(size_vector % process_count)));
	size_t my_b = ( (my_rank+1) * (size_vector/process_count) + MIN( (size_t)(my_rank+1), (size_t)(size_vector % process_count)));

	for(size_t line = my_a; line < my_b; ++line){
		char pivote = vector[line];
		#pragma omp parallel for num_threads(sysconf(_SC_NPROCESSORS_ONLN)) default(none) shared(vector, pivote, size_vector) schedule(dynamic)
		for(size_t index = 0; index < size_vector ; ++index){
			char temp = vector[index];
			size_t distance = levenshtein((const)&pivote.toArray() , (const)&temp.toArray());
			printf("%zu", distance);
		}
		prinft(" \n");
	}
	MPI_Finalize();
	return 0;
}
