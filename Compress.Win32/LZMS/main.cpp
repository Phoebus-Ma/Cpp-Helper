/**
 * Win32 lzms compression algorithms.
 * Ref: [https://docs.microsoft.com/en-us/windows/win32/cmpapi/using-the-compression-api-in-block-mode].
 *
 * License - MIT.
 */

#include <iostream>
#include <Windows.h>
#include <compressapi.h>

#include "lzms.h"


#define FILE_PATH               L"C:\\Windows\\System32\\shell32.dll"
#define COMPRESS_FILE           L"shell32.wim"
#define DECOMPRESS_FILE         L"shell32.dll"


/**
 * Main function.
*/
int main(void)
{
    printf("Start compress file.\n");
    lzms_compression(FILE_PATH, COMPRESS_FILE);

    printf("\nStart decompress file.\n");
    lzms_decompression(COMPRESS_FILE, DECOMPRESS_FILE);

    return 0;
}
