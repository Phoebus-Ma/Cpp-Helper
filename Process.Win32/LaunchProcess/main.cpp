/**
 * Win32 Process sample code.
 * ref: [https://docs.microsoft.com/en-us/windows/win32/procthread/creating-processes].
 * 
 * License - MIT.
*/

#include <iostream>
#include <Windows.h>


/**
 * LaunchAppTest - Launch app by Path.
*/
int
LaunchAppTest()
{
    int status = 0;
    WCHAR appPath[32] = L"C:\\Windows\\System32\\notepad.exe";

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));

    si.cb = sizeof(si);

    if (!CreateProcess(
        appPath,
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
        status = -1;
        printf("Error in CreateProcess %d.\n", GetLastError());
        goto err_cmd_create;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

err_cmd_create:
    return status;
}

/**
 * Main function.
*/
int
main(void)
{
    LaunchAppTest();

    return 0;
}
