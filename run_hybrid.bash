#!/bin/bash

#Bash program to automate the running and reading of all of the programs

#Initialize all the files
rm -rf mpi_pthread.txt mpi_omp.txt
touch mpi_pthread.txt mpi_omp.txt

#Run Hybrid Methods
for i in {1..10}
do
  #Hybrid Methods
  echo "Run $i for mpi pthread"
  mpirun -n 4 --hostfile hosts ./fp_mpi_pthread 2 >> mpi_pthread.txt
  echo "Run $i for mpi openmp"
  mpirun -n 4 --hostfile hosts ./fp_mpi_omp 2 >> mpi_omp.txt
done
