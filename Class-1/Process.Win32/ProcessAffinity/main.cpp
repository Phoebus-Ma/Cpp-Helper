/**
 * Win32 set process affinity CPU.
 * Ref: [https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-setprocessaffinitymask].
 * Ref: [https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-setpriorityclass].
 * 
 * License- MIT.
*/

#include <iostream>
#include <Windows.h>

#include <WinBase.h>
#include <processthreadsapi.h>

using namespace std;

/**
 * SetProcPriority - Set process priority.
*/
int SetProcPriority(HANDLE &hProcess, DWORD dwPriorityClass)
{
    BOOL ret = FALSE;
    DWORD priorityClass = 0;

    ret = SetPriorityClass(hProcess, dwPriorityClass);

    if (FALSE == ret)
    {
        printf("Error in SetPriorityClass.\n");
        return -1;
    }

    priorityClass = GetPriorityClass(hProcess);

    if (priorityClass == dwPriorityClass)
        printf("Set priority 0x%x OK!\n", dwPriorityClass);

    return 0;
}

/**
 * SetProcAffinity - Set process use cpu affinity mask.
*/
int SetProcAffinity(HANDLE &hProcess, DWORD_PTR dwProcessAffinityMask)
{
    BOOL ret = FALSE;
    DWORD_PTR processAffinityMask;
    DWORD_PTR systemAffinityMask;

    ret = SetProcessAffinityMask(hProcess, dwProcessAffinityMask);

    if (FALSE == ret)
    {
        printf("Error in SetProcessAffinityMask.\n");
        return -1;
    }

    ret = GetProcessAffinityMask(hProcess, &processAffinityMask, &systemAffinityMask);

    if (FALSE == ret)
    {
        printf("Error in GetProcessAffinityMask.\n");
    }

    if (processAffinityMask == dwProcessAffinityMask)
        printf("Set process affinity mask %lld OK!\n", dwProcessAffinityMask);

    return 0;
}

/**
 * LaunchApplication - Launch Application.
*/
int LaunchApplication(HANDLE &hProcess, int &pid)
{
    wstring appPath = L"C:\\Windows\\System32\\notepad.exe";

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));

    si.cb = sizeof(si);

    if (!CreateProcess(
        appPath.c_str(),
        NULL,
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &si,
        &pi)
    )
    {
        printf("Error in CreateProcess %d.\n", GetLastError());

        pid         = 0;
        hProcess    = (void *)0;

        return -1;
    }

    pid         = pi.dwProcessId;
    hProcess    = pi.hProcess;

    return 0;
}

/**
 * Main function.
*/
int main(void)
{
    int pid = -1;
    HANDLE hProcess;

    /**
     * 00000001(b) == 1, affinity cpu id 0.
     * 00000010(b) == 2, affinity cpu id 1.
     * 00001111(b) == 15, affinity cpu id 0, 1, 2, 3.
     * 
     * The Process affinity cpu id 1.
    */
    DWORD_PTR dwProcessAffinityMask = 2;

    /**
     * winbase.h
     * 
     * #define NORMAL_PRIORITY_CLASS             0x00000020
     * #define IDLE_PRIORITY_CLASS               0x00000040
     * #define HIGH_PRIORITY_CLASS               0x00000080
    */
    DWORD dwPriorityClass = NORMAL_PRIORITY_CLASS;

    if (!LaunchApplication(hProcess, pid))
    {
        printf("Launch APP success, pid: %d.\n", pid);

        /**
         * Style 1: Use Handle.
        */
        dwProcessAffinityMask   = 2;
        dwPriorityClass         = NORMAL_PRIORITY_CLASS;
        SetProcAffinity(hProcess, dwProcessAffinityMask);
        SetProcPriority(hProcess, dwPriorityClass);

        /**
         * Style 2: Use PID.
        */
        HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

        dwProcessAffinityMask   = 1;
        dwPriorityClass         = IDLE_PRIORITY_CLASS;
        SetProcAffinity(hProc, dwProcessAffinityMask);
        SetProcPriority(hProc, dwPriorityClass);

        Sleep(1000);
        CloseHandle(hProc);
    }

    return 0;
}
