//-----------------------------------------------------------------------------
// F39x_SMB.h - SMB_BL - Master_BL_FW
//-----------------------------------------------------------------------------
// Copyright (C) 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program performs as the Master for the CAN bootloader.
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
#ifndef F39X_SMB_H
#define F39X_SMB_H

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

// "request_response" parameter values (for TGT_Enter_BL_Mode function)
#define IGNORE_RESPONSE 0x00
#define REQUEST_RESPONSE 0x01

//------------------------
// Bit Definition Masks
//------------------------

// CAN0STAT
#define BOff  0x80                     // Busoff Status
#define EWarn 0x40                     // Warning Status
#define EPass 0x20                     // Error Passive
#define RxOk  0x10                     // Receive Message Successfully
#define TxOk  0x08                     // Transmitted Message Successfully
#define LEC   0x07                     // Last Error Code


//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
void CAN0_Init (void);

U8 TGT_Enter_BL_Mode (U8 request_response, U8 CAN_dev_addr);
U8 TGT_Get_Info (U8 *target_info);
U8 TGT_Set_Flash_Keys (U8 flash_key0, U8 flash_key1);
U8 TGT_Set_Addr (U8 bank, U32 addr);
U8 TGT_Erase_Page (void);
U8 TGT_Write_Flash (U8 *buf, U16 index, U8 numbytes);
U8 TGT_Get_Page_CRC (U16 *target_page_crc);
U8 TGT_Write_Signature (U8 sig_byte0, U8 sig_byte1, U8 sig_byte2, U8 sig_byte3);
void TGT_SW_Reset (void);

//-----------------------------------------------------------------------------
// Header File Preprocessor Directive
//-----------------------------------------------------------------------------
#endif

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
