program main

use nvml_fortran
implicit none

call nvml_init()

call nvml_start()

call nvml_stop()

end program main
