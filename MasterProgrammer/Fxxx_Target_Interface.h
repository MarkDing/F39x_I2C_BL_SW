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

#define TGT_CMD_ENTER_BL_MODE       0x10
#define TGT_CMD_GET_INFO            0x20
#define TGT_CMD_ERASE_FLASH_PAGE    0x30
#define TGT_CMD_WRITE_SIGNATURE     0x40
#define TGT_CMD_RESET_MCU           0x50
#define TGT_CMD_GET_PAGE_CRC        0x60
#define TGT_CMD_WRITE_FLASH_BYTES   0x70 

// ---------------------------------
// Target BL Response Codes
// ---------------------------------

#define TGT_RSP_OK                  0x00 // RSP_OK should always be 0
#define TGT_RSP_APP_MODE            0x01
#define TGT_RSP_BL_MODE             0x02
#define TGT_RSP_SIG_NOT_ERASED      0x03
#define TGT_RSP_ADDR_INVALID        0x04
#define TGT_RSP_UNSUPPORTED_CMD     0x05
#define TGT_RSP_PARAMETER_INVALID   0x06
#define TGT_RSP_ERROR               0x80

// -----------------------------------
// Target BL InfoBlock Offsets
// Offsets from last byte of BL space
// BLIBO stands for: 
// Bootloader InfoBlock Offset
// -----------------------------------
#define BLIBO_END_VALUE                   16
#define BLIBO_TGT_DEVICE_SERIAL1          15
#define BLIBO_TGT_DEVICE_SERIAL0          14
#define BLIBO_APP_FW_END_ADDR2            13
#define BLIBO_APP_FW_END_ADDR1            12
#define BLIBO_APP_FW_END_ADDR0            11
#define BLIBO_APP_FW_START_ADDR2          10
#define BLIBO_APP_FW_START_ADDR1          9
#define BLIBO_APP_FW_START_ADDR0          8
#define BLIBO_TGT_CRC_TYPE                7
#define BLIBO_TGT_BL_BUF_SIZE_CODE        6
#define BLIBO_TGT_FLASH_PAGE_SIZE_CODE    5
#define BLIBO_TGT_BL_TYPE                 4
#define BLIBO_TGT_PRODUCT_CODE            3
#define BLIBO_TGT_BL_FW_VERSION_HIGH      2
#define BLIBO_TGT_BL_FW_VERSION_LOW       1
#define BLIBO_TGT_BL_FW_INFOBLOCK_LENGTH  0

// -----------------------------------
// Target App InfoBlock Offsets
// Offsets from last byte of App InfoBlock
// (does not include signature bytes)
// APPIBO stands for: 
// Application InfoBlock Offset
// -----------------------------------
#define APPIBO_END_VALUE                     10
#define APPIBO_TGT_DEVICE_SERIAL1            9
#define APPIBO_TGT_DEVICE_SERIAL0            8
#define APPIBO_TGT_APP_FW_VERSION_HIGH       7
#define APPIBO_TGT_APP_FW_VERSION_LOW        6
#define APPIBO_TGT_CRC_TYPE                  5
#define APPIBO_TGT_TGT_BL_BUF_SIZE_CODE      4
#define APPIBO_TGT_FLASH_PAGE_SIZE_CODE      3
#define APPIBO_TGT_BL_TYPE                   2
#define APPIBO_TGT_PRODUCT_CODE              1
#define APPIBO_TGT_APP_FW_INFOBLOCK_LENGTH   0

// ---------------------------------
// Other definitions
// ---------------------------------

// Byte1 of TGT_ENTER_BL_MODE command:
#define TGT_ENTER_BL_MODE_REQ_RSP_CODE 0x52 // This will request a response
#define TGT_ENTER_BL_MODE_NO_RSP_CODE  0xFF // This will prevent a response

//-----------------------------------------------------------------------------
// Header file include-once preprocessor directive
//-----------------------------------------------------------------------------

#endif // #ifndef _FXXX_TARGET_INTERFACE_H_

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
