/*++
Copyright (c) 2001-2002  BSQUARE Corporation.  All rights reserved.

Module Name:

    platiocl.h

Module Description:

    This module contains defines for platform specific kernel I/O control
    codes.  These I/O control codes are generally used by drivers to gain
    access to resources maintained by the kernel.

Author:

    Benny Ng    11-June-2001

Revision History:

    Edward Sumanaseni 28-Jan-2002
        Added IOCTL_HAL_GET_BUS_CLKS

    Edward Sumanaseni 1-Feb-2002
        Added IOCTL_HAL_STALL_EXECUTION

    Edward Sumanaseni 11-Feb-2002
        Added IOCTL_HAL_BCR_READ, IOCTL_HAL_BCR_MODIFY, and IOCTL_HAL_EDBG_PCMCIA.

    Thomas Satagaj    22-Apr-2002
        Added IOCTL_HAL_WAIT_4_PB   Wait for Push Button.

--*/

#ifndef __PLATIOCL_H__
#define __PLATIOCL_H__

#include <winioctl.h>

#define FUNC_BCEDDK             2048
#define FUNC_PLATFORM           2552

//
// I/O control function code bounds for various groups of kernel I/O controls.
//

#define FUNC_BCEDDK_MIN         FUNC_BCEDDK
#define FUNC_BCEDDK_MAX         (FUNC_BCEDDK + 7)

#define FUNC_PLATFORM_MIN       FUNC_PLATFORM
#define FUNC_PLATFORM_MAX       (FUNC_PLATFORM + 7)

//
// Macro to shorten IOCTL defines.
//

#define KCTL_CODE(FUNC_CODE) \
    CTL_CODE(FILE_DEVICE_HAL, FUNC_CODE, METHOD_NEITHER, FILE_ANY_ACCESS)

//
// BCEDDK support I/O control codes.
//

#define IOCTL_HAL_INTR_CONNECT \
    KCTL_CODE(FUNC_BCEDDK+0)

#define IOCTL_HAL_INTR_DISCONNECT \
    KCTL_CODE(FUNC_BCEDDK+1)

#define IOCTL_HAL_INTR_CONNECT_TIMER \
    KCTL_CODE(FUNC_BCEDDK+2)

#define IOCTL_HAL_INTR_DISCONNECT_TIMER \
    KCTL_CODE(FUNC_BCEDDK+3)

#define IOCTL_HAL_INTR_START_TIMER \
    KCTL_CODE(FUNC_BCEDDK+4)

#define IOCTL_HAL_INTR_STOP_TIMER \
    KCTL_CODE(FUNC_BCEDDK+5)

#define IOCTL_HAL_INTR_QUERY_TIMER \
    KCTL_CODE(FUNC_BCEDDK+6)

#define IOCTL_HAL_STALL_EXECUTION \
    KCTL_CODE(FUNC_BCEDDK+7)

//
// Platform specific I/O control codes.
//

#define IOCTL_HAL_REGISTER_MODIFY \
    KCTL_CODE(FUNC_PLATFORM+0)

#define IOCTL_HAL_REGISTERS_DUMP \
    KCTL_CODE(FUNC_PLATFORM+1)

#define IOCTL_HAL_GET_BUS_CLKS \
    KCTL_CODE(FUNC_PLATFORM+2)

#define IOCTL_HAL_BCR_READ \
    KCTL_CODE(FUNC_PLATFORM+3)      // USHORT as output buffer.

#define IOCTL_HAL_BCR_MODIFY \
    KCTL_CODE(FUNC_PLATFORM+4)

#define IOCTL_HAL_EDBG_PCMCIA \
    KCTL_CODE(FUNC_PLATFORM+5)      // ULONG as output buffer.

#define IOCTL_HAL_WAIT_4_PB \
    KCTL_CODE(FUNC_PLATFORM+6)

//lincw

#define IOCTL_HAL_WINCE_SUSPEND \
    KCTL_CODE(FUNC_PLATFORM+7)

//winceioctrl reboot
#define WINCE_IOCTL_HAL_REBOOT \
    KCTL_CODE(FUNC_PLATFORM+8)
   
 
//changys
#define IOCTL_HAL_GET_UUID32 \
    KCTL_CODE(FUNC_PLATFORM+9)

#define IOCTL_HAL_GET_UUID16 \
    KCTL_CODE(FUNC_PLATFORM+10)
//changys
  

//
// Bit field for easily accessing various parts of the I/O control code.
// These fields are defined in winioctl.h.
//

typedef union _IOCONTROL_CODE {

    struct {

        ULONG   Method : 2;
        ULONG   Function : 12;
        ULONG   Access : 2;
        ULONG   DeviceType : 16;

    };

    ULONG   u;

} IOCONTROL_CODE, *PIOCONTROL_CODE;

//
// Typedefs for structures passed as input and output buffers.
//

//
// The ULONG_MODIFY stucture is used for I/O controls that typically perform
// the following sequence of operations.  It is used as input parameter for
// IOCTL_HAL_REGISTER_MODIFY.
//
// tmp = READ_REGISTER_ULONG(Address);
// tmp |= (tmp & ~Mask) | (Value & Mask);
// WRITE_REGISTER_ULONG(Address, tmp);
//

typedef struct _ULONG_MODIFY {
    PULONG  Address;                    // Address to modify.  (Optional)
    ULONG   Value;                      // Value to set register bits to.
    ULONG   Mask;                       // Mask for bits in Value to set.
} ULONG_MODIFY, *PULONG_MODIFY;

//
// Input parmeter for IOCTL_HAL_BCR_MODIFY.
//

typedef struct _USHORT_MODIFY {
    PUSHORT Address;                    // Address to modify.  (Optional)
    USHORT  Value;                      // Value to set register bits to.
    USHORT  Mask;                       // Mask for bits in Value to set.
} USHORT_MODIFY, *PUSHORT_MODIFY;

//
// Input parameter for IOCTL_HAL_GET_BUS_CLKS.
//

typedef struct _BUS_CLKS {
    ULONG FClk;                         // CPU clock speed
    ULONG HClk;                         // AHB bus clock speed
    ULONG PClk;                         // APB bus clock speed
} BUS_CLKS, *PBUS_CLKS;

//
// Return value in the ULONG output buffer for IOCTL_HAL_EDBG_PCMCIA.
//

#define EDBG_PCMCIA_NONE        0
#define EDBG_PCMCIA_SLOT1       1
#define EDBG_PCMCIA_SLOT2       2

#endif // __PLATIOCL_H__
