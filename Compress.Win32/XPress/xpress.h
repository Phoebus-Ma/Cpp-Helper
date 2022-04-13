/**
 * Win32 xpress compression algorithms.
 * Ref: [https://docs.microsoft.com/en-us/windows/win32/cmpapi/using-the-compression-api-in-buffer-mode].
 *
 * License - MIT.
 */

#ifndef __XPRESS_H__
#define __XPRESS_H__

#if _MSC_VER > 1000
#pragma once
#endif


#include <iostream>
#include <Windows.h>
#include <compressapi.h>


int xpress_decompression(LPCWSTR lpCompressFile, LPCWSTR lpFileName);
int xpress_compression(LPCWSTR lpFileName, LPCWSTR lpCompressFile);


#endif /* __XPRESS_H__ */
