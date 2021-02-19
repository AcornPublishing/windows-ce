#include <windows.h>
#include <pehdr.h>
#include <Romldr.h>

extern volatile DWORD CurMSec;
extern DWORD dwReschedTime;

BOOL NoPPFS = FALSE;					
extern ROMHDR *const volatile pTOC;

BOOL (*pQueryPerformanceCounter)(LARGE_INTEGER *lpliPerformanceCount);
BOOL (*pQueryPerformanceFrequency)(LARGE_INTEGER *lpliPerformanceFreq);

void OEMInitDebugSerial(void) 
{
}

void OEMWriteDebugByte(UCHAR ch) 
{
}

int OEMReadDebugByte() 
{
	return 0;
}

void OEMWriteDebugString(unsigned short *str) 
{
}

int OEMParallelPortGetByte(void)
{
	return -1;
}

VOID OEMParallelPortSendByte(BYTE chData)
{
}

BOOL OEMIoControl(DWORD dwIoControlCode, LPVOID lpInBuf, DWORD nInBufSize,
	LPVOID lpOutBuf, DWORD nOutBufSize, LPDWORD lpBytesReturned) 
{
	return FALSE;
}

BOOL 
OEMInterruptEnable(DWORD  idInt,	// @parm Interrupt ID to be enabled. See <l Interrupt ID's.Interrupt ID's>  for a list of possble values.
				   LPVOID pvData,	// @parm ptr to data passed in in the <f InterruptInitialize> call
		           DWORD  cbData)	// @parm Size of data pointed to be <p pvData>
{
	return(FALSE);
}

void 
OEMInterruptDisable(DWORD idInt)	// @parm Interrupt ID to be disabled. See <t Interrupt ID's>
									// for the list of possible values.
{
}

void 
OEMInterruptDone(DWORD idInt)	// @parm Interrupt ID. See <t Interrupt ID's>
                    			// for the list of possible values.
{
}

BOOL 
OEMGetExtensionDRAM(
    LPDWORD lpMemStart, 
    LPDWORD lpMemLen
    ) 
{
    return FALSE;
}

BOOL 
OEMQueryPerformanceCounter(
    LARGE_INTEGER *lpliPerformanceCount
    )
{
    return FALSE;
}

BOOL 
OEMQueryPerformanceFrequency(
    LARGE_INTEGER *lpliPerformanceFreq
    ) 
{
	return FALSE;
}

VOID OEMPowerOff(void)
{
}

void
OEMIdle(
	DWORD dwIdleParam
)
{
}

DWORD 
SC_GetTickCount(void) 
{
	return CurMSec;
}

void OEMInit()  
{
	memset((PVOID)pTOC->ulRAMFree,0,sizeof(DWORD)*3); // Cold Boot

	CurMSec = 0;
	pQueryPerformanceCounter = OEMQueryPerformanceCounter;
	pQueryPerformanceFrequency = OEMQueryPerformanceFrequency;
}

void OEMInterruptHandlerFIQ() 
{
}

int OEMInterruptHandler(unsigned int ra) 
{
	return 0;
}

BOOL
OEMSetRealTime(LPSYSTEMTIME lpst) 
{
	return FALSE;
}

BOOL 
OEMGetRealTime(LPSYSTEMTIME lpst) 
{
	return FALSE;
}

BOOL 
OEMSetAlarmTime(LPSYSTEMTIME lpst) 
{
	return FALSE;
}

void 
InitClock(void) 
{
}

/////////
// 5.0
void OEMCacheRangeFlush(VOID *pAddress, DWORD length, DWORD flags)
{
}

DWORD OEMARMCacheMode(void)
{
	return 0;
}

void OEMDataAbortHandler()
{
}