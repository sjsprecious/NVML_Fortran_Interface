program main

use nvml_fortran
#ifdef _MPI
use mpi
#endif
implicit none

integer, parameter :: wp = selected_real_kind(15)
integer, parameter :: ITER_MAX = 1000, &
                      rows = 10000, &
                      cols = 10000
real(wp), parameter :: BC = 10._wp
integer :: i, j, iter
integer :: myrank, mygpu, error
character(len=2) :: gpu_id
real(wp), allocatable, dimension(:,:) :: a_new, a_gpu

#ifdef _MPI
call mpi_init(error)
call mpi_comm_rank(mpi_comm_world, myrank, error)
#else
myrank = 0
#endif

! On Derecho:
!   - In an MPI run, the "set_gpu_rank" wrapper script should be used and CUDA_VISIBLE_DEVICES is mapped automatically;
!   - In a non-MPI run, the CUDA_VISIBLE_DEVICES variable may not be set properly;
call get_environment_variable("CUDA_VISIBLE_DEVICES", gpu_id)
! Convert character to integer
read(gpu_id,*) mygpu

allocate( a_gpu(0:rows+1,0:cols+1), a_new(rows,cols) )

! Initialize inner elements of a_gpu with zero

do j = 1, cols
   do i = 1, rows
      a_gpu(i,j) = 0._wp
   end do
end do

! Initialize the boundary conditions with fixed values

do j = 1, cols     
   a_gpu(0,j) = BC
   a_gpu(rows+1,j) = BC
end do

do i = 1, rows
   a_gpu(i,0) = BC
   a_gpu(i,cols+1) = BC
end do

! Start to collect the GPU power usage
call nvml_start(myrank, mygpu)

!$acc data copy (a_gpu) create(a_new)
do iter = 1, ITER_MAX
   !$acc parallel vector_length(128)
   !$acc loop gang vector collapse (2)
   do j = 1, cols
      do i = 1, rows
         a_new(i,j) = 0.25_wp * (a_gpu(i,j-1) + &
                                 a_gpu(i-1,j) + &
                                 a_gpu(i+1,j) + &
                                 a_gpu(i,j+1))
      end do
   end do
   !$acc end parallel

   !$acc parallel vector_length(128)
   !$acc loop gang vector collapse (2)
   do j = 1, cols
      do i = 1, rows
         a_gpu(i,j) = a_new(i,j)
      end do
   end do
   !$acc end parallel
end do
!$acc end data

! Stop to collect the GPU power usage
call nvml_stop()

end program main
