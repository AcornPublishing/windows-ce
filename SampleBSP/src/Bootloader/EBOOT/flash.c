#include <windows.h>

/* 
	BLCOMMON's BootloaderMain() -> BLCOMMON's DownloadImage() -> OEMStartEraseFlash()
	Flash Erase Start job
*/
BOOL OEMStartEraseFlash (DWORD dwStartAddr, DWORD dwLength) 
{ 
	return(FALSE); 
}

/* 
	BLCOMMON's BootloaderMain() -> BLCOMMON's DownloadImage() -> OEMContinueEraseFlash()
	Continue Erase job
*/
void OEMContinueEraseFlash (void) 
{
}

/* 
	BLCOMMON's BootloaderMain() -> BLCOMMON's DownloadImage() -> OEMFinishEraseFlash()
	Finish Erase job
*/
BOOL OEMFinishEraseFlash (void) 
{
	return(FALSE); 
}

/* 
	BLCOMMON's BootloaderMain() -> BLCOMMON's DownloadImage() -> OEMIsFlashAddr()
	Check that dwAddr is in Flash Address Range
*/
BOOL OEMIsFlashAddr (DWORD dwAddr) 
{ 
	return(FALSE); 
}

/* 
	BLCOMMON's BootloaderMain() -> BLCOMMON's DownloadImage() -> OEMWriteFlash()
	Write Data to Flash
*/
BOOL OEMWriteFlash(DWORD dwStartAddr, DWORD dwLength) 
{ 
	return(FALSE); 
}

/* 
	BLCOMMON's BootloaderMain() -> BLCOMMON's DownloadImage() -> OEMMapMemAddr()
	BLCOMMON's BootloaderMain() -> OEMMapMemAddr()
	return Cache RAM Address for given dwAddr
*/
LPBYTE OEMMapMemAddr (DWORD dwImageStart, DWORD dwAddr) 
{ 
	return((LPBYTE)dwAddr); 
}
