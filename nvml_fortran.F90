module nvml_fortran
        
use iso_c_binding
implicit none
public

!!!!!!!!!!!!!!!!!!!!!!!!!!
! Fortran to C interface !
!!!!!!!!!!!!!!!!!!!!!!!!!!

interface
  subroutine nvml_start_internal() bind(C, name='nvml_start')
  end subroutine

  subroutine nvml_stop_internal() bind(C, name="nvml_stop")
  end subroutine 
end interface

contains

  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  ! Start to collect GPU power usage !
  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  subroutine nvml_start()
    call nvml_start_internal()
  end subroutine

  !!!!!!!!!!!!!!!!!!
  ! Shut down NVML !
  !!!!!!!!!!!!!!!!!!
  subroutine nvml_stop() 
    call nvml_stop_internal()
  end subroutine

end module nvml_fortran
