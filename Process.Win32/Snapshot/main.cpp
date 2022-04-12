/**
 * Win32 Process snapshot sample code.
 * ref: [https://docs.microsoft.com/en-us/windows/win32/toolhelp/taking-a-snapshot-and-viewing-processes].
 * 
 * License - MIT.
*/

#include <iostream>
#include <Windows.h>
#include <tlhelp32.h>


#define FIND_PROCESS_NAME                       L"SystemSettings.exe"


/**
 * ListProcessModules - Get process module.
*/
int ListProcessModules(DWORD dwProcessId)
{
    HANDLE hModuleSnap          = INVALID_HANDLE_VALUE;
    MODULEENTRY32 moduleEntry   = { 0 };
    moduleEntry.dwSize          = sizeof(MODULEENTRY32);

    /* Get snapshot. */
    hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);

    if (INVALID_HANDLE_VALUE == hModuleSnap)
    {
        printf("Error in CreateToolhelp32Snapshot");
        goto err_modsnap;
    }

    if (!Module32First(hModuleSnap, &moduleEntry))
    {
        printf("Error in Module32First.\n");
        goto err_modfirst;
    }

    do
    {
        printf("Module name:\t %ws.\n", moduleEntry.szModule);
        printf("Ref count:\t %d.\n",    moduleEntry.GlblcntUsage);
    } while (Module32Next(hModuleSnap, &moduleEntry));
    

    CloseHandle(hModuleSnap);

    return 0;

err_modfirst:
    CloseHandle(hModuleSnap);

err_modsnap:
    return -1;
}

/**
 * ListProcessThread - Get process thread list.
*/
int ListProcessThread(DWORD dwProcessId)
{
    HANDLE hThreadSnap          = INVALID_HANDLE_VALUE;
    THREADENTRY32 threadEntry   = { 0 };
    threadEntry.dwSize          = sizeof(THREADENTRY32);

    /* Get snapshot. */
    hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

    if (INVALID_HANDLE_VALUE == hThreadSnap)
    {
        printf("Error in CreateToolhelp32Snapshot.\n");
        goto err_thrdsnap;
    }

    if (!Thread32First(hThreadSnap, &threadEntry))
    {
        printf("Error in Thread32First.\n");
        goto err_thrdfirst;
    }

    do
    {
        if (dwProcessId == threadEntry.th32OwnerProcessID)
        {
            printf("---------------------------------------------\n");
            printf("Thread id:\t %d.\n",        threadEntry.th32ThreadID);
            printf("Thread priority:\t %d.\n",  threadEntry.tpBasePri);
        }
    } while (Thread32Next(hThreadSnap, &threadEntry));

    CloseHandle(hThreadSnap);

    return 0;

err_thrdfirst:
    CloseHandle(hThreadSnap);

err_thrdsnap:
    return -1;
}

/**
 * GetProcessInfo - Get specified process information.
*/
int GetProcessInfo()
{
    HANDLE hProcess             = NULL;
    HANDLE hProcessSnap         = INVALID_HANDLE_VALUE;
    DWORD dwPriorityClass       = 0;
    PROCESSENTRY32 processEntry = { 0 };
    processEntry.dwSize         = sizeof(PROCESSENTRY32);

    /* Get snapshot. */
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (INVALID_HANDLE_VALUE == hProcessSnap)
    {
        printf("Error in CreateToolhelp32Snapshot.\n");
        goto err_snap;
    }

    /* Retrieve information about the first process. */
    if (!Process32First(hProcessSnap, &processEntry))
    {
        printf("Error in Process32First.\n");
        goto err_first;
    }

    /* Walk the snapshot of processes. */
    do
    {
        if (!wcscmp(FIND_PROCESS_NAME, processEntry.szExeFile))
        {
            printf("---------------------------------------------\n");
            printf("Process name:\t %ws.\n",        processEntry.szExeFile);
            printf("Process id:\t %d.\n",           processEntry.th32ProcessID);
            printf("Process thread count:\t %d.\n", processEntry.cntThreads);

            hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processEntry.th32ProcessID);

            if (NULL == hProcess)
            {
                printf("Error in OpenProcess.\n");
            }
            else
            {
                dwPriorityClass = GetPriorityClass(hProcess);

                if (dwPriorityClass)
                    printf("%ws priority: %d.\n", FIND_PROCESS_NAME, dwPriorityClass);

                CloseHandle(hProcess);
            }

            ListProcessThread(processEntry.th32ProcessID);
            ListProcessModules(processEntry.th32ProcessID);

            break;
        }

    } while (Process32Next(hProcessSnap, &processEntry));

    CloseHandle(hProcessSnap);

	return 0;

err_first:
    CloseHandle(hProcessSnap);

err_snap:
	return -1;
}

/**
 * Main function.
*/
int main(void)
{
    GetProcessInfo();

    return 0;
}
