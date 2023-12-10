#include <iostream>
#include <nvml.h>

// declaration of functions
void nvml_init();
void nvml_stop();
void check_status(nvmlReturn_t nvmlResult);

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
  if (nvmlResult != NVML_SUCCESS)
  {
    std::cout << "NVML fails to init: "
	      << nvmlErrorString(nvmlResult) << std::endl;
    exit(nvmlResult);
  }

  // Count the number of GPUs available.
  nvmlResult = nvmlDeviceGetCount( &deviceCount );
  if (nvmlResult != NVML_SUCCESS)
  {
    std::cout << "NVML fails to query device count: "
	      << nvmlErrorString(nvmlResult) << std::endl;
    exit(nvmlResult);
  }

  for (int i = 0; i < deviceCount; i++)
  {
    // Get the device ID.
    nvmlResult = nvmlDeviceGetHandleByIndex( i, &nvmlDeviceID );
    if (nvmlResult != NVML_SUCCESS)
    {
      std::cout << "NVML fails to get handle for device " << i << " : "
	        << nvmlErrorString(nvmlResult) << std::endl;
      exit(nvmlResult);
    }

    // Get the name of the device.
    nvmlResult = nvmlDeviceGetName( nvmlDeviceID, deviceNameStr, name_length );
    if (nvmlResult != NVML_SUCCESS)
    {
      std::cout << "NVML fails to get name of device " << i << " : "
	        << nvmlErrorString(nvmlResult) << std::endl;
      exit(nvmlResult);
    }

    // Get PCI information of the device.
    nvmlResult = nvmlDeviceGetPciInfo( nvmlDeviceID, &nvmPCIInfo );
    if (nvmlResult != NVML_SUCCESS)
    {
      std::cout << "NVML fails to get PCI info of device " << i << " : "
                << nvmlErrorString(nvmlResult) << std::endl;
      exit(nvmlResult);
    }

    // Get the compute mode of the device which indicates CUDA capabilities.
    nvmlResult = nvmlDeviceGetComputeMode( nvmlDeviceID, &computeMode );
    if (nvmlResult != NVML_SUCCESS)
    {
      std::cout	<< "NVML fails to get compute mode for device " << i << " : "
	        << nvmlErrorString(nvmlResult) << std::endl;
      exit(nvmlResult);
    }
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
  // the information below is obtained from https://docs.nvidia.com/deploy/nvml-api/group__nvmlDeviceEnumvs.html#group__nvmlDeviceEnumvs_1g06fa9b5de08c6cc716fbf565e93dd3d0
  switch (nvmlResult) {
    case NVML_SUCCESS:
      // The operation was successful.
      break;
    case NVML_ERROR_UNINITIALIZED:
      std::cout << "NVML was not first initialized with nvmlInit()." << std::endl;
      break;
    case NVML_ERROR_INVALID_ARGUMENT:
      std::cout << "A supplied argument is invalid." << std::endl;
      break;
    case NVML_ERROR_NOT_SUPPORTED:
      std::cout << "The requested operation is not available on target device." << std::endl;
      break;
    case NVML_ERROR_NO_PERMISSION:
      std::cout << "The current user does not have permission for operation." << std::endl;
      break;
    case NVML_ERROR_ALREADY_INITIALIZED:
      std::cout << "Deprecated: Multiple initializations are now allowed through ref counting." << std::endl;
      break;
    case NVML_ERROR_NOT_FOUND:
      std::cout << "A query to find an object was unsuccessful." << std::endl;
      break;
    case NVML_ERROR_INSUFFICIENT_SIZE:
      std::cout << "An input argument is not large enough." << std::endl;
      break;
    case NVML_ERROR_INSUFFICIENT_POWER:
      std::cout << "A device's external power cables are not properly attached." << std::endl;
      break;
    case NVML_ERROR_DRIVER_NOT_LOADED:
      std::cout << "NVIDIA driver is not loaded." << std::endl;
      break;
    case NVML_ERROR_TIMEOUT:
      std::cout << "User provided timeout passed." << std::endl;
      break;
    case NVML_ERROR_IRQ_ISSUE:
      std::cout << "NVIDIA Kernel detected an interrupt issue with a GPU." << std::endl;
      break;
    case NVML_ERROR_LIBRARY_NOT_FOUND:
      std::cout << "NVML Shared Library couldn't be found or loaded." << std::endl;
      break;
    case NVML_ERROR_FUNCTION_NOT_FOUND:
      std::cout << "Local version of NVML doesn't implement this function." << std::endl;
      break;
    case NVML_ERROR_CORRUPTED_INFOROM:
      std::cout << "infoROM is corrupted." << std::endl;
      break;
    case NVML_ERROR_GPU_IS_LOST:
      std::cout << "The GPU has fallen off the bus or has otherwise become inaccessible." << std::endl;
      break;
    case NVML_ERROR_RESET_REQUIRED:
      std::cout << "The GPU requires a reset before it can be used again." << std::endl;
      break;
    case NVML_ERROR_OPERATING_SYSTEM:
      std::cout << "The GPU control device has been blocked by the operating system/cgroups." << std::endl;
      break;
    case NVML_ERROR_LIB_RM_VERSION_MISMATCH:
      std::cout << "RM detects a driver/library version mismatch." << std::endl;
      break;
    case NVML_ERROR_IN_USE:
      std::cout << "An operation cannot be performed because the GPU is currently in use." << std::endl;
      break;
    case NVML_ERROR_MEMORY:
      std::cout << "Insufficient memory." << std::endl;
      break;
    case NVML_ERROR_NO_DATA:
      std::cout << "No data." << std::endl;
      break;
    case NVML_ERROR_VGPU_ECC_NOT_SUPPORTED:
      std::cout << "The requested vgpu operation is not available on target device, becasue ECC is enabled." << std::endl;
      break;
    case NVML_ERROR_INSUFFICIENT_RESOURCES:
      std::cout << "Ran out of critical resources, other than memory." << std::endl;
      break;
    case NVML_ERROR_FREQ_NOT_SUPPORTED:
      std::cout << "Ran out of critical resources, other than memory." << std::endl;
      break;
    default:
      std::cout << "Unknown error." << std::endl;
  }
  if (nvmlResult != NVML_SUCCESS) exit(nvmlResult);
}
