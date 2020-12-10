#include <mpi.h>			//para openmpi
#include <stdio.h>		//para la entrada
#include <string.h>		//para strlen
#include <stdlib.h>     /* srand, rand */
#include <time.h>			//para usar time
#include <stdbool.h>  //para usar bool
#include <ctype.h>		//para is alpha
#define VOCAL 1
#define CONSO 0

bool is_vocal(char letra){
	char vocales[5] = { 'a', 'e', 'i', 'o', 'u'};
	for(int index = 0; index < 5; ++index){
		if(vocales[index] == letra){
			return true;
		}
	}
	return false;
}

void swap(char * a, char * b){
	char temp = *a;
	*a = *b;
	*b = temp;
}


void change_char( char * mensaje, int tipo){
	char vocales[5] = { 'a', 'e', 'i', 'o', 'u'};
	int rand_index = -1;
	size_t size = strlen((const char *)mensaje);
	bool flag_correct = true;
	size_t index = -1;

	while(flag_correct){
		srand(time(NULL));
		index = rand()%size;
		if(isalpha(mensaje[index])){
			if(tipo == VOCAL){
				if(is_vocal(mensaje[index])){
					flag_correct = false;
				}
			}
			else{
				if(!is_vocal(mensaje[index])){
					flag_correct = false;
				}
			}
		}
	}

	if( isalpha( mensaje[index]) ){
		if(tipo == VOCAL){
			if(is_vocal( mensaje[index] )) {
				srand(time(NULL));
				rand_index = rand() % 5;
				char temp = vocales[rand_index];
				swap(&temp , &mensaje[index]);
			}
		}
		else{
			if(!is_vocal( mensaje[index] )){
				flag_correct = true;
				while(flag_correct){
					srand(time(NULL));
					rand_index = rand() % 122;
					if(isalpha(rand_index)){
						if(!is_vocal(rand_index)){
							flag_correct = false;
						}
					}
				}
				char temp = rand_index;
				swap(&temp, &mensaje[index]);
			}
		}
	}
}

int main(int argc, char* argv[])
{

	MPI_Init(&argc, &argv);

	int my_rank = -1;
	int process_count = -1;

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);

	double prob_dist_vocal = strtod(argv[1],NULL);
	double prob_dist_consonante = strtod(argv[1],NULL);
	size_t cant_repe = 1;

	if(argc >= 4){
		cant_repe = strtoull(argv[3],NULL,10);
	}

	char * mensaje = calloc(1024*10,sizeof(char));
	int next = (my_rank + 1) % process_count;
	int prev = (my_rank - 1) % process_count;
	size_t size_message = -1;

	if(my_rank == 0){
		int size = (1024*10*sizeof(char));
		while(!feof(stdin)){
	    	fgets (&mensaje[strlen(mensaje)] , size, stdin);
	  }
		size_message = strlen((const char *)mensaje)+1;
	  prev = process_count -1;
		MPI_Send(&cant_repe, 1, MPI_UNSIGNED_LONG_LONG, next, 0, MPI_COMM_WORLD);
		MPI_Send(&size_message, 1, MPI_UNSIGNED_LONG_LONG, next, 0, MPI_COMM_WORLD);
		MPI_Send(mensaje, size_message , MPI_CHAR, next, 0, MPI_COMM_WORLD);
	}

	double prob_unchange = 1 - (prob_dist_vocal+prob_dist_consonante);
	while(cant_repe > 0){
		//printf("Soy el proceso %i y mi mensaje es %s con un tamaño de %zu, tengo como prev: %i y como next: %i\n", my_rank,mensaje,size_message, prev, next);
		MPI_Recv(&cant_repe, 1, MPI_UNSIGNED_LONG_LONG, prev, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&size_message, 1, MPI_UNSIGNED_LONG_LONG, prev, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(mensaje, size_message, MPI_CHAR, prev, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		srand(time(NULL));
		int rand_prob = (rand() % 10) + 1;

		if( (prob_unchange*10) < rand_prob ){
			rand_prob -= (prob_unchange*10);
			if( prob_dist_vocal < prob_dist_consonante){
				if( rand_prob < (prob_dist_vocal*10) ){
					change_char(mensaje, VOCAL);
				}
				else{
					change_char(mensaje, CONSO);
				}
			}
			else{
				if(rand_prob < (prob_dist_consonante*10)){
					change_char(mensaje, CONSO);
				}
				else{
					change_char(mensaje, VOCAL);
				}
			}
		}

		if(my_rank == 0){
			printf("\n%s\n", mensaje);
			--cant_repe;
		}

		size_message = strlen((const char *)mensaje)+1;
		MPI_Send(&cant_repe, 1, MPI_UNSIGNED_LONG_LONG, next, my_rank, MPI_COMM_WORLD);
		MPI_Send(&size_message, 1, MPI_UNSIGNED_LONG_LONG, next, my_rank, MPI_COMM_WORLD);
		MPI_Send(mensaje, size_message , MPI_CHAR, next, my_rank, MPI_COMM_WORLD);
	}

	MPI_Finalize();
	return 0;
}
