#include <iostream>
#include <nvml.h>

// declaration of functions
extern "C" {
  void nvml_init();
  void nvml_stop();
  void check_status(nvmlReturn_t nvmlResult);
}

// define variables for NVML APIs
unsigned int deviceCount = 0;
const int name_length = 64;
char deviceNameStr[name_length];
nvmlReturn_t nvmlResult;
nvmlDevice_t nvmlDeviceID;
nvmlPciInfo_t nvmPCIInfo;
nvmlEnableState_t pmmode;
nvmlComputeMode_t computeMode;

void nvml_init()
{
  // Initialize nvml.
  nvmlResult = nvmlInit_v2();
  check_status(nvmlResult);

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

    // Get PCI information of the device.
    nvmlResult = nvmlDeviceGetPciInfo( nvmlDeviceID, &nvmPCIInfo );
    check_status(nvmlResult);

    // Get the compute mode of the device which indicates CUDA capabilities.
    nvmlResult = nvmlDeviceGetComputeMode( nvmlDeviceID, &computeMode );
    check_status(nvmlResult);
  }
}

void nvml_stop()
{
  nvmlResult = nvmlShutdown();
  if (nvmlResult != NVML_SUCCESS)
  {
    std::cout << "NVML fails to shut down : " << nvmlErrorString(nvmlResult) << std::endl;
    exit(nvmlResult);
  }
}

void check_status(nvmlReturn_t nvmlResult)
{
  if ( nvmlResult != NVML_SUCCESS ){
    std::cout << nvmlErrorString(nvmlResult) << std::endl;
    exit(nvmlResult);
  }
}
