This is a project that I'm using as a final project for CSCI - 551 Numerical and Parallel programming.

This uses 3 independent methods of parallelizing in combination with a Reimann sum integration estimation to measure the efficacy of the parallel methods.

The methods of parallelization are:
  * MPI
  * OpenMP
  * Pthreads

Also inclused are two Hybrid versions:
  * MPI with OpenMP
  * MPI with Pthreads

All output is generated from my i5-7200U laptop running Ubuntu. Since this is a 4 core system with hyperthreading, I only tested with up to 8 total processes/threads

Note: the Hybrid methods were tested using a cluster available at CSU, Chico
