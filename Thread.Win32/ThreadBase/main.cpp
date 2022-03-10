/**
 * Win32 Thread sample code.
 * 
 * License - MIT.
*/

#include <iostream>
#include <Windows.h>

/**
 * ThreadTestHandler - Thread callback function.
*/
DWORD WINAPI
ThreadTestHandler(LPVOID lpParam)
{
    int id = *(int *)lpParam;

    for (int i = 0; i < 5; i++)
    {
        std::cout << "ID: " << id << ", Loop: " << i << std::endl;

        Sleep(1000);
    }

    return 0;
}

/**
 * MultiThreadTest - Multi thread test.
*/
int
MultiThreadTest(int threadCount)
{
    int i = 0;
    int status = 0;

    HANDLE *pHandles = NULL;
    pHandles = new HANDLE[threadCount];

    /* Create thread. */
    for (i = 0; i < threadCount; i++)
    {
        pHandles[i] = CreateThread(
            NULL,
            0,
            ThreadTestHandler,
            &pHandles[i],
            CREATE_SUSPENDED,
            NULL);

        if (NULL == pHandles[i])
        {
            status = -1;
            std::cout << "Error in CreateThread.\n";
            goto err_mul_create;
        }
    }

    /* Awake all threads.. */
    for (i = 0; i < threadCount; i++)
        ResumeThread(pHandles[i]);

    /* Wait multi threads done.. */
    WaitForMultipleObjects(threadCount, pHandles, TRUE, INFINITE);

err_mul_create:
    /* Close handle. */
    for (i = 0; i < threadCount; i++)
    {
        if (NULL != pHandles[i])
            CloseHandle(pHandles[i]);
    }

    delete[] pHandles;

    return status;
}

/**
 * SingleThreadTest - Single thread test.
*/
int
SingleThreadTest()
{
    int id = 1;
    int status = 0;
    HANDLE thrdHandle = NULL;

    /* Create thread. */
    thrdHandle = CreateThread(
        NULL,
        0,
        ThreadTestHandler,
        &id,
        CREATE_SUSPENDED,
        NULL);

    if (NULL == thrdHandle)
    {
        status = -1;
        std::cout << "Error in CreateThread.\n";
        goto err_sin_create;
    }

    /* Awake thread. */
    ResumeThread(thrdHandle);

    /* Wait thread done. */
    WaitForSingleObject(thrdHandle, INFINITE);

    /* Close handle. */
    CloseHandle(thrdHandle);

err_sin_create:
    return status;
}

/**
 * Main function.
*/
int
main(void)
{
    std::cout << "single thread test.\n";
    SingleThreadTest();

    std::cout << "multi thread test.\n";
    MultiThreadTest(5);

    return 0;
}
