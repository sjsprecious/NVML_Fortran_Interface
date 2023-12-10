#!/bin/bash

# Load necessary modules
module purge
module load ncarenv/23.06
module load nvhpc/23.1
module load cuda/11.7.1
module load ncarcompilers/1.0.0

# Build the source code
nvc++ -c nvml_cpp.cpp -L${NCAR_LDFLAGS_CUDA64}/stubs -lnvidia-ml
nvfortran -c nvml_fortran.F90
nvfortran -c main.F90
nvfortran -o ./a.out main.o nvml_fortran.o nvml_cpp.o -L${NCAR_LDFLAGS_CUDA64}/stubs -lnvidia-ml

# Execute
./a.out

# Clean up
rm -f a.out *.o *.mod
