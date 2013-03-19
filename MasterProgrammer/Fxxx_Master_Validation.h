//-----------------------------------------------------------------------------
// Fxxx_Master_Validation.h
//-----------------------------------------------------------------------------
// Copyright (C) 2010 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// [*** TEMPLATE INFORMATION ***]
// Filename: Fxxx_Master_Validation.h - DO NOT change this file name.
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

#ifndef _FXXX_MASTER_VALIDATION_H_
#define _FXXX_MASTER_VALIDATION_H_

//-----------------------------------------------------------------------------
// Global CONSTANTS - Available to all modules
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Enums, Structs, Unions and Typedefs - Available to all modules
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// "Public" Global Variables - Externs - Available to all modules
//-----------------------------------------------------------------------------
extern U8 Last_Error;

//-----------------------------------------------------------------------------
// "Public" Function Prototypes - Externs - Available to all modules
//-----------------------------------------------------------------------------
void TGT_Validate_Response (U8 response, U8 command);
void SRC_Validate_Response (U8 response, U8 command);
void Validate_TGT_SRC_Match (response, U32 *page_addr);
void Validate_Pages_Written (U8 num_pages_from_source);


//-----------------------------------------------------------------------------
// Header file include-once preprocessor directive
//-----------------------------------------------------------------------------

#endif // #ifndef _FXXX_MASTER_VALIDATION_H_

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
