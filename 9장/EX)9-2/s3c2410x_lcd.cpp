//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this source code is subject to the terms of the Microsoft end-user
// license agreement (EULA) under which you licensed this SOFTWARE PRODUCT.
// If you did not accept the terms of the EULA, you are not authorized to use
// this source code. For a copy of the EULA, please see the LICENSE.RTF on your
// install media.
//
/*++
THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.
--*/

#include "precomp.h"
#ifdef CLEARTYPE
#include <ctblt.h>
#endif
#include <s3c2410x.h>
#include <ceddk.h>

DWORD gdwLCDVirtualFrameBase;


INSTANTIATE_GPE_ZONES(0x3,"MGDI Driver","unused1","unused2")	// Start with errors and warnings

static	GPE		*gGPE = (GPE*)NULL;
static	ulong	gBitMasks[] = { 0xF800, 0x07E0, 0x001F };		// 565 MODE

static TCHAR gszBaseInstance[256] = _T("Drivers\\Display\\S3C2410\\CONFIG");

#define dim(x)                                  (sizeof(x) / sizeof(x[0]))

// This prototype avoids problems exporting from .lib
BOOL APIENTRY GPEEnableDriver(ULONG engineVersion, ULONG cj, DRVENABLEDATA *data,
							  PENGCALLBACKS  engineCallbacks);

// GWES will invoke this routine once prior to making any other calls into the driver.
// This routine needs to save its instance path information and return TRUE.  If it
// returns FALSE, GWES will abort the display initialization.
BOOL APIENTRY
DisplayInit(LPCTSTR pszInstance, DWORD dwNumMonitors)
{
	DWORD dwStatus;
	HKEY hkDisplay;
	BOOL fOk = FALSE;

    RETAILMSG(0, (_T("SALCD2: display instance '%s', num monitors %d\r\n"),
    	pszInstance != NULL ? pszInstance : _T("<NULL>"), dwNumMonitors));

    if(pszInstance != NULL) {
        _tcsncpy(gszBaseInstance, pszInstance, dim(gszBaseInstance));
    }

	// sanity check the path by making sure it exists
	dwStatus = RegOpenKeyEx(HKEY_LOCAL_MACHINE, gszBaseInstance, 0, 0, &hkDisplay);
	if(dwStatus == ERROR_SUCCESS) {
		RegCloseKey(hkDisplay);
		fOk = TRUE;
	} else {
		RETAILMSG(0, (_T("SALCD2: DisplayInit: can't open '%s'\r\n"), gszBaseInstance));
	}

    return fOk;
}

BOOL APIENTRY DrvEnableDriver(ULONG engineVersion, ULONG cj, DRVENABLEDATA *data,
							  PENGCALLBACKS  engineCallbacks)
{
	BOOL fOk = FALSE;

	// make sure we know where our registry configuration is
	if(gszBaseInstance[0] != 0) {
		fOk = GPEEnableDriver(engineVersion, cj, data, engineCallbacks);
	}

	return fOk;
}

//
// Main entry point for a GPE-compliant driver
//

GPE *GetGPE(void)
{
	if (!gGPE)
	{
		gGPE = new S3C2410DISP();
	}

	return gGPE;
}

WORD	TempBuffer[241][320];

S3C2410DISP::S3C2410DISP (void)
{
	RETAILMSG(0, (TEXT("++S3C2410DISP::S3C2410DISP\r\n")));
	
	// setup up display mode related constants
	m_nScreenWidth = 240;
	m_nScreenHeight = 320;
	m_colorDepth = 16;
	m_cbScanLineLength = m_nScreenWidth * 2;
	m_FrameBufferSize = m_nScreenHeight * m_cbScanLineLength;
	
	// memory map register access window, frame buffer, and program LCD controller
	InitializeHardware();

#ifdef ROTATE
	m_iRotate = 0;
	SetRotateParms();
#endif //ROTATE	

	// setup ModeInfo structure
	m_ModeInfo.modeId = 0;
	m_ModeInfo.width = m_nScreenWidth;
	m_ModeInfo.height = m_nScreenHeight;
	m_ModeInfo.Bpp = m_colorDepth;
	m_ModeInfo.format = gpe16Bpp;
	m_ModeInfo.frequency = 60;	// ?
	m_pMode = &m_ModeInfo;
	
	// allocate primary display surface
#ifdef 	ROTATE
	m_pPrimarySurface = new GPESurfRotate(m_nScreenWidthSave, m_nScreenHeightSave, (void*)(m_VirtualFrameBuffer), m_cbScanLineLength, m_ModeInfo.format);
#else
	m_pPrimarySurface = new GPESurf(m_nScreenWidth, m_nScreenHeight, (void*)(m_VirtualFrameBuffer), m_cbScanLineLength, m_ModeInfo.format);	
#endif //!ROTATE

    if (m_pPrimarySurface)
    {
	    memset ((void*)m_pPrimarySurface->Buffer(), 0x0, m_FrameBufferSize);
    }

	// init cursor related vars
	m_CursorVisible = FALSE;
	m_CursorDisabled = TRUE;
	m_CursorForcedOff = FALSE;
	memset (&m_CursorRect, 0x0, sizeof(m_CursorRect));
	m_CursorBackingStore = NULL;
	m_CursorXorShape = NULL;
	m_CursorAndShape = NULL;

#ifdef CLEARTYPE
	HKEY  hKey;
	DWORD dwValue;
	ULONG ulGamma = DEFAULT_CT_GAMMA;	
	
	if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_LOCAL_MACHINE,szGamma,0, NULL,0,0,0,&hKey,&dwValue))
	{
	    if (dwValue == REG_OPENED_EXISTING_KEY)
	    {
		DWORD dwType = REG_DWORD;
		DWORD dwSize = sizeof(LONG);
		if (ERROR_SUCCESS == RegQueryValueEx(hKey,szGammaValue,0,&dwType,(BYTE *)&dwValue,&dwSize))
		{
		    ulGamma = dwValue;
		}
	    } 
	    else if (dwValue == REG_CREATED_NEW_KEY )
	    {
		RegSetValueEx(hKey,szGammaValue,0,REG_DWORD,(BYTE *)&ulGamma,sizeof(DWORD));
	    }
	    RegCloseKey(hKey);
	}

	SetClearTypeBltGamma(ulGamma);
	SetClearTypeBltMasks(gBitMasks[0], gBitMasks[1], gBitMasks[2]);
#endif //CLEARTYPE	

	RETAILMSG(0, (TEXT("--S3C2410DISP::S3C2410DISP\r\n")));
}


static BOOL InitializeLCDRegisters(DWORD dwPhysicalFrameBase)
{
    volatile S3C2410X_IOPORT_REG *s2410IOP = NULL;
    volatile S3C2410X_LCD_REG    *s2410LCD = NULL;
    PHYSICAL_ADDRESS pa;

    // Map the s3c2410x register pointers.
    //
    pa.QuadPart = S3C2410X_BASE_REG_PA_IOPORT;
    s2410IOP = (S3C2410X_IOPORT_REG *) MmMapIoSpace(pa, sizeof(S3C2410X_IOPORT_REG), FALSE);

    pa.QuadPart = S3C2410X_BASE_REG_PA_LCD;
    s2410LCD = (S3C2410X_LCD_REG *) MmMapIoSpace(pa, sizeof(S3C2410X_LCD_REG), FALSE);

    if (!s2410IOP || !s2410LCD)
    {
        RETAILMSG(1, (TEXT("ERROR: s3c2410x_lcd: InitializeLCDRegisters failed.\r\n")));
        return(FALSE);
    }

    // Set up the LCD controller registers to display a power-on bitmap image.
    //
    s2410IOP->GPCUP     = 0xFFFFFFFF;
    s2410IOP->GPCCON    = 0xAAAAAAAA;
                                    
    s2410IOP->GPDUP     = 0xFFFFFFFF;
    s2410IOP->GPDCON    = 0xAAAAAAAA; 

    s2410IOP->GPGUP    &= ~(1 << 4);
    s2410IOP->GPGCON   &= ~(3 << 8);

    s2410LCD->LCDCON1   =  (6           <<  8) |       /* VCLK = HCLK / ((CLKVAL + 1) * 2) -> About 7 Mhz  */
                           (LCD_MVAL   <<  7)  |       /* 0 : Each Frame                                   */
                           (3           <<  5) |       /* TFT LCD Pannel                                   */
                           (12          <<  1) |       /* 16bpp Mode                                       */
                           (0           <<  0) ;       /* Disable LCD Output                               */

    s2410LCD->LCDCON2   =  (LCD_VBPD        << 24) |   /* VBPD          :   1                              */
                           (LCD_LINEVAL_TFT << 14) |   /* Vertical Size : 320 - 1                          */
                           (LCD_VFPD        <<  6) |   /* VFPD          :   2                              */
                           (LCD_VSPW        <<  0) ;   /* VSPW          :   1                              */

    s2410LCD->LCDCON3   =  (LCD_HBPD        << 19) |   /* HBPD          :   6                              */
                           (LCD_HOZVAL_TFT  <<  8) |   /* HOZVAL_TFT    : 240 - 1                          */
                           (LCD_HFPD        <<  0) ;   /* HFPD          :   2                              */


    s2410LCD->LCDCON4   =  (LCD_MVAL        <<  8) |   /* MVAL          :  13                              */
                           (LCD_HSPW        <<  0) ;   /* HSPW          :   4                              */

    s2410LCD->LCDCON5   =  (0           << 12) |       /* BPP24BL       : LSB valid                        */
                           (1           << 11) |       /* FRM565 MODE   : 5:6:5 Format                     */
                           (0           << 10) |       /* INVVCLK       : VCLK Falling Edge                */
                           (1           <<  9) |       /* INVVLINE      : Inverted Polarity                */
                           (1           <<  8) |       /* INVVFRAME     : Inverted Polarity                */
                           (0           <<  7) |       /* INVVD         : Normal                           */
                           (0           <<  6) |       /* INVVDEN       : Normal                           */
                           (0           <<  5) |       /* INVPWREN      : Normal                           */
                           (0           <<  4) |       /* INVENDLINE    : Normal                           */
                           (0           <<  3) |       /* PWREN         : Disable PWREN                    */
                           (0           <<  2) |       /* ENLEND        : Disable LEND signal              */
                           (0           <<  1) |       /* BSWP          : Swap Disable                     */
                           (1           <<  0) ;       /* HWSWP         : Swap Enable                      */

    s2410LCD->LCDSADDR1 = ((dwPhysicalFrameBase >> 22)     << 21) | 
                          ((M5D(dwPhysicalFrameBase >> 1)) <<  0);

    s2410LCD->LCDSADDR2 = M5D((dwPhysicalFrameBase + (LCD_XSIZE_TFT * LCD_YSIZE_TFT * 2)) >> 1);

    s2410LCD->LCDSADDR3 = (((LCD_XSIZE_TFT - LCD_XSIZE_TFT) / 1) << 11) | (LCD_XSIZE_TFT / 1);        

	// 2410TK 보드를 위한 LCD설정으로 변경
	// s2410LCD->LPCSEL   |= 0x3;			// SMDK2410의 LCD 설정
	s2410LCD->LPCSEL	|= 0x2;				// 2410TK의 LCD 설정

    s2410LCD->TPAL      = 0x0;        
    s2410LCD->LCDCON1  |= 1;

    // Unmap register buffers.
    //
    MmUnmapIoSpace((PVOID)s2410IOP, 0);
    MmUnmapIoSpace((PVOID)s2410LCD, 0);

    return(TRUE);
}


void S3C2410DISP::InitializeHardware (void)
{
	WORD *ptr;
	DWORD index;
	HKEY hkDisplay = NULL;
	DWORD dwLCDPhysicalFrameBase = 0;
	DWORD dwStatus, dwType, dwSize;

	RETAILMSG(0, (_T("++S3C2410DISP::InitializeHardware\r\n")));

	// open the registry key and read our configuration
	dwStatus = RegOpenKeyEx(HKEY_LOCAL_MACHINE, gszBaseInstance, 0, 0, &hkDisplay);
	dwType = REG_DWORD;

	if(dwStatus == ERROR_SUCCESS && dwType == REG_DWORD) {
		dwSize = sizeof(DWORD);
		dwStatus = RegQueryValueEx(hkDisplay, _T("LCDVirtualFrameBase"), NULL, &dwType, 
			(LPBYTE) &gdwLCDVirtualFrameBase, &dwSize);
	}
	if(dwStatus == ERROR_SUCCESS && dwType == REG_DWORD) {
		dwSize = sizeof(DWORD);
		dwStatus = RegQueryValueEx(hkDisplay, _T("LCDPhysicalFrameBase"), NULL, &dwType, 
			(LPBYTE) &dwLCDPhysicalFrameBase, &dwSize);
	}

	// close the registry key
	if(hkDisplay != NULL) {
		RegCloseKey(hkDisplay);
	}

	// did we get everything?
	if(dwStatus != ERROR_SUCCESS) {
		RETAILMSG(0, (_T("SALCD2: InitializeHardware: couldn't get registry configuration\r\n")));
		return;
	}

    // Initialize LCD registers.
    if (!InitializeLCDRegisters(dwLCDPhysicalFrameBase))
    {
		RETAILMSG(0, (_T("SALCD2: InitializeHardware: failed to initialize LCD registers.\r\n")));
		return;
    }

	// map frame buffer into process space memory
	m_VirtualFrameBuffer = (DWORD)VirtualAlloc(0, (0x40000), MEM_RESERVE, PAGE_NOACCESS);
	if (m_VirtualFrameBuffer == NULL) 
	{
	    RETAILMSG(0,(TEXT("m_VirtualFrameBuffer is not allocated\n\r")));
		return;
	}
	else if (!VirtualCopy((PVOID)m_VirtualFrameBuffer, (PVOID)gdwLCDVirtualFrameBase, (0x40000), PAGE_READWRITE | PAGE_NOCACHE))
	{
	    RETAILMSG(0, (TEXT("m_VirtualFrameBuffer is not mapped\n\r")));
    	VirtualFree((PVOID)m_VirtualFrameBuffer, 0, MEM_RELEASE);
    	return;
	}

	RETAILMSG(0, (TEXT("m_VirtualFrameBuffer is mapped at %x(PHY : %x)\n\r"), m_VirtualFrameBuffer, gdwLCDVirtualFrameBase));
	RETAILMSG(0, (TEXT("Clearing frame buffer !!!\n\r")));
	
	ptr = (WORD *)m_VirtualFrameBuffer;

	// clear rest of frame buffer out
	for (index = 0; index < 320*240; index++)
	{
		if(index < 3200)
		{
			ptr[index] = 0xf800;
		}
		else if(index < 6400)
		{
			ptr[index] = 0x07e0;
		}
		else if(index < 9600)
		{
			ptr[index] = 0x001f;
		}
		else
		{
			ptr[index] = 0xffff;
		}
	}

	RETAILMSG(0, (_T("--S3C2410DISP::InitializeHardware\r\n")));
}

SCODE S3C2410DISP::SetMode (INT modeId, HPALETTE *palette)
{
	RETAILMSG(0, (TEXT("++S3C2410DISP::SetMode\r\n")));

	if (modeId != 0)
	{
		RETAILMSG(0, (TEXT("S3C2410DISP::SetMode Want mode %d, only have mode 0\r\n"),modeId));
		return	E_INVALIDARG;
	}

	if (palette)
	{
		*palette = EngCreatePalette (PAL_BITFIELDS, 0, NULL, gBitMasks[0], gBitMasks[1], gBitMasks[2]);
	}

	RETAILMSG(0, (TEXT("--S3C2410DISP::SetMode\r\n")));

	return S_OK;
}

SCODE S3C2410DISP::GetModeInfo(GPEMode *mode,	INT modeNumber)
{
	RETAILMSG(0, (TEXT("++S3C2410DISP::GetModeInfo\r\n")));

	if (modeNumber != 0)
	{
		return E_INVALIDARG;
	}

	*mode = m_ModeInfo;

	RETAILMSG(0, (TEXT("--S3C2410DISP::GetModeInfo\r\n")));

	return S_OK;
}

int	S3C2410DISP::NumModes()
{
	RETAILMSG(0, (TEXT("++S3C2410DISP::NumModes\r\n")));
	RETAILMSG(0, (TEXT("--S3C2410DISP::NumModes\r\n")));
	return	1;
}

void	S3C2410DISP::CursorOn (void)
{
	UCHAR	*ptrScreen = (UCHAR*)m_pPrimarySurface->Buffer();
	UCHAR	*ptrLine;
	UCHAR	*cbsLine;
#ifndef ROTATE
	UCHAR	*xorLine;
	UCHAR	*andLine;
#endif //!ROTATE	
	int		x, y;

	if (!m_CursorForcedOff && !m_CursorDisabled && !m_CursorVisible)
	{
#ifdef ROTATE
		RECTL rSave;
		int   iRotate;
#endif //ROTATE

		if (!m_CursorBackingStore)
		{
			RETAILMSG(0, (TEXT("S3C2410DISP::CursorOn - No backing store available\r\n")));
			return;
		}
#ifdef ROTATE
		rSave = m_CursorRect;
		RotateRectl(&m_CursorRect);
#endif //ROTATE

		for (y = m_CursorRect.top; y < m_CursorRect.bottom; y++)
		{
			if (y < 0)
			{
				continue;
			}
#ifdef ROTATE
			if (y >= m_nScreenHeightSave)
#else
			if (y >= m_nScreenHeight)
#endif //ROTATE			
			{
				break;
			}

			ptrLine = &ptrScreen[y * m_pPrimarySurface->Stride()];
			cbsLine = &m_CursorBackingStore[(y - m_CursorRect.top) * (m_CursorSize.x * (m_colorDepth >> 3))];
#ifndef ROTATE
			xorLine = &m_CursorXorShape[(y - m_CursorRect.top) * m_CursorSize.x];
			andLine = &m_CursorAndShape[(y - m_CursorRect.top) * m_CursorSize.x];
#endif //!ROTATE

			for (x = m_CursorRect.left; x < m_CursorRect.right; x++)
			{
				if (x < 0)
				{
					continue;
				}
#ifdef ROTATE
				if (x >= m_nScreenWidthSave)
#else
				if (x >= m_nScreenWidth)
#endif //!ROTATE				
				{
					break;
				}
#ifdef ROTATE
				switch (m_iRotate)
				{
					case DMDO_0:
						iRotate = (y - m_CursorRect.top)*m_CursorSize.x + x - m_CursorRect.left;
						break;
					case DMDO_90:
						iRotate = (x - m_CursorRect.left)*m_CursorSize.x + m_CursorSize.y - 1 - (y - m_CursorRect.top);   
						break;
					case DMDO_180:
						iRotate = (m_CursorSize.y - 1 - (y - m_CursorRect.top))*m_CursorSize.x + m_CursorSize.x - 1 - (x - m_CursorRect.left);
						break;
					case DMDO_270:
						iRotate = (m_CursorSize.x -1 - (x - m_CursorRect.left))*m_CursorSize.x + y - m_CursorRect.top;
						break;
					default:
					    iRotate = (y - m_CursorRect.top)*m_CursorSize.x + x - m_CursorRect.left;
						break;
				}
#endif //ROTATE					
				cbsLine[(x - m_CursorRect.left) * (m_colorDepth >> 3)] = ptrLine[x * (m_colorDepth >> 3)];
#ifdef ROTATE
				ptrLine[x * (m_colorDepth >> 3)] &= m_CursorAndShape[iRotate];
				ptrLine[x * (m_colorDepth >> 3)] ^= m_CursorXorShape[iRotate];
#else 
				ptrLine[x * (m_colorDepth >> 3)] &= andLine[x - m_CursorRect.left];
				ptrLine[x * (m_colorDepth >> 3)] ^= xorLine[x - m_CursorRect.left];
#endif //ROTATE				
				if (m_colorDepth > 8)
				{
					cbsLine[(x - m_CursorRect.left) * (m_colorDepth >> 3) + 1] = ptrLine[x * (m_colorDepth >> 3) + 1];
#ifdef ROTATE
       		        ptrLine[x * (m_colorDepth >> 3) + 1] &= m_CursorAndShape[iRotate];
					ptrLine[x * (m_colorDepth >> 3) + 1] ^= m_CursorXorShape[iRotate];				
#else
					ptrLine[x * (m_colorDepth >> 3) + 1] &= andLine[x - m_CursorRect.left];
					ptrLine[x * (m_colorDepth >> 3) + 1] ^= xorLine[x - m_CursorRect.left];
#endif //ROTATE					
					if (m_colorDepth > 16)
					{
						cbsLine[(x - m_CursorRect.left) * (m_colorDepth >> 3) + 2] = ptrLine[x * (m_colorDepth >> 3) + 2];
#ifdef ROTATE
						ptrLine[x * (m_colorDepth >> 3) + 2] &= m_CursorAndShape[iRotate];
						ptrLine[x * (m_colorDepth >> 3) + 2] ^= m_CursorXorShape[iRotate];
#else
						ptrLine[x * (m_colorDepth >> 3) + 2] &= andLine[x - m_CursorRect.left];
						ptrLine[x * (m_colorDepth >> 3) + 2] ^= xorLine[x - m_CursorRect.left];
#endif //ROTATE						
					}
				}
			}
		}
#ifdef ROTATE
		m_CursorRect = rSave;
#endif 
		m_CursorVisible = TRUE;
	}
}

void	S3C2410DISP::CursorOff (void)
{
	UCHAR	*ptrScreen = (UCHAR*)m_pPrimarySurface->Buffer();
	UCHAR	*ptrLine;
	UCHAR	*cbsLine;
	int		x, y;

	if (!m_CursorForcedOff && !m_CursorDisabled && m_CursorVisible)
	{
#ifdef ROTATE
		RECTL rSave;
#endif //ROTATE

		if (!m_CursorBackingStore)
		{
			RETAILMSG(0, (TEXT("S3C2410DISP::CursorOff - No backing store available\r\n")));
			return;
		}
#ifdef ROTATE
		rSave = m_CursorRect;
		RotateRectl(&m_CursorRect);
#endif //ROTATE

		for (y = m_CursorRect.top; y < m_CursorRect.bottom; y++)
		{
			// clip to displayable screen area (top/bottom)
			if (y < 0)
			{
				continue;
			}
#ifndef ROTATE
			if (y >= m_nScreenHeight)
#else 
			if (y >= m_nScreenHeightSave)
#endif //!ROTATE
			{
				break;
			}

			ptrLine = &ptrScreen[y * m_pPrimarySurface->Stride()];
			cbsLine = &m_CursorBackingStore[(y - m_CursorRect.top) * (m_CursorSize.x * (m_colorDepth >> 3))];

			for (x = m_CursorRect.left; x < m_CursorRect.right; x++)
			{
				// clip to displayable screen area (left/right)
				if (x < 0)
				{
					continue;
				}
#ifndef ROTATE
				if (x >= m_nScreenWidth)
#else
				if (x>= m_nScreenWidthSave)
#endif //!ROTATE
				{
					break;
				}

				ptrLine[x * (m_colorDepth >> 3)] = cbsLine[(x - m_CursorRect.left) * (m_colorDepth >> 3)];
				if (m_colorDepth > 8)
				{
					ptrLine[x * (m_colorDepth >> 3) + 1] = cbsLine[(x - m_CursorRect.left) * (m_colorDepth >> 3) + 1];
					if (m_colorDepth > 16)
					{
						ptrLine[x * (m_colorDepth >> 3) + 2] = cbsLine[(x - m_CursorRect.left) * (m_colorDepth >> 3) + 2];
					}
				}
			}
		}
#ifdef ROTATE
		m_CursorRect = rSave;
#endif //ROTATE
		m_CursorVisible = FALSE;
	}
}

SCODE	S3C2410DISP::SetPointerShape(GPESurf *pMask, GPESurf *pColorSurf, INT xHot, INT yHot, INT cX, INT cY)
{
	UCHAR	*andPtr;		// input pointer
	UCHAR	*xorPtr;		// input pointer
	UCHAR	*andLine;		// output pointer
	UCHAR	*xorLine;		// output pointer
	char	bAnd;
	char	bXor;
	int		row;
	int		col;
	int		i;
	int		bitMask;

	RETAILMSG(0, (TEXT("S3C2410DISP::SetPointerShape(0x%X, 0x%X, %d, %d, %d, %d)\r\n"),pMask, pColorSurf, xHot, yHot, cX, cY));

	// turn current cursor off
	CursorOff();

	// release memory associated with old cursor
	if (m_CursorBackingStore)
	{
		delete (void*)m_CursorBackingStore;
		m_CursorBackingStore = NULL;
	}
	if (m_CursorXorShape)
	{
		delete (void*)m_CursorXorShape;
        m_CursorXorShape = NULL;
	}
	if (m_CursorAndShape)
	{
		delete (void*)m_CursorAndShape;
        m_CursorAndShape = NULL;
	}

	if (!pMask)							// do we have a new cursor shape
	{
		m_CursorDisabled = TRUE;		// no, so tag as disabled
	}
	else
	{
		m_CursorDisabled = FALSE;		// yes, so tag as not disabled

		// allocate memory based on new cursor size
        m_CursorBackingStore = new UCHAR[(cX * (m_colorDepth >> 3)) * cY];
        m_CursorXorShape = new UCHAR[cX * cY];
        m_CursorAndShape = new UCHAR[cX * cY];

        if (!m_CursorXorShape || !m_CursorAndShape)
        {
            return(ERROR_NOT_ENOUGH_MEMORY);
        }

		// store size and hotspot for new cursor
		m_CursorSize.x = cX;
		m_CursorSize.y = cY;
		m_CursorHotspot.x = xHot;
		m_CursorHotspot.y = yHot;

		andPtr = (UCHAR*)pMask->Buffer();
		xorPtr = (UCHAR*)pMask->Buffer() + (cY * pMask->Stride());

		// store OR and AND mask for new cursor
		for (row = 0; row < cY; row++)
		{
			andLine = &m_CursorAndShape[cX * row];
			xorLine = &m_CursorXorShape[cX * row];

			for (col = 0; col < cX / 8; col++)
			{
				bAnd = andPtr[row * pMask->Stride() + col];
				bXor = xorPtr[row * pMask->Stride() + col];

				for (bitMask = 0x0080, i = 0; i < 8; bitMask >>= 1, i++)
				{
					andLine[(col * 8) + i] = bAnd & bitMask ? 0xFF : 0x00;
					xorLine[(col * 8) + i] = bXor & bitMask ? 0xFF : 0x00;
				}
			}
		}
	}

	return	S_OK;
}

SCODE	S3C2410DISP::MovePointer(INT xPosition, INT yPosition)
{
	RETAILMSG(0, (TEXT("S3C2410DISP::MovePointer(%d, %d)\r\n"), xPosition, yPosition));

	CursorOff();

	if (xPosition != -1 || yPosition != -1)
	{
		// compute new cursor rect
		m_CursorRect.left = xPosition - m_CursorHotspot.x;
		m_CursorRect.right = m_CursorRect.left + m_CursorSize.x;
		m_CursorRect.top = yPosition - m_CursorHotspot.y;
		m_CursorRect.bottom = m_CursorRect.top + m_CursorSize.y;

		CursorOn();
	}

	return	S_OK;
}

void	S3C2410DISP::WaitForNotBusy(void)
{
	RETAILMSG(0, (TEXT("S3C2410DISP::WaitForNotBusy\r\n")));
	return;
}

int		S3C2410DISP::IsBusy(void)
{
	RETAILMSG(0, (TEXT("S3C2410DISP::IsBusy\r\n")));
	return	0;
}

void	S3C2410DISP::GetPhysicalVideoMemory(unsigned long *physicalMemoryBase, unsigned long *videoMemorySize)
{
	RETAILMSG(0, (TEXT("S3C2410DISP::GetPhysicalVideoMemory\r\n")));

	*physicalMemoryBase = gdwLCDVirtualFrameBase;
	*videoMemorySize = m_cbScanLineLength * m_nScreenHeight;
}

SCODE	S3C2410DISP::AllocSurface(GPESurf **surface, INT width, INT height, EGPEFormat format, INT surfaceFlags)
{
	RETAILMSG(0, (TEXT("S3C2410DISP::AllocSurface\r\n")));

	if (surfaceFlags & GPE_REQUIRE_VIDEO_MEMORY)
	{
		return	E_OUTOFMEMORY;
	}

	// Allocate from system memory
	*surface = new GPESurf(width, height, format);

	if (*surface != NULL)
	{
		// Check that the bits were allocated succesfully
		if (((*surface)->Buffer()) == NULL)
		{
			delete *surface;				// Clean up
		}
		else
		{
			return S_OK;
		}
	}
	return E_OUTOFMEMORY;
}

SCODE	S3C2410DISP::WrappedEmulatedLine (GPELineParms *lineParameters)
{
	SCODE	retval;
	RECT	bounds;
	int		N_plus_1;				// Minor length of bounding rect + 1

	// calculate the bounding-rect to determine overlap with cursor
	if (lineParameters->dN)			// The line has a diagonal component (we'll refresh the bounding rect)
	{
		N_plus_1 = 2 + ((lineParameters->cPels * lineParameters->dN) / lineParameters->dM);
	}
	else
	{
		N_plus_1 = 1;
	}

	switch(lineParameters->iDir)
	{
		case 0:
			bounds.left = lineParameters->xStart;
			bounds.top = lineParameters->yStart;
			bounds.right = lineParameters->xStart + lineParameters->cPels + 1;
			bounds.bottom = bounds.top + N_plus_1;
			break;
		case 1:
			bounds.left = lineParameters->xStart;
			bounds.top = lineParameters->yStart;
			bounds.bottom = lineParameters->yStart + lineParameters->cPels + 1;
			bounds.right = bounds.left + N_plus_1;
			break;
		case 2:
			bounds.right = lineParameters->xStart + 1;
			bounds.top = lineParameters->yStart;
			bounds.bottom = lineParameters->yStart + lineParameters->cPels + 1;
			bounds.left = bounds.right - N_plus_1;
			break;
		case 3:
			bounds.right = lineParameters->xStart + 1;
			bounds.top = lineParameters->yStart;
			bounds.left = lineParameters->xStart - lineParameters->cPels;
			bounds.bottom = bounds.top + N_plus_1;
			break;
		case 4:
			bounds.right = lineParameters->xStart + 1;
			bounds.bottom = lineParameters->yStart + 1;
			bounds.left = lineParameters->xStart - lineParameters->cPels;
			bounds.top = bounds.bottom - N_plus_1;
			break;
		case 5:
			bounds.right = lineParameters->xStart + 1;
			bounds.bottom = lineParameters->yStart + 1;
			bounds.top = lineParameters->yStart - lineParameters->cPels;
			bounds.left = bounds.right - N_plus_1;
			break;
		case 6:
			bounds.left = lineParameters->xStart;
			bounds.bottom = lineParameters->yStart + 1;
			bounds.top = lineParameters->yStart - lineParameters->cPels;
			bounds.right = bounds.left + N_plus_1;
			break;
		case 7:
			bounds.left = lineParameters->xStart;
			bounds.bottom = lineParameters->yStart + 1;
			bounds.right = lineParameters->xStart + lineParameters->cPels + 1;
			bounds.top = bounds.bottom - N_plus_1;
			break;
		default:
			RETAILMSG(0, (TEXT("Invalid direction: %d\r\n"), lineParameters->iDir));
			return E_INVALIDARG;
	}

	// check for line overlap with cursor and turn off cursor if overlaps
	if (m_CursorVisible && !m_CursorDisabled &&
		m_CursorRect.top < bounds.bottom && m_CursorRect.bottom > bounds.top &&
		m_CursorRect.left < bounds.right && m_CursorRect.right > bounds.left)
	{
		CursorOff();
		m_CursorForcedOff = TRUE;
	}

	// do emulated line
	retval = EmulatedLine (lineParameters);

	// se if cursor was forced off because of overlap with line bouneds and turn back on
	if (m_CursorForcedOff)
	{
		m_CursorForcedOff = FALSE;
		CursorOn();
	}

	return	retval;

}

SCODE	S3C2410DISP::Line(GPELineParms *lineParameters, EGPEPhase phase)
{
	RETAILMSG(0, (TEXT("S3C2410DISP::Line\r\n")));

	if (phase == gpeSingle || phase == gpePrepare)
	{

		if ((lineParameters->pDst != m_pPrimarySurface))
		{
			lineParameters->pLine = EmulatedLine;
		}
		else
		{
			lineParameters->pLine = (SCODE (GPE::*)(struct GPELineParms *)) WrappedEmulatedLine;
		}
	}
	return S_OK;
}

SCODE	S3C2410DISP::BltPrepare(GPEBltParms *blitParameters)
{
	RECTL	rectl;

	RETAILMSG(0, (TEXT("S3C2410DISP::BltPrepare\r\n")));

	// default to base EmulatedBlt routine
	blitParameters->pBlt = EmulatedBlt;

	// see if we need to deal with cursor
	if (m_CursorVisible && !m_CursorDisabled)
	{
		// check for destination overlap with cursor and turn off cursor if overlaps
		if (blitParameters->pDst == m_pPrimarySurface)	// only care if dest is main display surface
		{
			if (blitParameters->prclDst != NULL)		// make sure there is a valid prclDst
			{
				rectl = *blitParameters->prclDst;		// if so, use it
			}
			else
			{
				rectl = m_CursorRect;					// if not, use the Cursor rect - this forces the cursor to be turned off in this case
			}

			if (m_CursorRect.top < rectl.bottom && m_CursorRect.bottom > rectl.top &&
				m_CursorRect.left < rectl.right && m_CursorRect.right > rectl.left)
			{
				CursorOff();
				m_CursorForcedOff = TRUE;
			}
		}

		// check for source overlap with cursor and turn off cursor if overlaps
		if (blitParameters->pSrc == m_pPrimarySurface)	// only care if source is main display surface
		{
			if (blitParameters->prclSrc != NULL)		// make sure there is a valid prclSrc
			{
				rectl = *blitParameters->prclSrc;		// if so, use it
			}
			else
			{
				rectl = m_CursorRect;					// if not, use the CUrsor rect - this forces the cursor to be turned off in this case
			}
			if (m_CursorRect.top < rectl.bottom && m_CursorRect.bottom > rectl.top &&
				m_CursorRect.left < rectl.right && m_CursorRect.right > rectl.left)
			{
				CursorOff();
				m_CursorForcedOff = TRUE;
			}
		}
	}

#ifdef ROTATE
    if (m_iRotate && (blitParameters->pDst == m_pPrimarySurface || blitParameters->pSrc == m_pPrimarySurface))
    {
        blitParameters->pBlt = (SCODE (GPE::*)(GPEBltParms *))EmulatedBltRotate;
    }
#endif //ROTATE

	#ifdef CLEARTYPE
	if (((blitParameters->rop4 & 0xffff) == 0xaaf0 ) && (blitParameters->pMask->Format() == gpe8Bpp))
	{
	    switch (m_colorDepth)
	    {
	    case 16:
	 	blitParameters->pBlt = (SCODE (GPE::*)(struct GPEBltParms *)) ClearTypeBlt::ClearTypeBltDst16;
		return S_OK;
	    case 24:
blitParameters->pBlt = (SCODE (GPE::*)(struct GPEBltParms *)) ClearTypeBlt::ClearTypeBltDst24;
		return S_OK;
	    case 32:
blitParameters->pBlt = (SCODE (GPE::*)(struct GPEBltParms *)) ClearTypeBlt::ClearTypeBltDst32;
		return S_OK;
	    default:
		break;
	    }
	}
#endif //CLEARTYPE

	// see if there are any optimized software blits available
	EmulatedBltSelect02(blitParameters);
	EmulatedBltSelect08(blitParameters);
	EmulatedBltSelect16(blitParameters);

	return S_OK;
}

SCODE	S3C2410DISP::BltComplete(GPEBltParms *blitParameters)
{
	RETAILMSG(0, (TEXT("S3C2410DISP::BltComplete\r\n")));

	// see if cursor was forced off because of overlap with source or destination and turn back on
	if (m_CursorForcedOff)
	{
		m_CursorForcedOff = FALSE;
		CursorOn();
	}

	return S_OK;
}

INT		S3C2410DISP::InVBlank(void)
{
	RETAILMSG(0, (TEXT("S3C2410DISP::InVBlank\r\n")));
	return 0;
}

SCODE	S3C2410DISP::SetPalette(const PALETTEENTRY *source, USHORT firstEntry, USHORT numEntries)
{
	RETAILMSG(0, (TEXT("S3C2410DISP::SetPalette\r\n")));

	if (firstEntry < 0 || firstEntry + numEntries > 256 || source == NULL)
	{
		return	E_INVALIDARG;
	}

	return	S_OK;
}

ULONG	S3C2410DISP::GetGraphicsCaps()
{
    
#ifdef  CLEARTYPE
	return	GCAPS_GRAY16 | GCAPS_CLEARTYPE;
#else
	return  GCAPS_GRAY16;
#endif 
}

void	RegisterDDHALAPI(void)
{
	return;	// no DDHAL support
}

#if defined(CLEARTYPE) || defined(ROTATE)
extern GetGammaValue(ULONG * pGamma);
extern SetGammaValue(ULONG ulGamma, BOOL bUpdateReg);

ULONG  S3C2410DISP::DrvEscape(
                        SURFOBJ *pso,
                        ULONG    iEsc,
                        ULONG    cjIn,
                        PVOID    pvIn,
                        ULONG    cjOut,
                        PVOID    pvOut)
{
    if (iEsc == DRVESC_GETGAMMAVALUE)
    {
	return GetGammaValue((ULONG *)pvOut);
    }
    else if (iEsc == DRVESC_SETGAMMAVALUE)
    {
	return SetGammaValue(cjIn, *(BOOL *)pvIn);
    }

#ifdef ROTATE
    if (iEsc == DRVESC_GETSCREENROTATION)
    {
        *(int *)pvOut = ((DMDO_0 | DMDO_90 | DMDO_180 | DMDO_270) << 8) | ((BYTE)m_iRotate);
        return DISP_CHANGE_SUCCESSFUL; 
    }
    else if (iEsc == DRVESC_SETSCREENROTATION)
    {
        if ((cjIn == DMDO_0) ||
           (cjIn == DMDO_90) ||
           (cjIn == DMDO_180) ||
           (cjIn == DMDO_270) )
           {
               return DynRotate(cjIn);
           }
        return DISP_CHANGE_BADMODE;
    }
#endif //ROTATE & ROTATE
    
    return 0;
}
#endif //CLEARTYPE

ULONG *APIENTRY DrvGetMasks(DHPDEV dhpdev)
{
	return gBitMasks;
}

#ifdef ROTATE
void S3C2410DISP::SetRotateParms()
{
    int iswap;
    switch(m_iRotate)
    {
    case DMDO_0:
		m_nScreenHeightSave = m_nScreenHeight;
		m_nScreenWidthSave = m_nScreenWidth;
		break;
    case DMDO_180:
		m_nScreenHeightSave = m_nScreenHeight;
		m_nScreenWidthSave = m_nScreenWidth;
		break;
	case DMDO_90:
	case DMDO_270:
		iswap = m_nScreenHeight;
		m_nScreenHeight = m_nScreenWidth;
		m_nScreenWidth = iswap;
	    m_nScreenHeightSave = m_nScreenWidth;
	    m_nScreenWidthSave = m_nScreenHeight;
		break;
	default:
	  	m_nScreenHeightSave = m_nScreenHeight;
		m_nScreenWidthSave = m_nScreenWidth;
		break;
    }
	return;
}

LONG S3C2410DISP::DynRotate(int angle)
{
    GPESurfRotate *pSurf = (GPESurfRotate *)m_pPrimarySurface;
	if (angle == m_iRotate)
		return DISP_CHANGE_SUCCESSFUL;

	m_iRotate = angle;

	switch(m_iRotate)
    {
    case DMDO_0:
    case DMDO_180:
		m_nScreenHeight = m_nScreenHeightSave;
		m_nScreenWidth = m_nScreenWidthSave;
		break;
	case DMDO_90:
	case DMDO_270:
		m_nScreenHeight = m_nScreenWidthSave;
		m_nScreenWidth = m_nScreenHeightSave;
		break;
    }

	m_pMode->width = m_nScreenWidth;
	m_pMode->height = m_nScreenHeight;
	pSurf->SetRotation(m_nScreenWidth, m_nScreenHeight, angle);

	return DISP_CHANGE_SUCCESSFUL;
}
#endif //ROTATE
