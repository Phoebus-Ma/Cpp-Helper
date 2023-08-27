/**
 * Win32 lzms compression algorithms.
 * Ref: [https://docs.microsoft.com/en-us/windows/win32/cmpapi/using-the-compression-api-in-block-mode].
 *
 * License - MIT.
 */

#ifndef __LZMS_H__
#define __LZMS_H__

#if _MSC_VER > 1000
#pragma once
#endif


#include <iostream>
#include <Windows.h>
#include <compressapi.h>


#define META_DATA_SIZE                  (2 * sizeof(ULONG))
#define BLOCK_SIZE                      (1 << 20)


int lzms_decompression(LPCWSTR lpCompressFile, LPCWSTR lpFileName);
int lzms_compression(LPCWSTR lpFileName, LPCWSTR lpCompressFile);


#endif /* __LZMS_H__ */
