#include <windows.h>
#include <types.h>
#include <excpt.h>
#include <tchar.h>
#include <cardserv.h>
#include <cardapi.h>
#include <tuple.h>
#include <devload.h>
#include <diskio.h>
#include <nkintr.h>
#include <windev.h>

#include <s3c2410x.h>

#define PRIVATE			static
#define PUBLIC

PUBLIC BOOL WINAPI
DllMain(HANDLE hInstDll, DWORD dwReason, LPVOID lpvReserved)
{
	switch ( dwReason ) 
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls((HMODULE) hInstDll);
		break;

	}
	return (TRUE);
}

BOOL SAM_Close(DWORD Handle)
{
	RETAILMSG(1,(TEXT("++SAM_Close Function\r\n")));
	return TRUE;
}

BOOL SAM_Deinit(DWORD dwData)
{
	RETAILMSG(1,(TEXT("++SAM_Deinit Function\r\n")));
	return TRUE;
}

DWORD SAM_Init(DWORD dwContext)
{
	RETAILMSG(1,(TEXT("++SAM_Init Function\r\n")));
	return TRUE;
}

BOOL SAM_IOControl(DWORD hOpenContext, DWORD dwCode, PBYTE pInBuf,
		   DWORD dwLenIn, PBYTE pOutBuf, DWORD dwLenOut,
		   PDWORD pBytesReturned)
{
	RETAILMSG(1,(TEXT("++SAM_IOControl Function\r\n")));
	switch(dwcode)
	{
		// dwCode에 따른 함수
	}
	return TRUE;
}

DWORD SAM_Open(DWORD dwData, DWORD dwAccess, DWORD dwShareMode)
{
	RETAILMSG(1,(TEXT("++SAM_Open Function\r\n")));
	return TRUE;
}

void SAM_PowerDown(DWORD hDeviceContext)
{
	RETAILMSG(1,(TEXT("++SAM_PowerDown Function\r\n")));
	return;
}

void SAM_PowerUp(DWORD hDeviceContext)
{
	RETAILMSG(1,(TEXT("++SAM_PowerUp Function\r\n")));
	return;
}

DWORD SAM_Read(DWORD hOpenContext, LPVOID pBuffer, DWORD Count)
{
	RETAILMSG(1,(TEXT("++SAM_Read Function\r\n")));
	return TRUE;
}

DWORD SAM_Write(DWORD hOpenContext, LPCVOID pBuffer, DWORD Count)
{
	RETAILMSG(1,(TEXT("++SAM_Write Function\r\n")));
	return TRUE;
}

DWORD SAM_Seek(DWORD hOpenContext, long Amount, WORD type)
{
	RETAILMSG(1,(TEXT("++SAM_Seek Function\r\n")));
	return TRUE;
}


