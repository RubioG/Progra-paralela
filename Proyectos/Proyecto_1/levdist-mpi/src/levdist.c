#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#include "concurrency.h"
#include "dir.h"
#include "levdist.h"
#include "levenshtein.h"
#include <string.h>
#include <memory.h>
#include <linux/limits.h>

// Private functions:

/// Shows how to travese the list removing elements
void levdist_print_and_destroy_files(levdist_t* this);

/// Shows how to traverse the list without removing elements
void levdist_print_files(levdist_t* this);

size_t factorial(size_t n){
  if (n == 0)
    return 1;
  else
    return(n * factorial(n-1));
}

size_t MIN(size_t a,size_t b){
  return (a < b) ? (a):(b);
}

int comp_funct(const void * a,const void * b){
    info_vec* file1ptr = ((info_vec*)a);
    info_vec* file2ptr = ((info_vec*)b);
    if(file1ptr->distance == file2ptr->distance && (strcmp(file1ptr->name_file1,file2ptr->name_file1) == 0)){
        return (strcmp(file1ptr->name_file2,file2ptr->name_file2) > 0);
    }else if(file1ptr->distance == file2ptr->distance){
        return (strcmp(file1ptr->name_file1,file2ptr->name_file1) > 0);
    }else{
        return (file1ptr->distance > file2ptr->distance);
  	}
}


void levdist_init(levdist_t* this)
{
	arguments_init(&this->arguments);
	this->files = NULL;
}

int levdist_run(levdist_t* this, int argc, char* argv[])
{
	// Analyze the arguments given by the user
	this->arguments = arguments_analyze(argc, argv);

	// If arguments are incorrect, stop
	if ( this->arguments.error )
		return this->arguments.error;

	// If user asked for help or software version, print it and stop
	if ( this->arguments.help_asked )
		return arguments_print_usage();
	if ( this->arguments.version_asked )
		return arguments_print_version();

	// Arguments seems fine, process the directories
	return levdist_process_dirs(this, argc, argv);
}

int levdist_process_dirs(levdist_t* this, int argc, char* argv[])
{

		// Start counting the time
		walltime_t start;
		walltime_start(&start);
		// Load all files into a list
		this->files = queue_create();
		levdist_list_files_in_args(this, argc, argv);

		int my_rank = -1;
		int process_count = -1;

		MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
		MPI_Comm_size(MPI_COMM_WORLD, &process_count);

    if(!this->arguments.silent && !this->arguments.quiet && my_rank == 0){
        // Print filenames found
        levdist_print_files(this);
    }

		size_t size_vector = ( factorial(queue_count(this->files)) / 2*factorial( queue_count(this->files)-2 ));

		info_vec * vector_dist = (info_vec*) malloc(sizeof(info_vec)*(size_vector));

		size_t index_vec = 0;

		FILE * file1 = NULL;
    FILE * file2 = NULL;
    const char * name_file1 = NULL;
    const char * name_file2 = NULL;
    char * final_data = NULL;
    long size_file1 = 0;
    long size_file2 = 0;
    size_t distance=0;
    if(queue_count(this->files)==1){
        return fprintf(stderr, "levdist: error: at least two files are required to compare\n"),2;
    }

    for(queue_iterator_t index = queue_begin(this->files); index != queue_end(this->files) ; index=queue_next(index)){
        //Obtain name of the file
        name_file1 = (const char *) queue_data(index);
        //Open the file
        file1 = fopen ( name_file1 , "rb" );
        //Obtain size of file
        fseek(file1, 0, SEEK_END);
        size_file1 = ftell(file1);
        rewind(file1);
        //allocate memory to contain the whole file:
        char * final_file1 = (char*) malloc (sizeof(char)*(size_file1+1));
        //subtract the information from the file
        final_file1[size_file1]='\0';
        fread(final_file1, 1 ,size_file1+1, file1);
        size_file1=strlen((char*)queue_data(index));
        for(queue_iterator_t index1 = queue_next(index); index1 != queue_end(this->files) ; index1=queue_next(index1),++index_vec){
            //Obtain name of the file
            name_file2 = (const char *) queue_data(index1);
            if(strcmp(name_file1, name_file2) == 0){
                return fprintf(stderr, "levdist: error: at least two files are required to compare\n"),2;
            }
            //Open the file
            file2 = fopen ( name_file2 , "rb" );

            //Obtain size of file
            fseek(file2, 0, SEEK_END);
            size_file2 = ftell(file2);
            rewind(file2);
            // allocate memory to contain the whole file:
            char * final_file2 = (char*) malloc (sizeof(char)*(size_file2+1));
            //subtract the information from the file
            final_file2[size_file2]='\0';
            fread(final_file2, 1,size_file2+1, file2);

						info_vec info;
						info.distance = distance;
						info.name_file1 = name_file1;
						info.name_file2 = name_file2;
						info.file1 = final_file1;
						info.file2 = final_file2;
						vector_dist[index_vec] = info;

            fclose (file2);
        }
        fclose (file1);
    }

		free(file1);
		free(file2);

		size_t mod = size_vector%process_count;
		size_t partition= size_vector/process_count;

		size_t my_a = my_rank*partition + MIN((size_t)my_rank, mod);
		size_t my_b = (my_rank+1)*partition + MIN((size_t)my_rank+1, mod);

		for(size_t index  = my_a; index < my_b; ++index ){
			vector_dist[index].distance = distance_levdist( (unsigned char*)vector_dist[index].file1, (unsigned char*)vector_dist[index].file2, (this->arguments.workers*2));
		}

		size_t size_final_data = (PATH_MAX*2) +1;

		if(my_rank == 0){
			size_t actual_start = 0;
			size_t actual_end = 0;
			for(int source = 1; source < process_count; ++source){
				actual_start = source*partition + MIN((size_t)source, mod);
				actual_end = (source+1)*partition + MIN((size_t)source+1, mod);
				for(size_t index = actual_start ; index < actual_end ; ++index){
					MPI_Recv(&vector_dist[index].distance, 1, MPI_UNSIGNED_LONG_LONG, source, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

				}
			}
			qsort(vector_dist, size_vector, sizeof(info_vec), comp_funct);
			for(size_t index  = 0; index < size_vector; ++index ){
				final_data = (char *) malloc (sizeof(char)*size_final_data);
				final_data[size_final_data]='\0';
				strcpy(final_data, vector_dist[index].name_file1);
				strcat(final_data, "\t");
				strcat(final_data, vector_dist[index].name_file2);
				printf("%ld\t%s\n", vector_dist[index].distance, final_data);
				free(final_data);
				final_data = NULL;
			}
		}
		else{
			for(size_t index  = my_a; index < my_b; ++index ){
				MPI_Send(&vector_dist[index].distance, 1, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD);
			}
		}

		for(size_t index = 0; index < size_vector; ++index){
			if(vector_dist[index].file1){
				free(vector_dist[index].file1);
				vector_dist[index].file1 = NULL;
			}
			if(vector_dist[index].file2){
				free(vector_dist[index].file2);
				vector_dist[index].file2 = NULL;
			}
			if(vector_dist[index].name_file1){
				free((void*)vector_dist[index].name_file1);
				vector_dist[index].name_file1 = NULL;
			}
			if(vector_dist[index].name_file2){
				free((void*)vector_dist[index].name_file2);
				vector_dist[index].name_file1 = NULL;
			}
		}

		free(vector_dist);

		queue_destroy(this->files, true);
    if(!this->arguments.silent && !this->arguments.quiet && my_rank == 0){
        printf("Elapsed %.9lfs with %d workers\n", walltime_elapsed(&start), this->arguments.workers);
    }
		MPI_Finalize();
		return 0;
}

int levdist_list_files_in_args(levdist_t* this, int argc, char* argv[])
{
	// Traverse all arguments
	for ( int current = 1; current < argc; ++current )
	{
		// Skip command-line options
		const char* arg = argv[current];
		if ( *arg == '-' )
			continue;

		dir_list_files_in_dir(this->files, arg, this->arguments.recursive);
	}

	return 0;
}

void levdist_print_and_destroy_files(levdist_t* this)
{
	long count = 0;
	while ( ! queue_is_empty(this->files) )
	{
		char* filename = (char*)queue_pop(this->files);
        if(false){
            printf("%ld: %s\n", ++count, filename);
        }
		free(filename);
	}
}

void levdist_print_files(levdist_t* this)
{
	long count = 0;
    for ( queue_iterator_t itr = queue_begin(this->files); itr != queue_end(this->files); itr=queue_next(itr) )
	{
		const char* filename = (const char*)queue_data(itr);
		printf("%ld: %s\n", ++count, filename);
	}
}
