#!/bin/bash

#Bash program to automate the running and reading of all of the programs

#Initialize all the files
rm -rf pthread.txt omp.txt mpi.txt 
touch pthread.txt omp.txt mpi.txt

#Run the program 10 times for each number of cores 1 through 10
for x in {1..8}
do
  echo "Running programs with $x threads"
  echo "THREAD COUNT = $x" >> pthread.txt
  echo "THREAD COUNT = $x" >> mpi.txt
  echo "THREAD COUNT = $x" >> omp.txt
  for i in {1..10}
  do
    #Single Methods
    echo "Run $i with OpenMP with $x threads"
    ./fp_omp $x >> omp.txt
    echo "Run $i with pthread with $x threads"
    ./fp_pthread $x >> pthread.txt
    echo "Run $i with MPI with $x threads"
    mpirun -np $x ./fp_mpi >> mpi.txt
  done
done
