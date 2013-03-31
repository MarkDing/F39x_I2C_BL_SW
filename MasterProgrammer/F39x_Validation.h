//-----------------------------------------------------------------------------
// F39x_Validation.h - SMB_BL - Master_BL_FW
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
#ifndef F39X_VALIDATION_H
#define F39X_VALIDATION_H

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Global Variables - Externs
//-----------------------------------------------------------------------------
extern U8 Last_Error;

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
void TGT_Validate_Response(U8 response, U8 command);
void SRC_Validate_Response(U8 response, U8 command);
void Validate_TGT_SRC_Match(U32 *page_addr);
void Validate_Pages_Written(U8 num_pages_from_source);

//-----------------------------------------------------------------------------
// Header File Preprocessor Directive
//-----------------------------------------------------------------------------
#endif

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
