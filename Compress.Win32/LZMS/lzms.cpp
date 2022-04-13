/**
 * Win32 lzms compression algorithms.
 * Ref: [https://docs.microsoft.com/en-us/windows/win32/cmpapi/using-the-compression-api-in-block-mode].
 *
 * License - MIT.
 */

#include "lzms.h"

#pragma comment(lib, "Cabinet.lib")

/**
 * SimpleAlloc - Allocate memory.
 */
PVOID SimpleAlloc(PVOID Context, SIZE_T Size)
{
    UNREFERENCED_PARAMETER(Context);
    return malloc(Size);
}

/**
 * SimpleFree - Free memory.
 */
VOID SimpleFree(PVOID Context, PVOID Memory)
{
    UNREFERENCED_PARAMETER(Context);

    if (NULL != Memory)
    {
        free(Memory);
    }

    return;
}

/**
 * BlockModeCompress - Block mode compress.
 */
BOOL BlockModeCompress(
    _In_ PBYTE InputData,
    _In_ DWORD InputSize,
    _Deref_out_opt_ PBYTE *OutputData,
    _Out_ DWORD *CompressedSize)
{
    COMPRESSOR_HANDLE Compressor    = NULL;
    DWORD ProcessedSoFar            = 0;
    SIZE_T OutputSoFar              = 0;
    DWORD CurrentBlockSize          = 0;
    SIZE_T CompressedDataSize       = 0;
    SIZE_T CompressedBlockSize      = 0;
    SIZE_T OutputDataSize           = 0;
    BOOL Success                    = FALSE;

    /* Set maximum input block size for compressor. */
    DWORD BlockSize = BLOCK_SIZE;

    COMPRESS_ALLOCATION_ROUTINES AllocationRoutines;

    /* Init. allocation routines. */
    AllocationRoutines.Allocate = SimpleAlloc;
    AllocationRoutines.Free = SimpleFree;
    AllocationRoutines.UserContext = NULL;

    *CompressedSize = 0;
    *OutputData = NULL;

    /* Create a LZMS compressor and set to Block mode. */
    Success = CreateCompressor(
        COMPRESS_ALGORITHM_LZMS | COMPRESS_RAW, // Compression algorithm is LZMS
        &AllocationRoutines,                    // Optional Memory allocation routines
        &Compressor);                           // Handle

    if (!Success)
    {
        wprintf(L"Cannot create compressor handle: %d\n", GetLastError());
        goto done;
    }

    Success = SetCompressorInformation(
        Compressor,
        COMPRESS_INFORMATION_CLASS_BLOCK_SIZE, // Set block size for LZMS compressor
        &BlockSize,                            // Block size information
        sizeof(DWORD));                        // Information size

    if (!Success)
    {
        wprintf(L"Set compressor information error: %d\n", GetLastError());
        goto done;
    }

    /* Query max. possible compressed block size. */
    Success = Compress(
        Compressor,            // Compressor Handle
        NULL,                  // Input buffer, Uncompressed data
        BlockSize,             // Uncompressed block size
        NULL,                  // Compressed Buffer
        0,                     // Compressed Buffer size
        &CompressedBlockSize); // Compressed Data size

    if (!Success)
    {
        DWORD ErrorCode = GetLastError();
        if (ErrorCode != ERROR_INSUFFICIENT_BUFFER)
        {
            wprintf(L"Query compressed block size error: %d\n", GetLastError());
            goto done;
        }
    }

    /* Get max. possible size for compressed data for given input data. */
    OutputDataSize = (InputSize % BLOCK_SIZE == 0) ? 0 : 1;
    OutputDataSize += InputSize / BLOCK_SIZE;
    OutputDataSize = OutputDataSize * (META_DATA_SIZE + CompressedBlockSize) + sizeof(ULONG);

    *OutputData = (PBYTE)malloc(OutputDataSize);
    if (!*OutputData)
    {
        wprintf(L"Cannot allocate memory for compressed buffer.\n");
        Success = FALSE;
        goto done;
    }

    /* Write uncompressed size to beginning of the buffer. */
    *((ULONG UNALIGNED *)*OutputData) = InputSize;
    OutputSoFar = sizeof(ULONG);

    /* Compress data block by block. */
    while (ProcessedSoFar < InputSize)
    {
        if (OutputSoFar + META_DATA_SIZE >= OutputDataSize)
        {
            Success = FALSE;
            wprintf(L"Compression fails.\n");
            goto done;
        }

        CurrentBlockSize =
            (InputSize - ProcessedSoFar < BlockSize) ? (InputSize - ProcessedSoFar) : BlockSize;

        /* Compress a block. */
        Success = Compress(
            Compressor,                                    // Compressor Handle
            InputData + ProcessedSoFar,                    // Uncompressed data
            CurrentBlockSize,                              // Uncompressed data size
            *OutputData + OutputSoFar + META_DATA_SIZE,    // Start of compressed buffer
            OutputDataSize - OutputSoFar - META_DATA_SIZE, // Compressed block size
            &CompressedDataSize);                          // Compressed data size

        if (!Success)
        {
            wprintf(L"Compression fails: %d\n", GetLastError());
            goto done;
        }

        /* Write block information to output data. */
        *((ULONG UNALIGNED *)(*OutputData + OutputSoFar)) = (ULONG)CompressedDataSize;
        OutputSoFar += sizeof(ULONG);
        *((ULONG UNALIGNED *)(*OutputData + OutputSoFar)) = (ULONG)CurrentBlockSize;
        OutputSoFar += sizeof(ULONG);
        OutputSoFar += CompressedDataSize;

        ProcessedSoFar += CurrentBlockSize;
    }

    if (OutputSoFar > UINT32_MAX)
    {
        *CompressedSize = 0;
        Success = FALSE;
    }
    else
    {
        *CompressedSize = static_cast<DWORD>(OutputSoFar);
    }

done:
    if (Compressor != NULL)
    {
        CloseCompressor(Compressor);
    }
    return Success;
}

/**
 * BlockModeDecompress - Block mode uncompress.
 */
BOOL BlockModeDecompress(
    _In_ PBYTE InputData,
    _In_ DWORD InputSize,
    _Deref_out_opt_ PBYTE *OutputData,
    _Out_ DWORD *DecompressedSize)
{
    DECOMPRESSOR_HANDLE Decompressor    = NULL;
    DWORD ProcessedSoFar                = 0;
    DWORD CompressedBlockSize           = 0;
    DWORD UncompressedBlockSize         = 0;
    DWORD DecompressedSoFar             = 0;
    DWORD OutputDataSize                = 0;
    BOOL Success                        = FALSE;

    COMPRESS_ALLOCATION_ROUTINES AllocationRoutines;

    /* Init. allocation routines */
    AllocationRoutines.Allocate = SimpleAlloc;
    AllocationRoutines.Free = SimpleFree;
    AllocationRoutines.UserContext = NULL;

    *DecompressedSize = 0;
    *OutputData = NULL;

    /* Create a LZMS decompressor and set to Block mode. */
    Success = CreateDecompressor(
        COMPRESS_ALGORITHM_LZMS | COMPRESS_RAW, // Compression algorithm is LZMS
        &AllocationRoutines,                    // Memory allocation routines
        &Decompressor);                         // handle

    if (!Success)
    {
        wprintf(L"Cannot create decompressor handle: %d\n", GetLastError());
        goto done;
    }

    /* Read uncompressed size. */
    ProcessedSoFar = 0;
    OutputDataSize = *((ULONG UNALIGNED *)(InputData + ProcessedSoFar));
    ProcessedSoFar += sizeof(ULONG);

    *OutputData = (PBYTE)malloc(OutputDataSize);
    if (!*OutputData)
    {
        wprintf(L"Cannot allocate memory for uncompressed buffer.\n");
        Success = FALSE;
        goto done;
    }

    /* Decompress data block by block. */
    while (ProcessedSoFar < InputSize)
    {
        if (ProcessedSoFar + META_DATA_SIZE > InputSize)
        {
            Success = FALSE;
            wprintf(L"Data corrupt.\n");
            goto done;
        }

        /* Read block information. */
        CompressedBlockSize = *((ULONG UNALIGNED *)(InputData + ProcessedSoFar));
        ProcessedSoFar += sizeof(ULONG);
        UncompressedBlockSize = *((ULONG UNALIGNED *)(InputData + ProcessedSoFar));
        ProcessedSoFar += sizeof(ULONG);

        if (ProcessedSoFar + CompressedBlockSize > InputSize)
        {
            Success = FALSE;
            wprintf(L"Data corrupt.\n");
            goto done;
        }

        if (DecompressedSoFar + UncompressedBlockSize > OutputDataSize)
        {
            Success = FALSE;
            wprintf(L"Output buffer not enough to hold decompressed data.\n");
            goto done;
        }

        /* Decompress a block. */
        Success = Decompress(
            Decompressor,                    // Decompressor Handle
            InputData + ProcessedSoFar,      // Compressed data
            CompressedBlockSize,             // compressed data size
            *OutputData + DecompressedSoFar, // Start of decompressed buffer
            UncompressedBlockSize,           // Uncompressed block size
            NULL);                           // Decompressed data size
        if (!Success)
        {
            wprintf(L"Decompression failure: %d\n", GetLastError());
            goto done;
        }
        ProcessedSoFar += CompressedBlockSize;
        DecompressedSoFar += UncompressedBlockSize;
    }

    *DecompressedSize = DecompressedSoFar;

done:
    if (Decompressor != NULL)
    {
        CloseDecompressor(Decompressor);
    }
    return Success;
}

/**
 * lzms_decompression - LZMS decompression algorithms.
 */
int lzms_decompression(LPCWSTR lpCompressFile, LPCWSTR lpFileName)
{
    PBYTE CompressedBuffer      = NULL;
    PBYTE DecompressedBuffer    = NULL;
    HANDLE InputFile            = INVALID_HANDLE_VALUE;
    HANDLE DecompressedFile     = INVALID_HANDLE_VALUE;
    BOOL DeleteTargetFile       = TRUE;
    BOOL Success;
    DWORD DecompressedDataSize;
    DWORD InputFileSize, ByteRead, ByteWritten;
    ULONGLONG StartTime, EndTime;
    LARGE_INTEGER FileSize;
    double TimeDuration;

    /* Open input file for reading, existing file only. */
    InputFile = CreateFile(
        lpCompressFile,        // Input file name, compressed file
        GENERIC_READ,          // Open for reading
        FILE_SHARE_READ,       // Share for read
        NULL,                  // Default security
        OPEN_EXISTING,         // Existing file only
        FILE_ATTRIBUTE_NORMAL, // Normal file
        NULL);                 // No template

    if (InputFile == INVALID_HANDLE_VALUE)
    {
        wprintf(L"Cannot open \t%s\n", lpCompressFile);
        goto done;
    }

    /* Get compressed file size. */
    Success = GetFileSizeEx(InputFile, &FileSize);
    if ((!Success) || (FileSize.QuadPart > 0xFFFFFFFF))
    {
        wprintf(L"Cannot get input file size or file is larger than 4GB.\n");
        goto done;
    }
    InputFileSize = FileSize.LowPart;

    /* Allocate memory for compressed content. */
    CompressedBuffer = (PBYTE)malloc(InputFileSize);
    if (!CompressedBuffer)
    {
        wprintf(L"Cannot allocate memory for compressed buffer.\n");
        goto done;
    }

    /* Read compressed content into buffer. */
    Success = ReadFile(InputFile, CompressedBuffer, InputFileSize, &ByteRead, NULL);
    if ((!Success) || (ByteRead != InputFileSize))
    {
        wprintf(L"Cannot read from \t%s\n", lpCompressFile);
        goto done;
    }

    /* Open an empty file for writing, if exist, destroy it. */
    DecompressedFile = CreateFile(
        lpFileName,             // Decompressed file name
        GENERIC_WRITE | DELETE, // Open for writing
        0,                      // Do not share
        NULL,                   // Default security
        CREATE_ALWAYS,          // Create a new file, if exists, overwrite it.
        FILE_ATTRIBUTE_NORMAL,  // Normal file
        NULL);                  // No template

    if (DecompressedFile == INVALID_HANDLE_VALUE)
    {
        wprintf(L"Cannot create file \t%s\n", lpFileName);
        goto done;
    }

    StartTime = GetTickCount64();

    /* Decompress data and write data to DecompressedBuffer. */
    Success = BlockModeDecompress(
        CompressedBuffer,       // Compressed data
        InputFileSize,          // Compressed data size
        &DecompressedBuffer,    // Decompressed buffer
        &DecompressedDataSize); // Decompressed data size

    if (!Success)
    {
        goto done;
    }

    EndTime = GetTickCount64();

    /* Get decompression time. */
    TimeDuration = (EndTime - StartTime) / 1000.0;

    /* Write decompressed data to output file. */
    Success = WriteFile(
        DecompressedFile,     // File handle
        DecompressedBuffer,   // Start of data to write
        DecompressedDataSize, // Number of byte to write
        &ByteWritten,         // Number of byte written
        NULL);                // No overlapping structure
    if ((ByteWritten != DecompressedDataSize) || (!Success))
    {
        wprintf(L"Cannot write decompressed data to file.\n");
        goto done;
    }

    wprintf(
        L"Compressed size: %d; Decompressed Size: %d\n",
        InputFileSize,
        DecompressedDataSize);
    wprintf(L"Decompression Time(Exclude I/O): %.2f seconds\n", TimeDuration);
    wprintf(L"File decompressed.\n");

    DeleteTargetFile = FALSE;

done:
    if (CompressedBuffer)
    {
        free(CompressedBuffer);
    }

    if (DecompressedBuffer)
    {
        free(DecompressedBuffer);
    }

    if (InputFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(InputFile);
    }

    if (DecompressedFile != INVALID_HANDLE_VALUE)
    {
        /* Compression fails, delete the compressed file. */
        if (DeleteTargetFile)
        {
            FILE_DISPOSITION_INFO fdi;
            fdi.DeleteFile = TRUE;                  // Marking for deletion
            Success = SetFileInformationByHandle(
                DecompressedFile,
                FileDispositionInfo,
                &fdi,
                sizeof(FILE_DISPOSITION_INFO));
            if (!Success)
            {
                wprintf(L"Cannot delete corrupted decompressed file.\n");
            }
        }

        CloseHandle(DecompressedFile);
    }

    return 0;
}

/**
 * lzms_compression - LZMS compression algorithms.
 */
int lzms_compression(LPCWSTR lpFileName, LPCWSTR lpCompressFile)
{
    PBYTE CompressedBuffer  = NULL;
    PBYTE InputBuffer       = NULL;
    HANDLE InputFile        = INVALID_HANDLE_VALUE;
    HANDLE CompressedFile   = INVALID_HANDLE_VALUE;
    BOOL DeleteTargetFile   = TRUE;
    BOOL Success;
    DWORD CompressedDataSize;
    DWORD InputFileSize, ByteRead, ByteWritten;
    LARGE_INTEGER FileSize;
    ULONGLONG StartTime, EndTime;
    double TimeDuration;

    /* Open input file for reading, existing file only. */
    InputFile = CreateFile(
        lpFileName,            // Input file name
        GENERIC_READ,          // Open for reading
        FILE_SHARE_READ,       // Share for read
        NULL,                  // Default security
        OPEN_EXISTING,         // Existing file only
        FILE_ATTRIBUTE_NORMAL, // Normal file
        NULL);                 // No attr. template

    if (InputFile == INVALID_HANDLE_VALUE)
    {
        wprintf(L"Cannot open \t%s\n", lpFileName);
        goto done;
    }

    /* Get input file size. */
    Success = GetFileSizeEx(InputFile, &FileSize);
    if ((!Success) || (FileSize.QuadPart > 0xFFFFFFFF))
    {
        wprintf(L"Cannot get input file size or file is larger than 4GB.\n");
        goto done;
    }
    InputFileSize = FileSize.LowPart;

    /* Allocate memory for file content. */
    InputBuffer = (PBYTE)malloc(InputFileSize);
    if (!InputBuffer)
    {
        wprintf(L"Cannot allocate memory for input buffer.\n");
        goto done;
    }

    /* Read input file. */
    Success = ReadFile(InputFile, InputBuffer, InputFileSize, &ByteRead, NULL);
    if ((!Success) || (ByteRead != InputFileSize))
    {
        wprintf(L"Cannot read from \t%s\n", lpFileName);
        goto done;
    }

    /* Open an empty file for writing, if exist, overwrite it. */
    CompressedFile = CreateFile(
        lpCompressFile,         // Compressed file name
        GENERIC_WRITE | DELETE, // Open for writing; delete if cannot compress
        0,                      // Do not share
        NULL,                   // Default security
        CREATE_ALWAYS,          // Create a new file; if exist, overwrite it
        FILE_ATTRIBUTE_NORMAL,  // Normal file
        NULL);                  // No template

    if (CompressedFile == INVALID_HANDLE_VALUE)
    {
        wprintf(L"Cannot create file \t%s\n", lpCompressFile);
        goto done;
    }

    StartTime = GetTickCount64();

    /* Call BlockModeCompress() again to do compression. */
    Success = BlockModeCompress(
        InputBuffer,          // Input buffer, Uncompressed data
        InputFileSize,        // Uncompressed data size
        &CompressedBuffer,    // Compressed Buffer
        &CompressedDataSize); // Compressed Data size

    if (!Success)
    {
        goto done;
    }

    EndTime = GetTickCount64();

    /* Get compression time. */
    TimeDuration = (EndTime - StartTime) / 1000.0;

    /* Write compressed data to output file. */
    Success = WriteFile(
        CompressedFile,     // File handle
        CompressedBuffer,   // Start of data to write
        CompressedDataSize, // Number of byte to write
        &ByteWritten,       // Number of byte written
        NULL);              // No overlapping structure

    if ((ByteWritten != CompressedDataSize) || (!Success))
    {
        wprintf(L"Cannot write compressed data to file: %d\n", GetLastError());
        goto done;
    }

    wprintf(L"Input file size: %ld; Compressed Size: %ld\n",
            InputFileSize, CompressedDataSize);
    wprintf(L"Compression Time(Exclude I/O): %.2f seconds\n", TimeDuration);
    wprintf(L"File Compressed.\n");

    DeleteTargetFile = FALSE;

done:

    if (CompressedBuffer)
    {
        free(CompressedBuffer);
    }

    if (InputBuffer)
    {
        free(InputBuffer);
    }

    if (InputFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(InputFile);
    }

    if (CompressedFile != INVALID_HANDLE_VALUE)
    {
        /* Compression fails, delete the compressed file. */
        if (DeleteTargetFile)
        {
            FILE_DISPOSITION_INFO fdi;
            fdi.DeleteFile = TRUE;                  // Marking for deletion
            Success = SetFileInformationByHandle(
                CompressedFile,
                FileDispositionInfo,
                &fdi,
                sizeof(FILE_DISPOSITION_INFO));
            if (!Success)
            {
                wprintf(L"Cannot delete corrupted compressed file.\n");
            }
        }

        CloseHandle(CompressedFile);
    }

    return 0;
}
