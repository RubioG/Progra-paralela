#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <omp.h>
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


size_t distance_levdist(unsigned char *s1, unsigned char *s2, int cant_threads){
    size_t result = 0;
    //pthread_barrier_t barrier;
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

        //pthread_barrier_init(&barrier, NULL, (unsigned)cant_threads);
        #pragma omp parallel default(none) num_threads(cant_threads) shared(size_dictionary, patron, texto, len_text, len_patron,  matrix_D, matrix_X)
        {
            #pragma omp for
            for( size_t index = 0; index < size_dictionary ; ++index ){
                for( size_t index2 = 0; index2 < len_text ; ++index2){
                    if(index2 == 0){
                        matrix_X[index][index2] = 0;
                    }else if(texto[index2 - 1] == (unsigned char)index){
                        matrix_X[index][index2] = index2;
                    }else{
                        matrix_X[index][index2] = matrix_X[index][index2-1];
                    }
                }
            }

            for( size_t index = 0; index < len_patron ; ++index){
                #pragma omp for
                for( size_t index2 = 0; index2 < len_text; ++index2){
                    if(index == 0){
                        matrix_D[index%2][index2] = index2;
                    }else if(index2 == 0){
                        matrix_D[index%2][index2] = index;
                    }else if(texto[index2-1] == patron[index-1]){
                        matrix_D[index%2][index2] = matrix_D[((index+1))%2][index2-1];
                    }else if(matrix_X[(int)patron[index-1]][index2] == 0){
                        matrix_D[index%2][index2] = 1 + MIN3(matrix_D[((index+1))%2][index2],matrix_D[((index+1))%2][index2-1],index+index2-1);
                    }else{
                        matrix_D[index%2][index2] = 1 + MIN3(matrix_D[((index+1))%2][index2],matrix_D[((index+1))%2][index2-1],(matrix_D[((index+1))%2][(matrix_X[(int)patron[index-1]][index2])-1])+((index2 - 1 - matrix_X[(int)patron[index-1]][index2])));
                    }
                }
                #pragma omp barrier
            }
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
    }
    return result;
}
