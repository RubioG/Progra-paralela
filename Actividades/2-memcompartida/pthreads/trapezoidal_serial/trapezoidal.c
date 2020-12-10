#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

float fun(float x)
{
  float f;
  f=x*x-4;
  return f;
}
int main(int argc, char* argv[])
{
  time_t start = clock();
  float a,b,n,dx,suma,index,xi,area;
  if(argc >= 4){
      a = atof(argv[1]);
      b = atof(argv[2]);
      n = atof(argv[3]);
  }
  dx = (b - a) / n;
  suma = 0;
  index = 1;
  while (index <= n-1)
  {
    xi = a + index * dx;
    suma = suma + fun(xi);
    index += 1;
  }
  area = (b-a) * ((fun(a) + 2 * suma + fun(b)) / (2 * n));
  printf("%f\n",suma );
  printf ("El area es:%f \n",area);
  time_t end = clock();
  printf("Took %lf seconds\n", (double)(end-start)/CLOCKS_PER_SEC);
  return 0;
}
