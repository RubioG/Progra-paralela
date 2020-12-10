#include<stdio.h>  // libreria basica para entrada y salida
#include<stdlib.h> // libreria para el uso de rand()

void swap (double * dato1, double * dato2){
  double temp = *dato1;
  *dato1 = *dato2;
  *dato2 = temp;
}

void serial_odd_even_sort(size_t n, double a[n]){
  for ( int phase = 0; phase < n; ++phase )
  {
  	if ( phase % 2 == 0 )
  	{
  		for ( int i = 1; i < n; i += 2 )
  			if ( a[i - 1] > a[i] )
  				swap( &a[i - 1], &a[i] );
  	}
  	else
  	{
  		for (int i = 1; i < n - 1; i += 2 )
  			if ( a[i] > a[i + 1] )
  				swap( &a[i], &a[i + 1]);
  	}
  }
  printf("{ ");
  for ( int phase = 0; phase < n; ++phase ){
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
      double * arr = (double*)malloc( arr_size * sizeof(double));
      for(int index = 0; index < arr_size ; ++index){
        arr[index] =(double)rand()/ RAND_MAX * 1000.0-0.0;
      }
			serial_odd_even_sort(arr_size, arr);
      free(arr);
  }
	return 0;
}
