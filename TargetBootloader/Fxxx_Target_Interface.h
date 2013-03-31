//-----------------------------------------------------------------------------
// Fxxx_Target_Interface.h
//-----------------------------------------------------------------------------
// Copyright (C) 2010 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// [*** TEMPLATE INFORMATION ***]
// Filename: Fxxx_TargetBL_Interface.c - DO NOT change this file name.
// xxx => Leave as-is; this means the file is compatible with all C8051F devices.
//
// Content: Publishes the interface used by the bootloader framework. 
// No changes necessary.
//
// Target:         Any Silicon Labs C8051 MCU
// Tool chain:     Generic
// Command Line:   None
//
//
// Release 1.1 / 31Mar2013 (MarkD)
//    -F390 SMBUS Revision
// Release 1.0 / 01SEP2010 (PKC)
//    -Initial Revision
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Header file include-once preprocessor directive
//-----------------------------------------------------------------------------

#ifndef _FXXX_TARGET_INTERFACE_H_
#define _FXXX_TARGET_INTERFACE_H_

//-----------------------------------------------------------------------------
// Global CONSTANTS - Available to all modules
//-----------------------------------------------------------------------------

#define TGT_CMD_FIXED_LENGTH        8

// ---------------------------------
// Target BL Command Codes
// ---------------------------------

#define TGT_CMD_RESET_MCU           0x00
#define TGT_CMD_GET_VERSION         0x01
#define TGT_CMD_ERASE_FLASH_PAGE    0x02
#define TGT_CMD_WRITE_FLASH_BYTES   0x03
#define TGT_CMD_READ_FLASH_BYTES    0x04
#define TGT_CMD_ENTER_BL_MODE       0x05 
// ---------------------------------
// Target BL Response Codes
// ---------------------------------

#define TGT_RSP_OK                  0x00 // RSP_OK should always be 0
#define TGT_RSP_PARAMETER_INVALID   0x01
#define TGT_RSP_UNSUPPORTED_CMD     0x02
#define TGT_RSP_BL_MODE             0x03
#define TGT_RSP_ERROR               0x80

// -----------------------------------  
// Bootloader Version (BCD encoding)
// -----------------------------------
#define TGT_BL_VER                  0x10

//-----------------------------------------------------------------------------
// Header file include-once preprocessor directive
//-----------------------------------------------------------------------------

#endif // #ifndef _FXXX_TARGET_INTERFACE_H_

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
