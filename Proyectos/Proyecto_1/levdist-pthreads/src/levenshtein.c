#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include "levenshtein.h"

static size_t size_dictionary = 256;
static size_t len_text = 0;
static size_t len_patron = 0;
static unsigned char* patron;
static unsigned char* texto;

size_t MIN3(size_t a, size_t b, size_t c){
    return ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)));
}


void* pthread_distance(void* data){
    info_thread_t* thread = (info_thread_t*)data;
    size_t inicio = (size_dictionary/thread->total_threads) * thread->current_worker;
    size_t final = ((size_dictionary/thread->total_threads) * (thread->current_worker + 1));
    if(thread->current_worker+1 == thread->total_threads){
      final = size_dictionary;
    }

    for( size_t index = inicio; index < final ; ++index ){
        for( size_t index2 = 0; index2 < len_text ; ++index2){
            if(index2 == 0){
                thread->matrix_X[index][index2] = 0;
            }else if(texto[index2 - 1] == (unsigned char)index){
                thread->matrix_X[index][index2] = index2;
            }else{
                thread->matrix_X[index][index2] = thread->matrix_X[index][index2-1];
            }
        }
    }
    inicio = (len_text/thread->total_threads) * thread->current_worker;
    final = (len_text/thread->total_threads) * (thread->current_worker + 1);
    if(thread->current_worker+1 == thread->total_threads){
        final = len_text;
    }

    for( size_t index = 0; index < len_patron ; ++index){
        for( size_t index2 = inicio; index2 < final; ++index2){
            if(index == 0){
                thread->matrix_D[index%2][index2] = index2;
            }else if(index2 == 0){
                thread->matrix_D[index%2][index2] = index;
            }else if(texto[index2-1] == patron[index-1]){
                thread->matrix_D[index%2][index2] = thread->matrix_D[((index+1))%2][index2-1];
            }else if(thread->matrix_X[(int)patron[index-1]][index2] == 0){
                thread->matrix_D[index%2][index2] = 1 + MIN3(thread->matrix_D[((index+1))%2][index2],thread->matrix_D[((index+1))%2][index2-1],index+index2-1);
            }else{
                thread->matrix_D[index%2][index2] = 1 + MIN3(thread->matrix_D[((index+1))%2][index2],thread->matrix_D[((index+1))%2][index2-1],(thread->matrix_D[((index+1))%2][(thread->matrix_X[(int)patron[index-1]][index2])-1])+((index2 - 1 - thread->matrix_X[(int)patron[index-1]][index2])));
            }
        }
        pthread_barrier_wait((thread->barrier));
    }

    return NULL;
}


size_t distance_levdist(unsigned char *s1, unsigned char *s2, int cant_threads){
    size_t result = 0;
    pthread_barrier_t barrier;
    patron = s1;
    texto = s2;
    len_patron = strlen((char*)s1);
    len_text = strlen((char*)s2);
    if(len_patron >len_text){
        patron = s2;
        texto = s1;
        size_t aux = len_patron ;
        len_patron = len_text;
        len_text = aux;
    }
    pthread_t* threads;
    if(len_patron >0 && len_text>0){

        size_t** matrix_X;
        matrix_X = (size_t **)malloc( size_dictionary * sizeof(size_t*));
        for(size_t index = 0; index < size_dictionary ; ++index){
            matrix_X[index] = (size_t *)malloc((len_text+1) * sizeof(size_t));
        }

        size_t** matrix_D;
        matrix_D = (size_t **)malloc( 2 * sizeof(size_t*));
        for(size_t index = 0; index < 2 ; ++index){
            matrix_D[index] = (size_t *)malloc((len_text+1) * sizeof(size_t));
        }

        pthread_barrier_init(&barrier, NULL, (unsigned)cant_threads);

        threads = (pthread_t*) malloc(cant_threads * sizeof(pthread_t));

        info_thread_t* threadinfo = (info_thread_t*) malloc(cant_threads * sizeof(info_thread_t));
        for ( size_t index = 0; index < (size_t)cant_threads; ++index ){
            threadinfo[index].current_worker = index;
            threadinfo[index].total_threads = cant_threads;
            threadinfo[index].matrix_X = matrix_X;
            threadinfo[index].matrix_D = matrix_D;
            threadinfo[index].barrier = &barrier;
            pthread_create(&threads[index], NULL, pthread_distance,(void*)&threadinfo[index]);
        }

        for(int index = 0;index < cant_threads;index++){
            pthread_join(threads[index],NULL);
        }

        result = matrix_D[((len_patron -1)%2)][len_text-1];

        for(size_t index = 0; index < size_dictionary ; ++index){
            free(matrix_X[index]);
        }
        free(matrix_X);

        for(size_t index = 0; index < 2 ; ++index){
            free(matrix_D[index]);
        }
        free(matrix_D);
        free(threadinfo);
        free(threads);
    }
    return result;
}
