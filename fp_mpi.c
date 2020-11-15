//File: fp_mpi.c
//
//Reimann Sum integration estimator used to benchmark MPI for a given system
//Program can be created using make fp_mpi and ran using mpirun -n NUM_PROCESSES ./fp_mpi

#include <stdio.h> //For prints
#include <time.h> //For timing the project
#include <math.h> 
#include <stdlib.h> //For exit

#include <mpi.h>

#define METHOD "MPI"

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

  int rank, size;

  //Get MPI process information
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  //Timing Info
  T start, end;

  T integral = 0.0;

  //The range we'll cross
  T range = MAX - MIN;

  //Each segment size
  T segment = range/size;

  //Start Timer
  start = MPI_Wtime();


  T cur_min, cur_max;
  cur_min = segment * rank;
  cur_max = (rank == size-1) ? MAX : segment * (rank + 1);

  T in_integral = 0.0;
  for(T i = cur_min; i < cur_max; i+=DT)
  {
    in_integral += trap_reimann_sum(i, DT, f);
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
