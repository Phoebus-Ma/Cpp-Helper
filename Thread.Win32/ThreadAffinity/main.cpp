/**
 * Win32 set thread cpu affinity example.
 * Ref 1: [https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-setthreadaffinitymask].
 * Ref 2: [https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-setthreadpriority].
 * 
 * License - MIT.
*/

#include <iostream>
#include <windows.h>

#include <winBase.h>
#include <processthreadsapi.h>

using namespace std;


/**
 * SetThrdPriority - Set thread priority.
*/
int SetThrdPriority(HANDLE &hThread, int nPriority)
{
    BOOL ret = FALSE;
    int threadPriority = 0;

    ret = SetThreadPriority(hThread, nPriority);

    if (FALSE == ret)
    {
        printf("Error in SetThreadPriority.\n");
        return -1;
    }

    threadPriority = GetThreadPriority(hThread);

    if (threadPriority == nPriority)
        printf("Set thread priority %d OK!\n", nPriority);

    return 0;
}

/**
 * SetThrdAffinity - Set thread use cpu affinity mask.
*/
int SetThrdAffinity(HANDLE &hThread, DWORD_PTR dwThreadAffinityMask)
{
    DWORD_PTR ret = 0;

    /**
     * There are no Win32 call GetThreadAffinityMask,
     * but there's function SetThreadAffinityMask that's use AffinityMask value.
    */
    ret = SetThreadAffinityMask(hThread, dwThreadAffinityMask);

    if (0 == ret)
    {
        printf("Error in SetThreadAffinityMask.\n");
        return -1;
    }

    printf("Set thread affinity mask %lld OK!\n", dwThreadAffinityMask);

    return 0;
}

/**
 * LaunchApplication - Launch Application.
*/
int LaunchApplication(HANDLE &hThread, int &tid)
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

        tid     = 0;
        hThread = (void *)0;

        return -1;
    }

    tid     = pi.dwThreadId;
    hThread = pi.hThread;

    return 0;
}

/**
 * Main function.
*/
int main(void)
{
    int tid = -1;
    HANDLE hThread;

    /**
     * 00000001(b) == 1, affinity cpu id 0.
     * 00000010(b) == 2, affinity cpu id 1.
     * 00001111(b) == 15, affinity cpu id 0, 1, 2, 3.
     * 
     * The Process affinity cpu id 1.
    */
    DWORD_PTR dwThreadAffinityMask = 2;

    /**
     * winbase.h
     * 
     * #define THREAD_PRIORITY_NORMAL          0
     * #define THREAD_PRIORITY_IDLE            THREAD_BASE_PRIORITY_IDLE
    */
    int nPriority = THREAD_PRIORITY_NORMAL;

    if (!LaunchApplication(hThread, tid))
    {
        printf("Launch APP success, tid: %d.\n", tid);

        /**
         * Style 1: Use Handle.
        */
        dwThreadAffinityMask    = 2;
        nPriority               = THREAD_PRIORITY_NORMAL;
        SetThrdAffinity(hThread, dwThreadAffinityMask);
        SetThrdPriority(hThread, nPriority);

        /**
         * Style 2: Use TID.
        */
        HANDLE hThrd = OpenThread(THREAD_ALL_ACCESS, FALSE, tid);

        dwThreadAffinityMask    = 1;
        nPriority               = THREAD_PRIORITY_IDLE;
        SetThrdAffinity(hThrd, dwThreadAffinityMask);
        SetThrdPriority(hThrd, nPriority);

        Sleep(1000);
        CloseHandle(hThrd);
    }

    return 0;
}
