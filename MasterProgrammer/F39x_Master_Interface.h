//-----------------------------------------------------------------------------
// F39x_Master_Interface.h - SMB_BL - Master_BL_FW
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
#ifndef F50X_MASTER_INTERFACE_H
#define F50X_MASTER_INTERFACE_H

//-----------------------------------------------------------------------------
// Structures, Unions, Enums & Typedefs
//-----------------------------------------------------------------------------
enum Target_Info_Rsp
{
   TGT_Info_Rsp_Length = 0,
   TGT_BL_FW_Ver_Low,
   TGT_BL_FW_Ver_High,
   TGT_MCU_Code,
   TGT_BL_Type,
   TGT_Flash_Page_Size_Code,
   TGT_BL_Buffer_Size_Code,
   TGT_CRC_Type,
   TGT_Encryption_Type,
   TGT_App_FW_Start_Addr0,
   TGT_App_FW_Start_Addr1,
   TGT_App_FW_Start_Addr2,
   TGT_App_FW_End_Addr0,
   TGT_App_FW_End_Addr1,
   TGT_App_FW_End_Addr2,
   TGT_CAN_Device_Addr,
   TGT_App_FW_Ver_Low,
   TGT_App_FW_Ver_High,
   TGT_Enum_End_Value
} ;

enum Source_Info_Rsp
{
   SRC_Info_Rsp_Length = 0,
   SRC_MCU_Code,
   SRC_BL_Type,
   SRC_Flash_Page_Size_Code,
   SRC_App_FW_Ver_Low,
   SRC_App_FW_Ver_High,
   SRC_SMBUS_Device_Addr,
   SRC_App_FW_Start_Addr0,
   SRC_App_FW_Start_Addr1,
   SRC_App_FW_Start_Addr2,
   SRC_App_FW_End_Addr0,
   SRC_App_FW_End_Addr1,
   SRC_App_FW_End_Addr2,
   SRC_Enum_End_Value
} ;

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------
// Target MCU Sizes
#define PAGE_SIZE               512

// User applicate code
#define APP_FW_START_ADDR        0x0200
#define APP_FW_END_ADDR          0x3FFF

// Signature Bytes
#define SIG_BYTE0             0x3D
#define SIG_BYTE1             0xC2
#define SIG_BYTE2             0xA5
#define SIG_BYTE3             0x1B

// Data Source Commands
#define SRC_CMD_GET_INFO         0x80
#define SRC_CMD_GET_PAGE_INFO    0x81
#define SRC_CMD_GET_PAGE         0x82
#define SRC_CMD_DISP_TGT_INFO    0x83
#define SRC_CMD_DISP_INFO_CODE   0x84

// Data Source Response Sizes
#define SRC_CMD_GET_INFO_RX_SZ      14
#define SRC_CMD_GET_PAGE_INFO_RX_SZ 6
#define SRC_CMD_GET_PAGE_RX_SZ      (PAGE_SIZE+2)
#define SRC_CMD_DISP_TGT_INFO_RX_SZ 1
#define SRC_CMD_DISP_INFO_CODE_RX_SZ 1

// Data Source Response Codes
#define SRC_RSP_OK               0x70
#define SRC_RSP_ERROR            0x71
#define SRC_RSP_DATA_END         0x72
#define SRC_RSP_UNKNOWN_CMD      0x73


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


//---------------------------------------------
// Error Codes
//---------------------------------------------
#define ERR_TGT_INFO_MISMATCH    0x01
#define ERR_TGT_BL_MODE          0x02
#define ERR_TGT_UNEXPECTED_RSP   0x03
#define ERR_TGT_CRC_MISMATCH     0x04

#define ERR_SRC_INFO_MISMATCH    0x10
#define ERR_SRC_CRC_MISMATCH     0x11
#define ERR_SRC_UNEXPECTED_RSP   0x12

#define ERR_TGT_SRC_INFO_MISMATCH   0x20
#define ERR_NUM_PAGES_MISMATCH      0x21


//-----------------------------------------------------------------------------
// Header File Preprocessor Directive
//-----------------------------------------------------------------------------
#endif

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
