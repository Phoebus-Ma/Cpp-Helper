/**
 * main.cpp - cpu stress.
 * 
 * You can view the effect through the task manager。 
 * 
 * License - MIT.
*/

#include <iostream>
#include <Windows.h>

// It adds ((500 / 10) == 50%) extra load on top of the original CPU load.
#define CPU_LOADING             500
#define CPU_CORES               8


DWORD WINAPI
CpuStressHandler(LPVOID lpParam)
{
    DWORD oldTime = 0;

    while (1)
    {
        oldTime = GetTickCount();

        while ((GetTickCount() - oldTime) < CPU_LOADING);

        Sleep(1000 - CPU_LOADING);
    }

    return 0;
}

int main()
{
    HANDLE thdHandle[CPU_CORES];
    
    for (int i = 0; i < CPU_CORES; i++) {
        thdHandle[i] = CreateThread(NULL, 0, CpuStressHandler, NULL, 0, NULL);

        if (NULL == thdHandle[i])
            printf("Error in create thread: %d.\n", i);
    }

    printf("Press 'Ctrl + C' to quit.\n");

    WaitForMultipleObjects(CPU_CORES, thdHandle, TRUE, INFINITE);

    return 0;
}
