#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/time.h>

typedef struct info_thread
{
        size_t index;
        double indice;
        double * suma;
        double dx;
        double a;
        int iteraciones;
        size_t size;
        sem_t * mutex_vector;
} info_thread;

double fun(double x)
{
  double f;
  f = x*x-4;
  return f;
}

void* oper(void* data)
{
  info_thread * info = (info_thread*)(data);
  sem_wait(&(info->mutex_vector[info->index]));
  for (int index = 0; index < (info->iteraciones-1); ++index){
      (*info->suma) += (double)fun( info->a + (info->indice+index) * info->dx);
  }
  if(info->index < (info->size - 1)){
    sem_post(&(info->mutex_vector[info->index+1]));
  }
  return NULL;
}

int main(int argc, char* argv[])
{
  time_t start = clock();
  double a,b,n,dx,area;
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
  int particion = n/(cant_threads);
  int particion_actual = n;
  pthread_t threads[cant_threads];
  info_thread * vector_info[cant_threads];
  sem_t sem_vector[cant_threads];
  sem_init(&(sem_vector[0]), 0, 1);
  for(size_t index = 1; index < cant_threads ; ++index){
      sem_init(&(sem_vector[index]), 0, 0);
  }
  size_t indice = 0;
  for(size_t index = 0; index < cant_threads ; ++index, ++indice){
    vector_info[index] = calloc(1,sizeof(info_thread));
    vector_info[index]->indice = indice;
    vector_info[index]->index = index;
    vector_info[index]->size = cant_threads;
    vector_info[index]->mutex_vector = sem_vector;
    vector_info[index]->suma = (&suma);
    vector_info[index]->a = a;
    vector_info[index]->dx = dx;
    vector_info[index]->iteraciones = particion;
    if((particion_actual - particion) <= 0){
      vector_info[index]->iteraciones = particion_actual-1;
    }
    particion_actual-=particion;
    indice += particion;
    pthread_create( &threads[index], NULL, oper, (void*)vector_info[index] );
  }
  for(size_t index = 0; index < cant_threads ; ++index){
      pthread_join(threads[index], NULL);
  }

  for(size_t index = 0; index < cant_threads ; ++index){
      free(vector_info[index]);
      sem_destroy(&(sem_vector[index]));
  }
  area = (b-a) * ((fun(a) + (2 * suma) + fun(b)) / (2 * n));
  printf ("El area es:%f \n",area);
  time_t end = clock();
  printf("Took %lf seconds\n", (double)(end-start)/CLOCKS_PER_SEC);
  return 0;
}
