//-----------------------------------------------------------------------------
// F39x_UART.h - SMB_BL - Master_BL_FW
//-----------------------------------------------------------------------------
// Copyright (C) 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program performs as the Master for the SMB bootloader.
// PC (Data Source) <--> UART <--> MCU (Master) <--> SMBUS <--> MCU (Target)
//
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

//-----------------------------------------------------------------------------
// Header File Preprocessor Directive
//-----------------------------------------------------------------------------
#ifndef F39X_UART_H
#define F39X_UART_H

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------
#define BAUDRATE 115200
#define ADDL_RX_BYTES_GET_PAGE   2

//-----------------------------------------------------------------------------
// Function Prototypes (Global)
//-----------------------------------------------------------------------------
U8 SRC_Get_Info(U8 *src_info);
U8 SRC_Get_Page_Info(U32 *addr, U16 *src_page_crc);
U8 SRC_Get_Page(U8 *buf);
U8 SRC_Disp_TGT_Info(U8 buf_length, U8 *buf);
U8 SRC_Disp_Info_Code(U8 info_code);

//-----------------------------------------------------------------------------
// Header File Preprocessor Directive
//-----------------------------------------------------------------------------
#endif

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
