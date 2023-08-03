/**
 * CpuUsageMonitor - Monitor CPU usage via WMI.
 * 
 * License - MIT.
*/

#include <iostream>
#include <Windows.h>

#include "wmiService.h"

#define MAX_CPU_CORES           (32 + 1)            // 32 cores + 1 total.

#define CLASS_NAME              L"Win32_PerfFormattedData_PerfOS_Processor"
#define INSTANCE_NAME           L"PercentIdleTime"

/**
 * main - program entry.
*/
int main(void)
{
    VARIANT usageProp[MAX_CPU_CORES] = { 0 };
    VARIANT usageProp2[MAX_CPU_CORES] = { 0 };
    EasyWMIAPI easyApi;

    easyApi.easy_wmi_init();

    printf("\n\nPress 'Ctrl + C' to quit.\n\n");

    while (1)
    {
        int count = easyApi.easy_wmi_query(CLASS_NAME, INSTANCE_NAME, usageProp);

        if (count <= MAX_CPU_CORES)
            printf("%d %% \n", 100 - _wtoi(usageProp[count - 1].bstrVal));

        Sleep(3000);
    }

    easyApi.easy_wmi_clear();

    return 0;
}
