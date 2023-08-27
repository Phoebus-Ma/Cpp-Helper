/**
 * main.cpp - memory stress.
 * 
 * You can view the effect through the task manager.
 * 
 * License - MIT.
*/

#include <iostream>
#include <signal.h>
#include <Windows.h>


#define THREAD_COUNT            16
#define TOTAL_MEMORY            1024        // MiB
#define CAPACITY_PER_SINGLE     65536       // KiB


#ifdef USE_MALLOC

#define AllocApi(size)          malloc(size)
#define FreeApi(ptr)            free(ptr)
#else   /* USE_MALLOC */

#define AllocApi(size)          _aligned_malloc((size), 32)
#define FreeApi(ptr)            _aligned_free(ptr)
#endif  /* USE_MALLOC */


bool quitEvent = false;

/**
 * The requested memory is virtual memory, and if the virtual memory is not
 * read or written, no space is allocated or the allocated space is revoked.
*/
DWORD WINAPI
MemoryStressHandler(LPVOID lpParam)
{
    int i = 0;
    UINT bitToggle = 0xffffffff;

    char** ptrs = (char**)malloc(sizeof(void*) * TOTAL_MEMORY);

    Sleep(100);

    for (i = 0; i < TOTAL_MEMORY; i++)
    {
        char* ptr = (char*)AllocApi(CAPACITY_PER_SINGLE);

        if (ptr)
        {
            ptrs[i] = ptr;
        }
        else
        {
            ptrs[i] = nullptr;
            printf("Error in AllocApi id: %d.\n", i);
        }
    }

    while (!quitEvent)
    {
        bitToggle = ~bitToggle;

        for (i = 0; i < TOTAL_MEMORY; i++)
        {
            if (ptrs[i])
            {
                memset(ptrs[i], bitToggle, CAPACITY_PER_SINGLE);
            }
        }

        Sleep(4000);
    }

    for (i = 0; i < TOTAL_MEMORY; i++)
    {
        if (ptrs[i])
        {
            FreeApi(ptrs[i]);
            ptrs[i] = nullptr;
        }
    }

    free(ptrs);

    return 0;
}

void SignalHandler(int s)
{
    if (SIGINT == s) {
        printf("It will exit...\n");
        quitEvent = true;
    }
}

int main()
{
    HANDLE thdHandle[THREAD_COUNT];

    signal(SIGINT, SignalHandler);
    
    for (int i = 0; i < THREAD_COUNT; i++) {
        thdHandle[i] = CreateThread(NULL, 0, MemoryStressHandler, NULL, 0, NULL);

        if (NULL == thdHandle[i])
            printf("Error in create thread: %d.\n", i);
    }

    printf("Press 'Ctrl + C' to quit.\n");

    WaitForMultipleObjects(THREAD_COUNT, thdHandle, TRUE, INFINITE);

    return 0;
}
