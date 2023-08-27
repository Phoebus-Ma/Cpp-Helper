/**
 * Win32 mszip compression algorithms.
 * Ref: [https://docs.microsoft.com/en-us/windows/win32/cmpapi/using-the-compression-api-in-buffer-mode].
 *
 * License - MIT.
 */

#ifndef __MSZIP_H__
#define __MSZIP_H__

#if _MSC_VER > 1000
#pragma once
#endif


#include <iostream>
#include <Windows.h>
#include <compressapi.h>


int mszip_decompression(LPCWSTR lpCompressFile, LPCWSTR lpFileName);
int mszip_compression(LPCWSTR lpFileName, LPCWSTR lpCompressFile);


#endif /* __MSZIP_H__ */

