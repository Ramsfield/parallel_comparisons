//File: fp_mpi_omp.c
//
//Reimann Sum integration estimator used for benchmarking MPI and OpenMP for a given system
//Program can be created using make fp_mpi_omp and ran using mpirun -n NUM_PROCESSES ./fp_mpi_omp NUM_THREADS
//Where NUM_PROCESSES will indicate how many processes to spin off and NUM_THREADS indicates the number of threads to run per processes

#include <stdio.h> //For prints
#include <time.h> //For timing the project
#include <math.h> 
#include <stdlib.h> //For exit

#include <mpi.h>
#include <omp.h>

#define METHOD "MPI/OpenMP"


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

  T in_integral = 0.0;

  //Start Timer
  start = MPI_Wtime();

  //Find the integral
#pragma omp parallel for num_threads(THREADS)
  for(int j=0; j < THREADS; j++)
  {
    T t_cur_min, t_cur_max;
    t_cur_min = proc_sec * j + cur_min;
    if(j == THREADS-1)
    {
      t_cur_max = cur_max;
    }
    else
    {
      t_cur_max = proc_sec * (j + 1) + cur_min;
    }
    T t_in_integral = 0.0;

    for(T i = t_cur_min; i < t_cur_max; i+=DT)
    {
      t_in_integral += trap_reimann_sum(i, DT, f);
    }
#pragma omp critical
    in_integral += t_in_integral;
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
