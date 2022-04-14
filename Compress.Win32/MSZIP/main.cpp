/**
 * Win32 mszip compression algorithms.
 * Ref: [https://docs.microsoft.com/en-us/windows/win32/cmpapi/using-the-compression-api-in-buffer-mode].
 *
 * License - MIT.
 */

#include <iostream>
#include <Windows.h>
#include <compressapi.h>

#include "mszip.h"


#define FILE_PATH               L"C:\\Windows\\System32\\shell32.dll"
#define COMPRESS_FILE           L"shell32.cab"
#define DECOMPRESS_FILE         L"shell32.dll"


/**
 * Main function.
*/
int main(void)
{
    printf("Start compress file.\n");
    mszip_compression(FILE_PATH, COMPRESS_FILE);

    printf("\nStart decompress file.\n");
    mszip_decompression(COMPRESS_FILE, DECOMPRESS_FILE);

    return 0;
}
