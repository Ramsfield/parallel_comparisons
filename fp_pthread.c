//File: fp_pthread.c
//
//Reimann Sum integration estimator used for benchmarking pthreads on a given system
//Program can be created using make fp_pthread and ran using ./fp_pthread NUM_THREADS
#include <stdio.h> //For prints
#include <time.h> //For timing the project
#include <math.h> 
#include <stdlib.h> //For exit

#include <pthread.h>

#define METHOD "Pthread"

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

T get_time(struct timespec * start, struct timespec * end)
{
  //Get the difference in type T between two points
  T s = start->tv_sec + (start->tv_nsec / 1e+9);
  T e = end->tv_sec + (end->tv_nsec / 1e+9);
  return e-s;
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

  //Set up thread info
  pthread_t threads[THREADS];
  pthread_mutex_t lock;
  pthread_mutex_init(&lock, NULL);

  tinfo myinfo[THREADS];

  for(int i=0; i < THREADS; i++)
  {
    myinfo[i].tnum = i;
    myinfo[i].cur_min = segment * i;
    myinfo[i].cur_max = (i == THREADS-1) ? MAX : segment * (i+1);
    myinfo[i].integral = &integral;
    myinfo[i].lock = &lock;
  }

  //Start Timer
  clock_gettime(CLOCK_REALTIME, &start);

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

  //Stop Time
  clock_gettime(CLOCK_REALTIME, &end);

  printf("Final Reimann Sum is %1.20Lf\n", integral);
  printf("Method %s took %1.5Lf seconds\n", METHOD, get_time(&start, &end));
  return 0;
}
