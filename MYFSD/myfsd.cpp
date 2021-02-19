#include "MYFSD.h"

#ifdef DEBUG

DBGPARAM dpCurSettings =
{
    TEXT("MYFSD"),
	{
        TEXT("Apis"),
		TEXT(""),
		TEXT(""),
		TEXT(""),
        TEXT(""),
		TEXT(""),
		TEXT(""),
		TEXT(""),
        TEXT(""),
		TEXT(""),
		TEXT(""),
		TEXT(""),
        TEXT(""),
		TEXT(""),
		TEXT(""),
		TEXT("")
	},
    0
};

#endif

static CRITICAL_SECTION	csMain;
VolumeState *g_pvs = NULL;

BOOL MYFSD_MountDisk(HDSK hDsk)
{
	DWORD dwError = ERROR_SUCCESS;
	BOOL fSuccess = FALSE;
	
	DEBUGMSG(ZONE_APIS, (L"ReleaseFSD: FSD_UnmountDisk\n"));
	EnterCriticalSection(&csMain);
	g_pvs = (VolumeState *)HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(VolumeState));
	if (g_pvs)
	{
		g_pvs->hVolume = ::FSDMGR_RegisterVolume(hDsk, DEFAULT_VOLUME_NAME, (PVOLUME)g_pvs);
		if (g_pvs->hVolume)
		{
			fSuccess = TRUE;
			RETAILMSG(1, (L"Mounted MYFSD volume '\\%s'\n", DEFAULT_VOLUME_NAME));
		}
	} else {
		dwError = ERROR_OUTOFMEMORY;
	}
	LeaveCriticalSection(&csMain);
    if (dwError) {
    	SetLastError(dwError);
    }
	return fSuccess;
}

BOOL MYFSD_UnmountDisk(HDSK hdsk)
{
    FSDMGR_DeregisterVolume(g_pvs->hVolume);
    return TRUE;
}

BOOL WINAPI DllMain(HANDLE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    switch(dwReason) {
	    case DLL_PROCESS_ATTACH: {
			DEBUGREGISTER((HINSTANCE)hInstance);
			InitializeCriticalSection(&csMain);
			break;
	    }
	    case DLL_PROCESS_DETACH: {
			DeleteCriticalSection(&csMain);
			break;
	    }
    	default:
        	break;
    }
    return TRUE;
}

BOOL MYFSD_CreateDirectoryW(PVOLUME pvol, PCWSTR pwsPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	BOOL fSuccess = TRUE;
    DWORD dwError =ERROR_NOT_SUPPORTED;
	
	DEBUGMSG(ZONE_APIS, (L"MYFSD: FSD_CreateDirectoryW\n"));
	if (dwError) {
		SetLastError( dwError);
	}
    return fSuccess;
}

BOOL MYFSD_RemoveDirectoryW(PVOLUME pvol, PCWSTR pwsPathName)
{
	BOOL fSuccess = TRUE;
    DWORD dwError =ERROR_NOT_SUPPORTED;
	
	DEBUGMSG(ZONE_APIS, (L"MYFSD: FSD_RemoveDirectoryWW\n"));
	if (dwError) {
		SetLastError( dwError);
	}
    return fSuccess;
}

DWORD MYFSD_GetFileAttributesW(PVOLUME pvol, PCWSTR pwsFileName)
{
    DWORD dwError =ERROR_NOT_SUPPORTED;
    DWORD dwRet = 0;
    
	DEBUGMSG(ZONE_APIS, (L"MYFSD: FSD_GetFileAttributesWW\n"));
	if (dwError) {
		SetLastError( dwError);
	}

    return dwRet;
}

BOOL MYFSD_SetFileAttributesW(PVOLUME pvol, PCWSTR pwsFileName, DWORD dwAttributes)
{
	BOOL fSuccess = TRUE;
    DWORD dwError =ERROR_NOT_SUPPORTED;
	
	DEBUGMSG(ZONE_APIS, (L"MYFSD: FSD_SetFileAttributesWW\n"));
	if (dwError) {
		SetLastError( dwError);
	}
    return fSuccess;
}

BOOL MYFSD_DeleteFileW(PVOLUME pvol, PCWSTR pwsFileName)
{
	BOOL fSuccess = TRUE;
    DWORD dwError =ERROR_NOT_SUPPORTED;
	
	DEBUGMSG(ZONE_APIS, (L"MYFSD: FSD_DeleteFileWW\n"));
	if (dwError) {
		SetLastError( dwError);
	}
    return fSuccess;
}

BOOL MYFSD_MoveFileW(PVOLUME pvol, PCWSTR pwsOldFileName, PCWSTR pwsNewFileName)
{
	BOOL fSuccess = TRUE;
    DWORD dwError =ERROR_NOT_SUPPORTED;
	
	DEBUGMSG(ZONE_APIS, (L"MYFSD: FSD_MoveFileWW\n"));
	if (dwError) {
		SetLastError( dwError);
	}
    return fSuccess;
}


BOOL MYFSD_RegisterFileSystemFunction(PVOLUME pvol, SHELLFILECHANGEFUNC_t pfn)
{
	BOOL fSuccess = TRUE;
    DWORD dwError =ERROR_NOT_SUPPORTED;
	
	DEBUGMSG(ZONE_APIS, (L"MYFSD: FSD_RegisterFileSystemFunctionW\n"));
	if (dwError) {
		SetLastError( dwError);
	}
    return fSuccess;
}


BOOL MYFSD_DeleteAndRenameFileW(PVOLUME pvol, PCWSTR pwsOldFile, PCWSTR pwsNewFile)
{
	BOOL fSuccess = TRUE;
    DWORD dwError =ERROR_NOT_SUPPORTED;
	
	DEBUGMSG(ZONE_APIS, (L"MYFSD: FSD_DeleteAndRenameFileW\n"));
	if (dwError) {
		SetLastError( dwError);
	}
    return fSuccess;
}


BOOL MYFSD_GetDiskFreeSpaceW(
PVOLUME pvol,
PCWSTR pwsPathName,
PDWORD pSectorsPerCluster,
PDWORD pBytesPerSector,
PDWORD pFreeClusters,
PDWORD pClusters)
{
	BOOL fSuccess = TRUE;
    DWORD dwError =ERROR_NOT_SUPPORTED;

	DEBUGMSG(ZONE_APIS, (L"MYFSD: FSD_GetFreeDIskSpaceW\n"));


	if (dwError) {
		SetLastError( dwError);
	}
    return fSuccess;
}

void MYFSD_Notify(PVOLUME pvol, DWORD dwFlags)
{
	DEBUGMSG(ZONE_APIS, (L"MYFSD: FSD_NotifyW\n"));
    return;
}

HANDLE MYFSD_FindFirstFileW(PVOLUME pvol, HANDLE hProc, PCWSTR pwsFileSpec, PWIN32_FIND_DATAW pfd)
{
    DWORD dwError =ERROR_NOT_SUPPORTED;
	HANDLE hSearch = INVALID_HANDLE_VALUE;
	DEBUGMSG(ZONE_APIS, (L"MYFSD: FSD_FindFirstFileW\n"));
	if (dwError) {
		SetLastError( dwError);
	}
    return hSearch;
}


BOOL MYFSD_FindNextFileW(PSEARCH  psh, PWIN32_FIND_DATAW pfd)
{
	BOOL fSuccess = TRUE;
    DWORD dwError =ERROR_NOT_SUPPORTED;
	
	DEBUGMSG(ZONE_APIS, (L"MYFSD: FSD_FindNextFileW\n"));
	if (dwError) {
		SetLastError( dwError);
	}
    return fSuccess;
}

BOOL MYFSD_FindClose(PSEARCH psh)
{
	BOOL fSuccess = TRUE;
    DWORD dwError =ERROR_NOT_SUPPORTED;
	
	DEBUGMSG(ZONE_APIS, (L"MYFSD: FSD_FindClose\n"));
	if (dwError) {
		SetLastError( dwError);
	}
    return fSuccess;
}


HANDLE MYFSD_CreateFileW(
PVOLUME pvol,
HANDLE hProc,
LPCWSTR lpFileName,
DWORD dwAccess,
DWORD dwShareMode,
LPSECURITY_ATTRIBUTES lpSecurityAttributes,
DWORD dwCreate,
DWORD dwFlagsAndAttributes,
HANDLE hTemplateFile)
{
    DWORD dwError =ERROR_NOT_SUPPORTED;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	DEBUGMSG(ZONE_APIS, (L"MYFSD: FSD_CreateFileW\n"));
	if (dwError) {
		SetLastError( dwError);
	}
    return hFile;
}


BOOL MYFSD_CloseFile(PFILE pfh)
{
	BOOL fSuccess = TRUE;
	DEBUGMSG(ZONE_APIS, (L"MYFSD: FSD_CloseFile\n"));
    DWORD dwError =ERROR_NOT_SUPPORTED;
	
	if (dwError) {
		SetLastError( dwError);
	}
    return fSuccess;
}

BOOL MYFSD_ReadFile(
PFILE pfh,
LPVOID buffer,
DWORD nBytesToRead,
LPDWORD lpNumBytesRead,
LPOVERLAPPED lpOverlapped)
{
    DWORD dwError =ERROR_NOT_SUPPORTED;
    BOOL fSuccess = FALSE;

	DEBUGMSG(ZONE_APIS, (L"MYFSD: FSD_ReadFile\n"));
    if (lpNumBytesRead)
    	*lpNumBytesRead = 0;

	if (dwError) {
		SetLastError( dwError);
	}
    return fSuccess;
}



BOOL MYFSD_ReadFileWithSeek(
PFILE pfh,
LPVOID buffer,
DWORD nBytesToRead,
LPDWORD lpNumBytesRead,
LPOVERLAPPED lpOverlapped,
DWORD dwLowOffset,
DWORD dwHighOffset)
{
    DWORD dwError =ERROR_NOT_SUPPORTED;
    BOOL fSuccess = FALSE;

	DEBUGMSG(ZONE_APIS, (L"MYFSD: FSD_ReadFileWithSeek\n"));
    if (lpNumBytesRead)
    	*lpNumBytesRead = 0;

	if (dwError) {
		SetLastError( dwError);
	}
    return fSuccess;
}

BOOL MYFSD_WriteFile(
PFILE pfh,
LPCVOID buffer,
DWORD nBytesToWrite,
LPDWORD lpNumBytesWritten,
LPOVERLAPPED lpOverlapped)
{
    DWORD dwError =ERROR_NOT_SUPPORTED;
    BOOL fSuccess = FALSE;

	DEBUGMSG(ZONE_APIS, (L"MYFSD: FSD_WriteFile\n"));
    if (lpNumBytesWritten)
    	*lpNumBytesWritten = 0;

	if (dwError) {
		SetLastError( dwError);
	}
    return fSuccess;
}



BOOL MYFSD_WriteFileWithSeek(
PFILE pfh,
LPCVOID buffer,
DWORD nBytesToWrite,
LPDWORD lpNumBytesWritten,
LPOVERLAPPED lpOverlapped,
DWORD dwLowOffset,
DWORD dwHighOffset)
{
    DWORD dwError =ERROR_NOT_SUPPORTED;
    BOOL fSuccess = FALSE;

	DEBUGMSG(ZONE_APIS, (L"MYFSD: FSD_WriteFileWithSeek\n"));
    if (lpNumBytesWritten)
    	*lpNumBytesWritten = 0;
    	
	if (dwError) {
		SetLastError( dwError);
	}
    return fSuccess;
}



DWORD MYFSD_SetFilePointer(
PFILE pfh,
LONG lDistanceToMove,
PLONG lpDistanceToMoveHigh,
DWORD dwMoveMethod)
{
    DWORD dwError =ERROR_NOT_SUPPORTED;
    DWORD dwRet = 0;

	DEBUGMSG(ZONE_APIS, (L"MYFSD: FSD_SetFilePointer\n"));
	if (dwError) {
		SetLastError( dwError);
	}

    return dwRet;
}


DWORD MYFSD_GetFileSize(PFILE pfh, LPDWORD lpFileSizeHigh)
{
    DWORD dwError =ERROR_NOT_SUPPORTED;
    DWORD dwRet = 0;

	DEBUGMSG(ZONE_APIS, (L"MYFSD: FSD_GetFileSize\n"));
    if (lpFileSizeHigh)
    	*lpFileSizeHigh = 0;
    	
	if (dwError) {
		SetLastError( dwError);
	}

    return dwRet;
}


BOOL MYFSD_GetFileInformationByHandle(PFILE pfh, LPBY_HANDLE_FILE_INFORMATION lpFileInfo)
{
    DWORD dwError =ERROR_NOT_SUPPORTED;
    BOOL fSuccess = FALSE;

	DEBUGMSG(ZONE_APIS, (L"MYFSD: FSD_GetFileInformationByHandle\n"));
	if (dwError) {
		SetLastError( dwError);
	}
    return fSuccess;
}


BOOL MYFSD_FlushFileBuffers(PFILE pfh)
{
    DWORD dwError =ERROR_NOT_SUPPORTED;
    BOOL fSuccess = FALSE;

	DEBUGMSG(ZONE_APIS, (L"MYFSD: FSD_FlushFileBuffers\n"));
	if (dwError) {
		SetLastError( dwError);
	}
    return fSuccess;
}


BOOL MYFSD_GetFileTime(
PFILE pfh,
LPFILETIME lpCreation,
LPFILETIME lpLastAccess,
LPFILETIME lpLastWrite)
{
    DWORD dwError =ERROR_NOT_SUPPORTED;
    BOOL fSuccess = FALSE;

	DEBUGMSG(ZONE_APIS, (L"MYFSD: FSD_GetFileTime\n"));
	if (lpCreation)
		memset( lpCreation, 0, sizeof(FILETIME));
	if (lpLastAccess)
		memset( lpLastAccess, 0, sizeof(FILETIME));
	if (lpLastWrite)
		memset( lpLastWrite, 0, sizeof(FILETIME));
		
	if (dwError) {
		SetLastError( dwError);
	}
    return fSuccess;
}


BOOL MYFSD_SetFileTime(
PFILE pfh,
CONST FILETIME *lpCreation,
CONST FILETIME *lpLastAccess,
CONST FILETIME *lpLastWrite)
{
    DWORD dwError =ERROR_NOT_SUPPORTED;
    BOOL fSuccess = FALSE;

	DEBUGMSG(ZONE_APIS, (L"MYFSD: FSD_SetFileTime\n"));
	if (dwError) {
		SetLastError( dwError);
	}
    return fSuccess;
}


BOOL MYFSD_SetEndOfFile(PFILE pfh)
{
    DWORD dwError =ERROR_NOT_SUPPORTED;
    BOOL fSuccess = FALSE;

	DEBUGMSG(ZONE_APIS, (L"MYFSD: FSD_SetEndOfFile\n"));
	if (dwError) {
		SetLastError( dwError);
	}
    return fSuccess;
}


BOOL MYFSD_DeviceIoControl(
PFILE pfh,
DWORD dwIoControlCode,
LPVOID lpInBuf, DWORD nInBufSize,
LPVOID lpOutBuf, DWORD nOutBufSize,
LPDWORD lpBytesReturned,
LPOVERLAPPED lpOverlapped)
{

    DWORD dwError =ERROR_NOT_SUPPORTED;
    BOOL fSuccess = FALSE;

	DEBUGMSG(ZONE_APIS, (L"MYFSD: FSD_DeviceIoControl\n"));
	if (lpBytesReturned)
		*lpBytesReturned = 0;
	if (dwError) {
		SetLastError( dwError);
	}
    return fSuccess;


}

