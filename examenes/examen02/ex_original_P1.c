#include <mpi.h>
#include <stdio.h>
#include <string.h>
#define VOCAL 1
#define CONSO 0

int main(int argc, char* argv[])
{

	MPI_Init(&argc, &argv);

	int my_rank = -1;
	int process_count = -1;

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);

	size_t prob_dist_vocal = strtoull(argv[1],NULL,10);
	size_t prob_dist_consonante = strtoull(argv[2],NULL,10);
	size_t cant_repe = 1;
	if(argc >= 4){
		cant_repe = strtoull(argv[3],NULL,10);
	}

	string mensaje = "";
	int next = ((my_rank + 1) % process_count);
	int prev = ((my_rank + 1) % process_count);

	if(my_rank == 0){
		scanf("%s", &mensaje);
	}

	int actual_process = 0;
	size_t prob_unchange = 1 - (prob_dist_vocal+prob_dist_consonante);

	while(cant_repe > 0){
		MPI_Recv(&mensaje, 1, MPI_STRING, prev, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&actual_process, 1, MPI_INT, prev, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		if(my_rank == actual_process){
			srand(time(NULL));
			int rand_prob = (rand() % 10) + 1;
			if( (prob_unchange*10) < rand_prob ){
				rand_prob = (prob_unchange*10);
				if( prob_dist_vocal < prob_dist_consonante){
					if( rand_prob < prob_dist_vocal ){
						change_char(&mensaje, VOCAL);
					}
					else{
						change_char(&mensaje, CONSO);
					}
				}
				else{
					if(rand_prob < prob_dist_consonante){
						change_char(&mensaje, CONSO);
					}
					else{
						change_char(&mensaje, VOCAL);
					}
				}
			}
		}
		if(my_rank == 0){
			printf("%s", mensaje);
			--cant_repe;
		}
		MPI_Send(&actual_process, 1, MPI_INT, (my_rank + 1), 0, MPI_COMM_WORLD);
	}

	MPI_Finalize();
	return 0;
}

void change_char( string * mensaje, int tipo){
	int rand_index = -1;
	size_t size = strlen(*mensaje);
	size_t index = rand()%size;
	if(isalpha(mensaje->at(index))){
		if(tipo == VOCAL){
			if(is_vocal(mensaje->at(index))){
				srand(time(NULL));
				rand_index = rand() % 5;
				char temp = vocales[rand_index];
				swap(&temp , &(mensaje->at(index)));
			}
		}
		else{
			if(!is_vocal((mensaje->at(index)))){
				bool not_conso = true;
				while(not_conso){
					srand(time(NULL));
					rand_index = rand() % 255;
					if(isalpha(rand_index)){
						if(!is_vocal(rand_index)){
							not_conso = false;
						}
					}
				}
				char temp = rand_index;
				swap(&temp, index);
			}
		}
	}
}

bool is_vocal(char letra){
	char[5] vocales = { 'a', 'e', 'i', 'o', 'u'};
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
