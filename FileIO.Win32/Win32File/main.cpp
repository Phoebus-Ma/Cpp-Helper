/**
 * Win32 File IO sample code.
 * ref: [https://docs.microsoft.com/en-us/windows/win32/fileio/opening-a-file-for-reading-or-writing].
 *
 * License - MIT.
*/

#include <iostream>
#include <windows.h>

#define BUFFERSIZE                  1024
#define TESTFILE                    L"test.txt"


using namespace std;


/**
 * file_read - Read data from file.
*/
int file_read(LPWSTR fileName)
{
    char ReadBuffer[BUFFERSIZE] = { 0 };

    HANDLE hFile = CreateFile(
        fileName,               // file to open
        GENERIC_READ,           // open for reading
        FILE_SHARE_READ,        // share for reading
        NULL,                   // default security
        OPEN_EXISTING,          // existing file only
        FILE_ATTRIBUTE_NORMAL,  // normal file
        NULL                    // no attr. template
    );

    if (INVALID_HANDLE_VALUE == hFile)
    {
        cout << "Error in CreateFile.\n";
        goto err_create_r;
    }

    if (FALSE == ReadFile(hFile, ReadBuffer, BUFFERSIZE - 1, NULL, NULL))
    {
        cout << "Error in ReadFile.\n";
        goto err_read;
    }

    printf("%s\n", ReadBuffer);

    CloseHandle(hFile);

    return 0;

err_read:
    CloseHandle(hFile);

err_create_r:
    return -1;
}


/**
 * file_write - Write data to file.
*/
int file_write(LPWSTR fileName)
{
    char DataBuffer[] = "To be NO.1.\n";

    HANDLE hFile = CreateFile(
        fileName,                   // name of the write
        GENERIC_WRITE,              // open for writing
        0,                          // do not share
        NULL,                       // default security
        CREATE_ALWAYS,                // create new file only
        FILE_ATTRIBUTE_NORMAL,      // normal file
        NULL                        // no attr. template
    );

    if (INVALID_HANDLE_VALUE == hFile)
    { 
        cout << "Error in CreateFile.\n";
        goto err_create_w;
    }

    if (FALSE == WriteFile( 
        hFile,                      // open file handle
        DataBuffer,                 // start of data to write
        (DWORD)strlen(DataBuffer),  // number of bytes to write
        NULL,                       // number of bytes that were written
        NULL                        // no overlapped structure
    ))
    {
        cout << "Error in WriteFile.\n";
        goto err_write;
    }

    cout << "Wrote successed.\n";

    CloseHandle(hFile);

    return 0;

err_write:
    CloseHandle(hFile);

err_create_w:
    return -1;
}


/**
 * Main function.
*/
int main(void)
{
    wchar_t fileName[] = TESTFILE;

    /* Example 1: Write file. */
    file_write(fileName);

    /* Example 2: Read file. */
    file_read(fileName);

    return 0;
}

