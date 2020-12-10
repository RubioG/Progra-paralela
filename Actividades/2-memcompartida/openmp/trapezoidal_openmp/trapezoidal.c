#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <omp.h>
#include <pthread.h>

double fun(double x)
{
  double f;
  f = x*x-4;
  return f;
}

int main(int argc, char* argv[])
{
  time_t start = clock();
  double a,b,dx,area;
  int n;
  size_t cant_threads=sysconf(_SC_NPROCESSORS_ONLN);
  if(argc >= 4){
      a = atof(argv[1]);
      b = atof(argv[2]);
      n = atof(argv[3]);
  }
  if(argc >= 5){
    cant_threads = atof(argv[4]);
  }
  dx = (b - a) / n;
  double suma = 0;
  #pragma omp parallel for num_threads(cant_threads) default(none) shared (n,a,dx,suma)
  for (int index = 0; index < n; ++index){
      #pragma omp critical
      {
          suma += (double)fun( a + index * dx);
      }
  }
  area = (b-a) * ((fun(a) + (2 * suma) + fun(b)) / (2 * n));
  printf ("El area es:%f \n",area);
  time_t end = clock();
  printf("Took %lf seconds\n", (double)(end-start)/CLOCKS_PER_SEC);
  return 0;
}
