#Makefile For All Projects
#all will make every version of the project
#Chaging any of the CDEFINES will alter the functionality of the projects
#If you decide to alter any CDEFINES, please make sure to make clean then make

GXX=gcc
MPIXX=mpicc
CFLAGS=-Wall --pedantic 
CLIBS=-lm
CDEFINES=-DDT=1e-8 -DMIN=0 -DMAX=M_PI -DT="long double"
#Define list:
# DT -> Step size for the Reimann Sum
# MIN -> Where to start the integral from
# MAX -> Where to end the integral at
# T -> The type of floating point precision to use

.PHONY:all
all: fp_omp fp_pthread fp_mpi fp_mpi_pthread fp_mpi_omp

fp_omp: fp_omp.c
	${GXX} -o fp_omp fp_omp.c ${CFLAGS} ${CDEFINES} ${CLIBS} -fopenmp

fp_pthread: fp_pthread.c
	${GXX} -o fp_pthread fp_pthread.c ${CFLAGS} ${CDEFINES} ${CLIBS} -pthread

fp_mpi: fp_mpi.c
	${MPIXX} -o fp_mpi fp_mpi.c ${CFLAGS} ${CDEFINES} ${CLIBS} 

fp_mpi_pthread: fp_mpi_pthread.c
	${MPIXX} -o fp_mpi_pthread fp_mpi_pthread.c ${CFLAGS} ${CDEFINES} ${CLIBS} -pthread

fp_mpi_omp: fp_mpi_omp.c
	${MPIXX} -o fp_mpi_omp fp_mpi_omp.c ${CFLAGS} ${CDEFINES} ${CLIBS} -fopenmp

.PHONY:txt
txt: all
	./run_programs.bash

.PHONY:hybrid_txt
hybrid_txt: all
	./run_hybrid.bash

.PHONY:clean
clean:
	rm -rf fp_omp fp_pthread fp_mpi fp_mpi_pthread fp_mpi_omp
