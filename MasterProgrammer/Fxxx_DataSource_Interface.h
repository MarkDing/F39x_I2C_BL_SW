//-----------------------------------------------------------------------------
// Fxxx_DataSource_Interface.h
//-----------------------------------------------------------------------------
// Copyright (C) 2010 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// [*** TEMPLATE INFORMATION ***]
// Filename: Fxxx_DataSource_Interface.c - DO NOT change this file name.
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

#ifndef _FXXX_DATASOURCE_INTERFACE_H_
#define _FXXX_DATASOURCE_INTERFACE_H_

//-----------------------------------------------------------------------------
// Global CONSTANTS - Available to all modules
//-----------------------------------------------------------------------------

// ---------------------------------
// DataSource Command Codes
// ---------------------------------
#define SRC_CMD_GET_INFO            0x01
#define SRC_CMD_GET_NEXTPAGE_INFO   0x02
#define SRC_CMD_GET_NEXTPAGE        0x03
#define SRC_CMD_DISP_TGT_INFO       0x04
#define SRC_CMD_DISP_ERROR          0x80
// Note: The SRC_CMD_DISP_ERROR command uses codes 
// 0x80 thru 0x8F, where in 0x8y, y = numbytes1[3:0]

// ---------------------------------
// DataSource Response Codes
// ---------------------------------
#define SRC_RSP_OK               0x00  // RSP_OK should always be 0
#define SRC_RSP_ERROR            0x01
#define SRC_RSP_DATA_END         0x02
#define SRC_RSP_UNKNOWN_CMD      0x03

// ---------------------------------
// DataSource Response Sizes
// ---------------------------------
// #define SRC_RSP_SZ_GET_INFO         1+9+6 // Actual value determined using AppIBLen from hex file
#define SRC_RSP_SZ_GET_NEXTPAGE_INFO   8
// #define SRC_RSP_SZ_GET_NEXTPAGE     (PAGE_SIZE+2) // Page size is specified in SRC Info
#define SRC_RSP_SZ_DISP_TGT_INFO       1
#define SRC_RSP_SZ_DISP_ERROR          1

//-----------------------------------
// Error Codes (for SRC_Disp_Error)
//-----------------------------------
#define ERR_NO_ERRORS            0x00
#define ERR_TGT_INFO_MISMATCH    0x01
#define ERR_TGT_BL_MODE          0x02
#define ERR_TGT_UNEXPECTED_RSP   0x03
#define ERR_TGT_CRC_MISMATCH     0x04

#define ERR_SRC_INFO_MISMATCH    0x05
#define ERR_SRC_CRC_MISMATCH     0x06
#define ERR_SRC_UNEXPECTED_RSP   0x07

#define ERR_TGT_SRC_INFO_MISMATCH   0x08
#define ERR_NUM_PAGES_MISMATCH      0x09

#define ERR_USER_CANCELED_UPDATE    0x0A

//-----------------------------------------------------------------------------
// Header file include-once preprocessor directive
//-----------------------------------------------------------------------------

#endif // #ifndef _FXXX_DATASOURCE_INTERFACE_H_

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
