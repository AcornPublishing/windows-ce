#include <windows.h>
#include <bsp.h>

// SMDK2410 CSP 를 사용하는 경우에만 필요
//
LPCWSTR g_oalIoCtlPlatformType = IOCTL_PLATFORM_TYPE;
LPCWSTR g_oalIoCtlPlatformOEM  = IOCTL_PLATFORM_OEM;
LPCWSTR g_oalIoCtlProcessorVendor = IOCTL_PROCESSOR_VENDOR;
LPCWSTR g_oalIoCtlProcessorName   = IOCTL_PROCESSOR_NAME;
LPCWSTR g_oalIoCtlProcessorCore   = IOCTL_PROCESSOR_CORE;
UINT32 g_oalIoCtlInstructionSet = IOCTL_PROCESSOR_INSTRUCTION_SET;
UINT32 g_oalIoCtlClockSpeed = IOCTL_PROCESSOR_CLOCK_SPEED;
//

const OAL_IOCTL_HANDLER g_oalIoCtlTable[] = {
#include "ioctl_tab.h"
};

// SMDK2410 CSP 를 사용하는 경우에만 필요
//
#define BSP_PREV_DRVGBL_SIG_LOC 0xAC020800
#define BSP_PREV_DRVGBL_SIG     0x45424F54  // "EBOT"
#define BSP_PREV_DRVGBL_MAC     0xAC02080C  
CHAR g_deviceName[EDBG_MAX_DEV_NAMELEN] = "";

VOID OALKitlInitRegistry()
{
}

BOOL OALIoCtlVBridge(
    UINT32 code, VOID *pInpBuffer, UINT32 inpSize, VOID *pOutBuffer,
    UINT32 outSize, UINT32 *pOutSize
) {
    return FALSE;
}

//

void OEMWriteDebugByte(UCHAR ch) 
{
}

int OEMReadDebugByte() 
{
	return 0;
}

void OEMInitDebugSerial(void) 
{
}

void OEMInit()  
{
}

BOOL BSPIntrInit()
{
    OALMSG(OAL_INTR&&OAL_FUNC, (L"+BSPIntrInit\r\n"));
    

    OALMSG(OAL_INTR&&OAL_FUNC, (L"-BSPIntrInit(rc = 1)\r\n"));
    return TRUE;
}

BOOL BSPIntrRequestIrqs(DEVICE_LOCATION *pDevLoc, UINT32 *pCount, UINT32 *pIrqs)
{
    BOOL rc = FALSE;

    OALMSG(OAL_INTR&&OAL_FUNC, (
        L"+BSPIntrRequestIrq(0x%08x->%d/%d/0x%08x/%d, 0x%08x, 0x%08x)\r\n",
        pDevLoc, pDevLoc->IfcType, pDevLoc->BusNumber, pDevLoc->LogicalLoc,
        pDevLoc->Pin, pCount, pIrqs
    ));

    OALMSG(OAL_INTR&&OAL_FUNC, (L"-BSPIntrRequestIrq(rc = %d)\r\n", rc));
    return rc;
}

UINT32 BSPIntrEnableIrq(UINT32 irq)
{
    OALMSG(OAL_INTR&&OAL_VERBOSE, (L"+BSPIntrEnableIrq(%d)\r\n", irq));
    OALMSG(OAL_INTR&&OAL_VERBOSE, (L"-BSPIntrEnableIrq(irq = %d)\r\n", irq));
    return irq;
}

UINT32 BSPIntrDisableIrq(UINT32 irq)
{
    OALMSG(OAL_INTR&&OAL_VERBOSE, (L"+BSPIntrDisableIrq(%d)\r\n", irq));
    OALMSG(OAL_INTR&&OAL_VERBOSE, (L"-BSPIntrDisableIrq(irq = %d\r\n", irq));
    return irq;
}

UINT32 BSPIntrDoneIrq(UINT32 irq)
{
    OALMSG(OAL_INTR&&OAL_VERBOSE, (L"+BSPIntrDoneIrq(%d)\r\n", irq));
    OALMSG(OAL_INTR&&OAL_VERBOSE, (L"-BSPIntrDoneIrq(irq = %d)\r\n", irq));
    return irq;
}

UINT32 BSPIntrActiveIrq(UINT32 irq)
{
    OALMSG(OAL_INTR&&OAL_VERBOSE, (L"+BSPIntrActiveIrq(%d)\r\n", irq));
    OALMSG(OAL_INTR&&OAL_VERBOSE, (L"-BSPIntrActiveIrq(%d)\r\n", irq));
    return irq;
}

VOID* OALArgsQuery(UINT32 type)
{
    VOID *pData = NULL;
    BSP_ARGS *pArgs;

    OALMSG(OAL_ARGS&&OAL_FUNC, (L"+OALArgsQuery(%d)\r\n", type));

    pArgs = (BSP_ARGS*)IMAGE_SHARE_ARGS_UA_START;

    // Depending on required args
    switch (type) {
    case OAL_ARGS_QUERY_DEVID:
        pData = &pArgs->deviceId;
        break;
    case OAL_ARGS_QUERY_KITL:
        pData = &pArgs->kitl;
        break;
    }

    OALMSG(OAL_ARGS&&OAL_FUNC, (L"-OALArgsQuery(pData = 0x%08x)\r\n", pData));
    return pData;
}
