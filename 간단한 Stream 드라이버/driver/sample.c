#include <windows.h>

#ifdef DEBUG

#define ZONE_INIT		DEBUGZONE(0)
#define ZONE_DEINIT		DEBUGZONE(1)
#define ZONE_OPEN		DEBUGZONE(2)
#define ZONE_WRITE      DEBUGZONE(3)
#define ZONE_IOCONTROL  DEBUGZONE(4)
#define ZONE_CLOSE      DEBUGZONE(5)
#define ZONE_POWERUP    DEBUGZONE(6)
#define ZONE_POWERDOWN  DEBUGZONE(7)

DBGPARAM dpCurSettings = {
    TEXT("SAMPLE Driver"), {
    TEXT("Init"),TEXT("Deinit"),TEXT("Open"),TEXT("Write"),
    TEXT("IOControl"),TEXT("Close"),TEXT("PowerUp"),TEXT("PowerDown"),
    TEXT("Undefined"),TEXT("Undefined"),TEXT("Undefined"),TEXT("Undefined"),
    TEXT("Undefined"),TEXT("Undefined"),TEXT("Undefined"),TEXT("Undefined") },
    0x1
};
#endif 

#define METHOD_BUFFERED                 0
#define FILE_ANY_ACCESS                 0
#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)

#define IOCTL_SAMPLE_FIRST	CTL_CODE( 0, 0x800, 0, 0 )

BOOL WINAPI
DllMain(HINSTANCE DllInstance, DWORD Reason, LPVOID Reserved)
{
    switch(Reason) {
        case DLL_PROCESS_ATTACH:
            DEBUGREGISTER(DllInstance);
            break;
    }
    return TRUE;
}   

#define INITHANDLE 1
DWORD
SAM_Init(
    DWORD dwContext
    )
{
    DEBUGMSG(ZONE_INIT, (TEXT("SAMPLE: SAM_Init \r\n")));
	return INITHANDLE;
}

BOOL
SAM_Deinit(
    DWORD InitHandle     
    )
{
    DEBUGMSG(ZONE_DEINIT, (TEXT("SAMPLE: SAM_Deinit \r\n")));
    return TRUE;
}   

void SAM_PowerUp(
	DWORD InitHandle
	)
{
    DEBUGMSG(ZONE_POWERUP, (TEXT("SAMPLE: SAM_PowerUp \r\n")));
}

void SAM_PowerDown(
	DWORD InitHandle
	)
{
    DEBUGMSG(ZONE_POWERDOWN, (TEXT("SAMPLE: SAM_PowerDown \r\n")));
}

#define OPENHANDLE 2
DWORD
SAM_Open(
	DWORD InitHandle,
    DWORD dwAccess,
    DWORD dwShareMode
    )
{
    DEBUGMSG(ZONE_OPEN, (TEXT("SAMPLE: SAM_Open \r\n")));
    return OPENHANDLE;
}   

BOOL
SAM_Close(
    DWORD OpenHandle
    )
{
    DEBUGMSG(ZONE_CLOSE, (TEXT("SAMPLE: SAM_Close \r\n")));
    return TRUE;
}   

BOOL
SAM_IOControl(
    DWORD OpenHandle,
    DWORD dwIoControlCode,
    PBYTE pInBuf,
    DWORD nInBufSize,
    PBYTE pOutBuf,
    DWORD nOutBufSize,
    PDWORD pBytesReturned
    )
{
    DEBUGMSG(ZONE_IOCONTROL, (TEXT("SAMPLE: SAM_IOControl \r\n")));

	switch( dwIoControlCode )
	{
	case IOCTL_SAMPLE_FIRST:
		break;
	}
	return TRUE;
}   

DWORD SAM_Write(
	DWORD OpenHandle, 
	LPCVOID pBuffer, 
	DWORD dwNumBytes
	)
{
    DEBUGMSG(ZONE_WRITE, (TEXT("SAMPLE: SAM_Write \r\n")));
	return 0;
}


