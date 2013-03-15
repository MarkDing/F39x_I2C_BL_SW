//-----------------------------------------------------------------------------
// Fxxx_TargetBL_InfoBlock.c
//-----------------------------------------------------------------------------
// Copyright (C) 2010 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This should be located at the end of the target bootloader flash area using
// compiler/linker directives.
// 
//
//
// Target:         C8051Fxxx (Any Silicon Labs Flash MCU)
// Tool chain:     Generic
// Command Line:   None
//
//
// Release 1.0 / 01SEP2010 (PKC)
//    -Initial Revision
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>

#include "Fxxx_Target_Config.h"
#include "Fxxx_Target_Interface.h"

#include "Fxxx_TargetBL_Config.h"
#include "Fxxx_TargetBL_Interface.h"

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

/*****************************************
Target Bootloader InfoBlock Format
Byte offset (from last byte) - Field

0  InfoBlock Length
-1  Bootloader FW Version Low (0)
-2  Bootloader FW Version High (1)
-3  Product Code
-4  BL Type
-5  Flash Page Size Code
-6  BL Packet/Buffer Size Code
-7  CRC Type
-8  Application FW Start Addr Low (0)
-9 Application FW Start Addr (1)
-10 Application FW Start Addr High (2)
-11 Application FW End Addr Low (0)
-12 Application FW End Addr (1)
-13 Application FW End Addr High (2)
-14 Device Serial Number Byte0 (0)
-15 Device Serial Number Byte1 (1)
-16 [OPTIONAL] Add BL-specific bytes here
-17 ...
*****************************************/

// Reverse byte order for C:
SEGMENT_VARIABLE(TGT_BL_InfoBlock[TGT_BL_FW_INFOBLOCK_LENGTH], const U8, SEG_CODE) =
{
   // [Optional] Add BL-specific bytes here
   TGT_DEVICE_SERIAL1,
   TGT_DEVICE_SERIAL0,
   ((APP_FW_END_ADDR & 0xFF0000) >> 16),
   ((APP_FW_END_ADDR & 0xFF00) >> 8),
   (APP_FW_END_ADDR & 0xFF),
   ((APP_FW_START_ADDR & 0xFF0000) >> 16),
   ((APP_FW_START_ADDR & 0xFF00) >> 8),
   (APP_FW_START_ADDR & 0xFF),
   TGT_CRC_TYPE,
   ((TGT_BL_PKT_SIZE_CODE << 4)|(TGT_BL_BUF_SIZE_CODE & 0x0F)),
   TGT_FLASH_PAGE_SIZE_CODE,
   TGT_BL_TYPE,
   TGT_PRODUCT_CODE,
   TGT_BL_FW_VERSION_HIGH,
   TGT_BL_FW_VERSION_LOW,
   TGT_BL_FW_INFOBLOCK_LENGTH
};


//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------