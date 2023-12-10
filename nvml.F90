module nvml
        
use iso_c_binding
implicit none
public

!!!!!!!!!!!!!!!!!!!!!!!!!!
! Fortran to C interface !
!!!!!!!!!!!!!!!!!!!!!!!!!!

interface
  subroutine nvml_init_internal() bind(C, name='nvml_init')
  end subroutine 

  subroutine nvml_stop_internal() bind(C, name="nvml_stop")
  end subroutine 
end interface

contains

  !!!!!!!!!!!!!!!!!!!
  ! Initializa NVML !
  !!!!!!!!!!!!!!!!!!!
  subroutine nvml_init()
    call nvml_init_internal() 
  end subroutine

  !!!!!!!!!!!!!!!!!!
  ! Shut down NVML !
  !!!!!!!!!!!!!!!!!!
  subroutine nvml_stop() 
    call nvml_stop_internal()
  end subroutine

end module nvml
