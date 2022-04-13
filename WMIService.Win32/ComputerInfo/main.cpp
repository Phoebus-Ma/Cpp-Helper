/**
 * Win32 get computer information by WMI service.
 * Ref 1: [https://docs.microsoft.com/en-us/windows/win32/wmisdk/example--getting-wmi-data-from-the-local-computer].
 * Ref 2: [https://docs.microsoft.com/en-us/windows/win32/cimwin32prov/computer-system-hardware-classes].
 * 
 * Example for Powershell:
 * [Get-WmiObject -Class Win32_Processor | select *].
 * 
 * License - MIT.
*/

#include <iostream>
#include <Windows.h>

#include "wmi-api.h"


#define MAX_DEVICE_COUNT                    4

#define WIN32_CPU_CLASS                     L"Win32_Processor"
#define CPU_ITEM_NAME                       L"Name"
#define CPU_ITEM_LOGIC_CORES                L"NumberOfLogicalProcessors"

#define WIN32_DISK_CLASS                    L"Win32_DiskDrive"
#define DISK_ITEM_SIZE                      L"Size"


using namespace std;


/**
 * GetCPUinfo - Get CPU information from wmi data.
*/
int GetCPUinfo()
{
    VARIANT cpuProp { 0 };

    wmi_query(WIN32_CPU_CLASS, CPU_ITEM_NAME, &cpuProp);
    printf("CPU Info: %ws.\n", cpuProp.bstrVal);

    wmi_query(WIN32_CPU_CLASS, CPU_ITEM_LOGIC_CORES, &cpuProp);
    printf("CPU logic cores: %d.\n", cpuProp.intVal);

    return 0;
}

/**
 * GetDiskInfo - Get Disk information from wmi data.
*/
int GetDiskInfo()
{
    int size = 0;
    int count = 0;
    VARIANT diskProps[4] = { 0 };

    count = wmi_query(WIN32_DISK_CLASS, DISK_ITEM_SIZE, diskProps);

    for (int i = 0; (i < count) && (i < MAX_DEVICE_COUNT); i++)
    {
        /**
         * GB  = 1000 MB.
         * GiB = 1024 MB.
        */
        size = (int)(_wtoi64(diskProps[i].bstrVal) / 1000 / 1000 / 1000);
        printf("Disk %d Size: %d GB.\n", i, size);
    }

    return 0;
}

/**
 * Main function.
*/
int main(void)
{
    wmi_init();

    GetCPUinfo();

    GetDiskInfo();

    wmi_clear();

    return 0;
}
