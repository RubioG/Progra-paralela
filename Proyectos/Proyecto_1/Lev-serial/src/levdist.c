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


void levdist_init(levdist_t* this)
{
	arguments_init(&this->arguments);
	this->files = NULL;
    this->tree = NULL;
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
    if(!this->arguments.silent && !this->arguments.quiet){
        // Print filenames found
        levdist_print_files(this);
    }
    FILE * file1;
    FILE * file2;
    char * final_file1;
    char * final_file2;
    const char * name_file1;
    const char * name_file2;
    char * final_data;
    long size_file1;
    long size_file2;
    size_t distance=0;

    if(queue_count(this->files)==1){
        return fprintf(stderr, "levdist: error: at least two files are required to compare"),2;
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
        final_file1 = (char*) malloc (sizeof(char)*(size_file1+1));

        //subtract the information from the file
        final_file1[size_file1]='\0';
        fread(final_file1, 1 ,size_file1+1, file1);
        size_file1=strlen((char*)queue_data(index));

        for(queue_iterator_t index1 = queue_next(index); index1 != queue_end(this->files) ; index1=queue_next(index1)){

            //Obtain name of the file
            name_file2 = (const char *) queue_data(index1);

            if(strcmp(name_file1, name_file2) == 0){
                return fprintf(stderr, "levdist: error: at least two files are required to compare"),2;
            }

            //Open the file
            file2 = fopen ( name_file2 , "rb" );

            //Obtain size of file
            fseek(file2, 0, SEEK_END);
            size_file2 = ftell(file2);
            rewind(file2);

            // allocate memory to contain the whole file:
            final_file2 = (char*) malloc (sizeof(char)*(size_file2+1));


            //subtract the information from the file
            final_file2[size_file2]='\0';
            fread(final_file2, 1,size_file2+1, file2);

            //obtain distance
            distance=levenshtein( final_file1, final_file2);


            size_file2=strlen((char*)queue_data(index1));

            //concat the data
            size_t size_final_data = (PATH_MAX*2) +1;

            // allocate memory to contain the final data
            final_data = (char *) malloc (sizeof(char)*size_final_data);
            final_data[size_final_data]='\0';

            //choose which order should be stored
            if( strcmp (name_file1,name_file2) < 0){
                strcpy(final_data, name_file1);
                strcat(final_data, "\t");
                strcat(final_data, name_file2);
            }
            else{
                strcpy(final_data, name_file2);
                strcat(final_data, "\t");
                strcat(final_data, name_file1);
            }

            //insert data
            if(this->tree == NULL){
                this->tree = new_node(distance, final_data);
            }
            else{
                insert(this->tree,distance, final_data);
            }

            // terminate
            free (final_file2);
            fclose (file2);
        }
        //terminate
        free (final_file1);
        fclose (file1);
    }

    //print the result
    inorder(this->tree);



	queue_destroy(this->files, true);
    if(!this->arguments.silent){
        // Report elapsed time
        printf("Elapsed %.9lfs with %d workers\n", walltime_elapsed(&start), this->arguments.workers);
    }

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
