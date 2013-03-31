//-----------------------------------------------------------------------------
// F39x_Main.h - SMB_BL - Master_BL_FW
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
#ifndef F39X_MAIN_H
#define F39X_MAIN_H

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

// LED State Values
#define LED_OFF      0x00
#define LED_ON       0x01
#define LED_BLINK_SLOW  0x02
#define LED_BLINK_FAST  0x03

//-----------------------------------------------------------------------------
// Global Variables - Externs
//-----------------------------------------------------------------------------
extern U32 Page_Addr;
extern U16 SRC_Page_CRC;
extern U16 TGT_Page_CRC;

extern U8 Pages_Written;

extern U8 LED_State;

SEGMENT_VARIABLE( Page_Buf[PAGE_SIZE + 2], extern U8, SEG_XDATA);
SEGMENT_VARIABLE( TGT_Info[TGT_Enum_End_Value], extern U8, SEG_XDATA);
SEGMENT_VARIABLE( SRC_Info[SRC_Enum_End_Value], extern U8, SEG_XDATA);

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Header File Preprocessor Directive
//-----------------------------------------------------------------------------
#endif

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
