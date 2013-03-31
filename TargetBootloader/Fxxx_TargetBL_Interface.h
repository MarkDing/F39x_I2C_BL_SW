//-----------------------------------------------------------------------------
// Fxxx_TargetBL_Interface.h
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

#ifndef _FXXX_TARGETBL_INTERFACE_H_
#define _FXXX_TARGETBL_INTERFACE_H_

//-----------------------------------------------------------------------------
// Global CONSTANTS - Available to all modules
//-----------------------------------------------------------------------------

// The following macros will allow inclusion of specific sections from Comm_Interface files
#define _INCLUDE_TARGET_ONLY_ITEMS_
#define _INCLUDE_TARGET_BL_ONLY_ITEMS_

//-----------------------------------------------------------------------------
// Enums, Structs, Unions and Typedefs - Available to all modules
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// "Public" Global Variables - Externs - Available to all modules
//-----------------------------------------------------------------------------
extern U8 Rx_Buf[TGT_BL_BUF_SIZE];
extern U8 Tx_Buf[TGT_BL_BUF_SIZE];

extern U8 Flash_Key0;
extern U8 Flash_Key1;

//-----------------------------------------------------------------------------
// "Public" Function Prototypes - Externs - Available to all modules
//-----------------------------------------------------------------------------

// This is usually defined in the STARTUP.A51 file.
void START_APPLICATION(void); 

// ---------------------------------
// Device-Specific Functions
// Defined in "Fnnn_TargetBL_DevSpecific.c"
// ---------------------------------
void Init_Device(void);

// ---------------------------------
// Flash Read/Erase/Write Functions
// Defined in "Fnnn_Flash.c"
// ---------------------------------
void FLASH_Modify (U16 addr, U8 byte);
// Note: No flash read function is necessary for non-code banked devices
// (because flash reads are performed directly using a CODE pointer).

// ---------------------------------
// Comm Functions
// Defined in "Fnnn_Comm_*.c"
// Comm-specific interface constants defined in "Fxxx_BLddd_*.h"
// ---------------------------------
//void Comm_Wait_For_Rx_Bytes (U8);
//void Comm_Send_Tx_Bytes (U8);
void SMB0_Op (U8 SMB0_Num_Bytes);
void Set_TX_TGT_RSP_OK (void);
void Set_TX_TGT_RSP_BL_MODE (void);
void Set_TX_TGT_RSP_PARAMETER_INVALID (void);
void Set_TX_TGT_RSP_UNSUPPORTED_CMD (void);
void Set_TX_TGT_BL_VER (void);

// -------------------------------------
// Bootloader Command Processors
// Defined in "Fxxx_TargetBL_Commands.c"
// -------------------------------------
void TGT_Enter_BL_Mode (void);
void TGT_Erase_Page (void);
void TGT_Write_Flash (void);
void TGT_Read_Flash (void);

//-----------------------------------------------------------------------------
// Header file include-once preprocessor directive
//-----------------------------------------------------------------------------

#endif // #ifndef _FXXX_TARGETBL_INTERFACE_H_

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
