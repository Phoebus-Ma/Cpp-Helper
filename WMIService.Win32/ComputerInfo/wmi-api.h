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

#ifndef __WMI_API_H__
#define __WMI_API_H__

#if _MSC_VER > 1000
#pragma once
#endif


#define _WIN32_DCOM

#include <iostream>
#include <comdef.h>
#include <Wbemidl.h>

using namespace std;


int wmi_query(wstring wmiClassName, wstring itemKey, LPVARIANT vtProps);
int wmi_clear(void);
int wmi_init(void);

#endif /* __WMI_API_H__ */
