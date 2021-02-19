#ifndef __ARGS_H
#define __ARGS_H

//------------------------------------------------------------------------------
//
// File:        args.h
//
// Description: This header file defines device structures and constant related
//              to boot configuration. BOOT_CFG structure defines layout of
//              persistent device information. It is used to control boot
//              process. BSP_ARGS structure defines information passed from
//              boot loader to kernel HAL/OAL. Each structure has version
//              field which should be updated each time when structure layout
//              change.
//
//------------------------------------------------------------------------------

#include "oal_args.h"
#include "oal_kitl.h"


//------------------------------------------------------------------------------
// Bootloader configuration parameters (stored in NOR flash).
//
#define CONFIG_SIGNATURE                    0x12345678

typedef struct
{
    ULONG  Signature;           // Config signature (used to validate).
    USHORT VerMajor;            // Config major version.
    USHORT VerMinor;            // Config minor version.
    ULONG  ConfigFlags;         // General bootloader flags.
    ULONG  IPAddr;              // Device static IP address.
    ULONG  SubnetMask;          // Device subnet mask.
    BYTE   BootDelay;           // Bootloader count-down delay.
    BYTE   LoadDeviceOrder;     // Search order for download devices.
    USHORT CS8900MAC[3];        // Crystal CS8900A MAC address.
    ULONG  LaunchAddress;       // Kernel region launch address.

} BOOT_CFG, *PBOOT_CFG;

#define CONFIG_AUTOBOOT_DEFAULT_COUNT       5
#define CONFIG_FLAGS_AUTOBOOT               (1 << 0)
#define CONFIG_FLAGS_DHCP                   (1 << 1)
#define CONFIG_FLAGS_SAVETOFLASH            (1 << 2)

//------------------------------------------------------------------------------

#define BSP_ARGS_VERSION    1

typedef struct {
    OAL_ARGS_HEADER header;
    UINT8 deviceId[16];                 // Device identification
    OAL_KITL_ARGS kitl;
} BSP_ARGS;

//------------------------------------------------------------------------------

#endif
