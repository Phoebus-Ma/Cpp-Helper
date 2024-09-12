/**
 * GPU Workload Example.
 * 
 * License - MIT.
*/

#include <Windows.h>
#include "opencl.hpp"

#define MAX_CALIBRATION_COUNT       100     // Calibration times.


UINT percentage = 0;


/**
 * GpuWorkload - Run GPU Workload.
*/
DWORD WINAPI GpuWorkload(LPVOID lpParam)
{
    UINT i = 0;
    UINT index = *(UINT *)lpParam;

    // kernel range: N * M * sizeof(float).
    const UINT N = 2048u * 2048u;
    
    // coalescence size.
    const UINT M = 16u;

    const string defines =
        "\n	#define def_N " + to_string(N) + "u"
        "\n	#define def_M " + to_string(M) + "u";

    if (percentage > 100)
        return 1;

    /* Get devices. */
    const vector<Device_Info> devices = get_devices(false);

    /* Display devices information. */
    for (int i = 0; i < devices.size(); i++)
    {
        print_device_info(devices[i]);
    }

    Device device(devices[index], defines + get_opencl_c_code());
    Memory<float> buffer(device, N, M);

    /**
     * kernel_double
     * kernel_float
     * kernel_half
     *
     * kernel_long
     * kernel_int
     * kernel_short
     * kernel_char
    */
    Kernel kernelApp(device, N, "kernel_float", buffer);

    /**
     * Calibration.
     * Get time of run 100 times.
    */
    ULONGLONG start_time = GetTickCount64();

    for (i = 0; i < MAX_CALIBRATION_COUNT; i++)
    {
        kernelApp.run();
    }

    UINT total_time = (UINT)(GetTickCount64() - start_time);

    UINT loops      = percentage * 10 * MAX_CALIBRATION_COUNT / total_time;
    UINT sleepTime  = (100 - percentage) * 10;

    /* Workload. */
    while (TRUE)
    {
        for (i = 0; i < loops; i++)
        {
            kernelApp.run();
        }

        Sleep(sleepTime);
    }

    return 0;
}

/**
 * How to use:
 * 
 * 1. GpuLoad.exe
 * 2. GpuLoad.exe [0|1|...] [0-100]
 * 
 * Example 1:
 * GpuLoad.exe
 * 
 * Example 2:
 * GpuLoad.exe 0 50
*/
int main(int argc, char **argv)
{
    UINT index = 0;
    percentage = 50;

    if (argc > 2)
    {
        index = atoi(argv[1]);
        percentage = atoi(argv[2]);
    }

    HANDLE handle = CreateThread(NULL, 0, GpuWorkload, &index, CREATE_SUSPENDED, NULL);

    if (!handle)
        return -1;

    ResumeThread(handle);
    Sleep(10000);

    /* Destroy. */
    percentage = 0;
    SuspendThread(handle);
    CloseHandle(handle);

    return 0;
}
