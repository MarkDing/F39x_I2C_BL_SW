//-----------------------------------------------------------------------------
// F39x_InfoBlock.c - SMB_BL - Target_BL_FW
//-----------------------------------------------------------------------------
// Copyright (C) 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This file contains a set of bytes that will be placed as the last bytes of
// the App FW. The locating is done by using the linker command line.
//
// Locate starting at address (16*1024-512-7-4)=0X3DF5  
//
// Target:         C8051F390
// Tool chain:     Keil C51 9.02 / Keil EVAL C51
// Command Line:   None
//
//
// Release 1.0 / 1Apr2013 (Mark Ding)
//    -Initial Revision
//
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "compiler_defs.h"
#include "C8051F390_defs.h"


//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

/*****************************************
Target App FW InfoBlock Format
Byte offset (from last byte) - Field

0	Signature Byte Low (0) (sig bytes are added by the bootloader after successful programming)
-1	Signature Byte (1)
-2	Signature Byte (2)
-3	Signature Byte High (3)
-4	InfoBlock Length = 7 (for CAN BL; count includes this length byte)
-5	MCU Code
-6	BL Type
-7	Flash Page Size Code
-8	Application FW Version Low (0)
-9	Application FW Version High (1)
-10	BL-specific byte (0) - SMBus Device Addr - Not used for read by FW (only present for hex file image)
*****************************************/

// Defines for use in the InfoBlock
#define TGT_APP_INFOBLOCK_LENGTH 7

#define TGT_MCU_CODE             0x39  // Code for this MCU partnumber
#define TGT_BL_TYPE              0x04  // SMBUS BL
#define TGT_FLASH_PAGE_SIZE_CODE 0x01  // Flash Page Size Code: 1 => 512 bytes
#define APP_FW_VERSION_HIGH      0x01
#define APP_FW_VERSION_LOW       0x01
#define SMBUS_DEVICE_ADDR        0x20
// Signature Bytes
#define SIG_BYTE0             0x3D
#define SIG_BYTE1             0xC2
#define SIG_BYTE2             0xA5
#define SIG_BYTE3             0x1B


// Reverse byte order for Keil:
SEGMENT_VARIABLE(TGT_App_InfoBlock[], const U8, SEG_CODE) =
{
	SMBUS_DEVICE_ADDR,
	APP_FW_VERSION_HIGH,
	APP_FW_VERSION_LOW,
	TGT_FLASH_PAGE_SIZE_CODE,
	TGT_BL_TYPE,
	TGT_MCU_CODE,
	TGT_APP_INFOBLOCK_LENGTH,
	SIG_BYTE3,
	SIG_BYTE2,
	SIG_BYTE1,
	SIG_BYTE0,
};



//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------