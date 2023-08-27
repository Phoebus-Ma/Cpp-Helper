/**
 * License - MIT.
 * 
 * Module Name:
 *      wmiService.h
 * 
 * Abstract:
 *      wmi service API header file.
*/


#ifndef __WMISERVICE_H__
#define __WMISERVICE_H__

#if _MSC_VER > 1000
#pragma once
#endif

#define _WIN32_DCOM
#include <iostream>
#include <vector>
#include <comdef.h>
#include <Wbemidl.h>


/**
 * Win32_OperatingSystem
 * Win32_ComputerSystem
 * Win32_BaseBoard
 * Win32_Processor
 * Win32_PhysicalMemory
 * Win32_VideoController
 * Win32_NetworkAdapter
 * Win32_DiskDrive
 * Win32_LogicalDisk
 * MSFT_PhysicalDisk
 * MSFT_Disk
 * Win32_PerfFormattedData_PerfOS_Processor
*/


class EasyWMIAPI {
private:
    IWbemLocator *pLoc = NULL;
    IWbemServices *pSvc = NULL;
    IEnumWbemClassObject* pEnumerator = NULL;

public:
    EasyWMIAPI() {
        pLoc = NULL;
        pSvc = NULL;
        pEnumerator = NULL;
    }

    ~EasyWMIAPI() {
        easy_wmi_clear();
    }

    int easy_wmi_init();
    int easy_wmi_clear();
    int easy_wmi_query(std::wstring wmiClassName, std::wstring itemId, LPVARIANT vtProp);
};

#endif /* __WMISERVICE_H__ */
