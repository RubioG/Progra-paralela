#include<stdio.h>  // libreria basica para entrada y salida
#include<stdlib.h> // libreria para el uso de rand()
#include<omp.h>
#include <unistd.h>

void swap (double * dato1, double * dato2){
  double temp = *dato1;
  *dato1 = *dato2;
  *dato2 = temp;
}

void serial_odd_even_sort(size_t n, double a[n], size_t cant_threads){
  for ( size_t phase = 0; phase < n; ++phase )
  {
  	if ( phase % 2 == 0 )
  	{
      #pragma omp parallel for num_threads(cant_threads) \
        default(none) shared (cant_threads,n,a)
  		for ( size_t i = 1; i < n; i += 2 )
  			if ( a[i - 1] > a[i] )
  				swap( &a[i - 1], &a[i] );
  	}
  	else
  	{
      #pragma omp parallel for num_threads(cant_threads) \
        default(none) shared (cant_threads,n,a)
  		for (size_t i = 1; i < n - 1; i += 2 )
  			if ( a[i] > a[i + 1] )
  				swap( &a[i], &a[i + 1]);
  	}
  }
  printf("{ ");
  for ( size_t phase = 0; phase < n; ++phase ){
    if(phase != 0){
      printf(", ");
    }
    printf("%lf",a[phase]);
  }
  printf(" }\n");
}

int main(int argc, char* argv[])
{
  size_t arr_size = 0;
  if(argc >= 2){
      arr_size = atoi(argv[1]);
      size_t cant_threads=sysconf(_SC_NPROCESSORS_ONLN);
      if(argc >= 3){
        cant_threads = atoi(argv[2]);
      }
      double * arr = (double*)malloc( arr_size * sizeof(double));
      #pragma omp parallel for num_threads(cant_threads) \
        default(none) shared (cant_threads,arr_size,arr)
      for(size_t index = 0; index < arr_size ; ++index){
        arr[index] =(double)rand()/ RAND_MAX * 1000.0-0.0;
      }
			serial_odd_even_sort(arr_size, arr, 1);
      free(arr);
  }
	return 0;
}
