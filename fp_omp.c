//File: fp_omp.c
//
//Reimann Sum integration estimator used to benchmarking OpenMP for a given system
//Program can be created using make fp_omp and ran using ./fp_omp NUM_THREADS

#include <stdio.h> //For prints
#include <time.h> //For timing the project
#include <math.h> 
#include <stdlib.h> //For exit

#include <omp.h>

#define METHOD "OpenMP"

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

T get_time(struct timespec * start, struct timespec * end)
{
  //Get the difference in type T between two points
  T s = start->tv_sec + (start->tv_nsec / 1e+9);
  T e = end->tv_sec + (end->tv_nsec / 1e+9);
  return e-s;
}

int main(int argc, char * argv[])
{
  if(argc < 2)
  {
    printf("Useage: %s num_threads\n", argv[0]);
    exit(-1);
  }
  int THREADS;
  sscanf(argv[1], "%d", &THREADS);

  T integral = 0.0;
  struct timespec start, end;

  //The range we'll cross
  T range = MAX - MIN;

  //Each segment size
  T segment = range/THREADS;

  //Start Timer
  clock_gettime(CLOCK_REALTIME, &start);

  //Find the integral
#pragma omp parallel for num_threads(THREADS)
  for(int j=0; j < THREADS; j++)
  {
    T cur_min, cur_max;
    cur_min = segment * j;
    if(j == THREADS-1)
    {
      cur_max = MAX;
    }
    else
    {
      cur_max = segment * (j + 1);
    }
    T in_integral = 0.0;

    for(T i = cur_min; i < cur_max; i+=DT)
    {
      in_integral += trap_reimann_sum(i, DT, f);
    }
#pragma omp critical
    integral += in_integral;
  }

  //Stop Time
  clock_gettime(CLOCK_REALTIME, &end);

  printf("Final Reimann Sum is %1.20Lf\n", integral);
  printf("Method %s took %1.5Lf seconds\n", METHOD, get_time(&start, &end));
  return 0;
}
