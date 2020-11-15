//File: fp_mpi_pthread.c
//
//Reimann Sum integration estimator used for benchmarking MPI and Pthreads for a given system
//Program can be created using make fp_mpi_pthread and ran using mpirun -n NUM_PROCESSES ./fp_mpi_pthread NUM_THREADS
//Where NUM_PROCESSES will indicate how many processes to spin off and NUM_THREADS indicates the number of threads to run per processes

#include <stdio.h> //For prints
#include <time.h> //For timing the project
#include <math.h> 
#include <stdlib.h> //For exit

#include <mpi.h>
#include <pthread.h>

#define METHOD "MPI/Pthreads"

typedef struct{
  int tnum;
  T cur_min;
  T cur_max;
  T * integral;
  pthread_mutex_t * lock;
} tinfo;

T f(T x)
{
  return sin(x);
}

/*
 * Trapezoidal Reimann Sum
 * Will find the area between a point x and some point after x, dx
 * Height is determined by function passed in by fx
 */
T trap_reimann_sum(T x, T dx, T(*fx)(T))
{
  T h1, h2;
  h1 = fx(x);
  h2 = fx(x+dx);
  return ((h1 + h2)/2) * dx;
}

T get_time(T * start, T * end)
{
  return *end - *start;
}

void * thread_calc(tinfo * myinfo)
{
    T in_integral = 0.0;

    for(T i = myinfo->cur_min; i < myinfo->cur_max; i+=DT)
    {
      in_integral += trap_reimann_sum(i, DT, f);
    }
    pthread_mutex_lock(myinfo->lock);
    *myinfo->integral += in_integral;
    pthread_mutex_unlock(myinfo->lock);
    return NULL;
}

int main(int argc, char * argv[])
{
  MPI_Init(&argc, &argv);
  if(argc < 2)
  {
    printf("Useage: mpirun -n num_processes %s num_threads\n", argv[0]);
    exit(-1);
  }
  int THREADS;
  sscanf(argv[1], "%d", &THREADS);

  int rank, size;

  //Get MPI process information
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  //Timing Info
  T start, end;

  //Final Integral information
  T integral = 0.0;

  //The range we'll cross
  T range = MAX - MIN;

  //Each segment size
  T segment = range/size;

  T cur_min, cur_max;
  cur_min = (segment * rank) + MIN;
  cur_max = (rank == size-1) ? MAX : (segment * (rank + 1)) + MIN;

  T proc_range = cur_max - cur_min;
  T proc_sec = proc_range/THREADS;

  //Thread info
  pthread_t threads[THREADS];
  pthread_mutex_t lock;
  tinfo myinfo[THREADS];
  T in_integral = 0.0;

  pthread_mutex_init(&lock, NULL);

  for(int i=0; i < THREADS; i++)
  {
    myinfo[i].tnum = i;
    myinfo[i].cur_min = proc_sec * i + cur_min;
    myinfo[i].cur_max = (i == THREADS-1) ? cur_max : (proc_sec * (i+1)) + cur_min;
    myinfo[i].integral = &in_integral;
    myinfo[i].lock = &lock;
  }

  //Start Timer
  start = MPI_Wtime();

  //Find the integral
  for(int i=0; i < THREADS-1; i++)
  {
    pthread_create(&threads[i], NULL, (void *(*)(void*))thread_calc, myinfo+i);
  }
  //Make main do the last one
  thread_calc(myinfo + (THREADS-1));

  //Join back threads
  for(int i=0; i < THREADS-1; i++)
  {
    pthread_join(threads[i], NULL);
  }

  MPI_Reduce(&in_integral, &integral, 1, MPI_LONG_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  //Stop Time
  end = MPI_Wtime();

  //Print results
  if(!rank)
  {
    printf("Final Reimann Sum is %1.20Lf\n", integral);
    printf("Method %s took %1.5Lf seconds\n", METHOD, get_time(&start, &end));
  }
  MPI_Finalize();
  return 0;
}
