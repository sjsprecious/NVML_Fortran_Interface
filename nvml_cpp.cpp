#include <iostream>
#include <nvml.h>
#include <pthread.h>
#include <unistd.h>    // for usleep function
#include <fstream>     // for ofstream class
#include <iomanip>     // for setprecision function
#include <cstdlib>     // for getenv function 

// Declaration of functions
extern "C" {
  void nvml_start( int mpi_rank_id, int device_id );
  void nvml_stop();
}
void check_status(nvmlReturn_t nvmlResult);
void *power_polling_func(void *ptr);

// Print additional diagnostic output
const bool verbose = false;

// Time interval between two samples of GPU power usage (unit: microseconds)
const unsigned int time_interval = 100000;

// Output file path and name
std::string filepath = "";

// Define variables for NVML APIs
unsigned int deviceCount = 0;
const int name_length = 64;
char deviceNameStr[name_length];
nvmlReturn_t nvmlResult;
nvmlDevice_t nvmlDeviceID;
nvmlPciInfo_t nvmPCIInfo;
nvmlEnableState_t powermode;
nvmlComputeMode_t computemode;

// Define variables for pthread
bool pollThreadStatus = false;
pthread_t powerPollThread;
pthread_mutex_t fileMutex = PTHREAD_MUTEX_INITIALIZER; // Mutex to synchronize file access

// Use pthread to poll the GPU data obtained from NVML APIs
void *power_polling_func( void *ptr )
{
  unsigned int powerlevel = 0;

  // Open the file in append mode
  std::ofstream file(filepath.c_str(), std::ios::app);

  // Acquire the lock before writing to the file
  pthread_mutex_lock(&fileMutex);

  while (pollThreadStatus)
  {
    // This thread is not cancelable
    int error = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, 0);
    if ( error != 0 )
    {
      if (verbose) std::cout << "Error: fail to set the pthread state." << std::endl;
      exit(error);
    }

    // Get the power management mode of the GPU.
    nvmlResult = nvmlDeviceGetPowerManagementMode(nvmlDeviceID, &powermode);
    check_status(nvmlResult);

    // Check if power management mode is enabled.
    if (powermode == NVML_FEATURE_ENABLED)
    {
      // Get the GPU power usage in milliwatts and its associated circuitry (e.g. memory)
      nvmlResult = nvmlDeviceGetPowerUsage( nvmlDeviceID, &powerlevel );
      check_status(nvmlResult);
    }

    // The output file stores GPU power in Watts.
    if (file.is_open())
    {
      file << std::setprecision(3) << powerlevel/1000.0 << "\n";
    } 
    else
    {
      std::cout << "Unable to open the file for writing. Something is wrong!\n";
      exit(123);
    }

    // This thread is now cancelable
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, 0);

    // Wait for this amount of time before next sampling 
    usleep(time_interval);
  }

  // Close the output file
  file.close();

  // Release the lock
  pthread_mutex_unlock(&fileMutex);

  pthread_exit(0);
}

// Initialize and start the NVML measurement
void nvml_start( int mpi_rank_id, int device_id )
{
  // Initialize nvml.
  nvmlResult = nvmlInit_v2();
  check_status(nvmlResult);

  if (mpi_rank_id == 0)
  {
    // Count the number of GPUs available.
    nvmlResult = nvmlDeviceGetCount( &deviceCount );
    check_status(nvmlResult);

    for (int i = 0; i < deviceCount; i++)
    {
      // Get the device ID.
      nvmlResult = nvmlDeviceGetHandleByIndex( i, &nvmlDeviceID );
      check_status(nvmlResult);

      // Get the name of the device.
      nvmlResult = nvmlDeviceGetName( nvmlDeviceID, deviceNameStr, name_length );
      check_status(nvmlResult);
      if (verbose) std::cout << "Device " << i << ", name = " << deviceNameStr << std::endl;

      // Get PCI information of the device.
      nvmlResult = nvmlDeviceGetPciInfo( nvmlDeviceID, &nvmPCIInfo );
      check_status(nvmlResult);

      // Get the compute mode of the device which indicates CUDA capabilities.
      nvmlResult = nvmlDeviceGetComputeMode( nvmlDeviceID, &computemode );
      check_status(nvmlResult);
      if (verbose) std::cout << "Device " << i << ", compute mode = " << computemode << std::endl;

      // Get the power management mode of the GPU.
      nvmlResult = nvmlDeviceGetPowerManagementMode( nvmlDeviceID, &powermode );
      check_status(nvmlResult);
      if (verbose) std::cout << "Device " << i << ", power mode = " << powermode << std::endl;
    }
  }   // End of if statement for "mpi_rank_id"

  // Change the value of the global variable; may not be refreshed in the child thread yet
  pollThreadStatus = true;
  filepath = "./power_usage_rank" + std::to_string(mpi_rank_id) +
	     "_gpu" + std::to_string(device_id) + ".txt";

  // Get the device ID.
  nvmlResult = nvmlDeviceGetHandleByIndex(device_id, &nvmlDeviceID);

  // Generate a new pthread
  int error = pthread_create(&powerPollThread, NULL, power_polling_func, NULL);
  if ( error )
  {
    std::cout << "Error: pthread_create() return code " << error << std::endl;
    exit(error);
  }

  /* 
    The following loop makes sure that:
      - the global variable is updated in the child thread before "nvml_stop" is called;
      - the output file is generated;
      - some data is written to the output file.
    In this way, we will always get some data even if the measured GPU kernel finishes too fast.
  */
  bool file_empty;
  file_empty = true;
  while (file_empty)
  {
    int result = pthread_mutex_trylock(&fileMutex);
    if (result == 0)
    {
      // Lock acquired, release immediately to avoid blocking other threads
      pthread_mutex_unlock(&fileMutex);
      if (verbose) std::cout << "Output file is not being written" << std::endl;
    }
    else
    {
      if (verbose) std::cout << "Output file is being written" << std::endl;
      file_empty = false;
    }
    usleep(time_interval);
  }
}

// End the NVML measurement
void nvml_stop()
{
  pollThreadStatus = false;
  // Wait for thread termination
  pthread_join(powerPollThread, NULL);

  nvmlResult = nvmlShutdown();
  check_status(nvmlResult);
}

void check_status(nvmlReturn_t nvmlResult)
{
  if ( nvmlResult != NVML_SUCCESS ){
    std::cout << nvmlErrorString(nvmlResult) << std::endl;
    exit(nvmlResult);
  }
}
