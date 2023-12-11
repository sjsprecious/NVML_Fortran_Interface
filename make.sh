#!/bin/bash

# Load necessary modules
module purge
module load ncarenv/23.06
module load nvhpc/23.1
module load cuda/11.7.1
module load cray-mpich/8.1.25
module load ncarcompilers/1.0.0

# Build the source code
nvc++ -c nvml_cpp.cpp -D_MPI -L${NCAR_LDFLAGS_CUDA64}/stubs -lnvidia-ml
nvfortran -c nvml_fortran.F90
nvfortran -c -D_MPI -acc -gpu=cc80,lineinfo -Minfo=accel main.F90
nvfortran -o ./a.out main.o nvml_fortran.o nvml_cpp.o -acc -gpu=cc80,lineinfo -Minfo=accel -L${NCAR_LDFLAGS_CUDA64}/stubs -lnvidia-ml

# Execute
mpiexec -n 4 -ppn 4 set_gpu_rank ./a.out

# Clean up
rm -f a.out *.o *.mod
