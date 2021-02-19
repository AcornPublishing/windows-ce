#include <windows.h>
#include <blcommon.h>

void main(void) // start.s -> main()
{
    BootloaderMain(); // BLCOMMON's BootloaderMain() Call
}

/* 
	BLCOMMON's BootloaderMain() -> OEMDebugInit
	UART(Debug Monitor Port) Initializing
*/
BOOL OEMDebugInit(void) 
{ 
	return(TRUE); 
}

/* 
	BLCOMMON's BootloaderMain() -> OEMPlatformInit
	Ethernet, USB(Download Port) Initializing
*/
BOOL OEMPlatformInit(void) 
{ 
	return(TRUE); 
}

/* 
	BLCOMMON's BootloaderMain() -> OEMPreDownload
	Till Acquiring Host's Platform Builder Ack
*/
DWORD OEMPreDownload(void) 
{ 
	return(0); 
}

/* 
	BLCOMMON's BootloaderMain() -> OEMLaunch
	Jump Kernel(NK.EXE)
*/
void OEMLaunch (DWORD dwImageStart, DWORD dwImageLength, DWORD dwLaunchAddr, const ROMHDR *pRomHdr) 
{
}

/* 
	BLCOMMON's BootloaderMain() -> BLCOMMON's DownloadImage() -> OEMReadData()
	Read Data from Download Port
*/
BOOL OEMReadData (DWORD cbData, LPBYTE pbData) 
{ 
	return(TRUE); 
}

/* 
	BLCOMMON's BootloaderMain() -> BLCOMMON's DownloadImage() -> OEMShowProgress()
	Show Current Download Progress to User
*/
void OEMShowProgress (DWORD dwPacketNum) 
{
}

/* 
	Any Kernel Code or OAL Code -> OEMWriteDebugByte()
	Send Debug Data to Host
*/
void OEMWriteDebugByte(unsigned char c) 
{
}
