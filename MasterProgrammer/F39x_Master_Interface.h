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
   SRC_CAN_Device_Addr,
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

// Target MCU Commands
#define TGT_CMD_ENTER_BL_MODE    0x90
#define TGT_CMD_GET_INFO         0x91
#define TGT_CMD_SET_FLASH_KEYS   0x92
#define TGT_CMD_SET_ADDR         0x93
#define TGT_CMD_ERASE_PAGE       0x94
#define TGT_CMD_WRITE_FLASH      0x95
#define TGT_CMD_GET_PAGE_CRC     0x96
#define TGT_CMD_WRITE_SIGNATURE  0x97
#define TGT_CMD_SW_RESET         0x98

// Target MCU Response Codes
#define TGT_RSP_OK               0x60
#define TGT_RSP_ERROR            0x61
#define TGT_RSP_BL_MODE          0x62
#define TGT_RSP_APP_MODE         0x63

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

//---------------------------------------------
// CAN-specific Constants
//---------------------------------------------

// Size of CAN Messages
#define MESSAGE_SIZE   8            // Range is 1-8 bytes
#define TGT_MAX_BYTES_PER_WRITE MESSAGE_SIZE

// Note: The master and target message IDs need to be mirrored (TX vs RX)

#ifdef MASTER_MCU_BL

   // Message Objects to use (range 0-31)
   #define MO_TX_BL_CMD    0x10
   #define MO_TX_BL_WRITE8 0x11
   #define MO_RX_BL_RSP    0x12

   // 11-bit Arbitration Message IDs
   #define MSG_ID_TX_BL_CMD      0x0B1 // Transmitted by Master MCU for all BL commands, except:
   #define MSG_ID_TX_BL_WRITE8   0x1B1 // Transmitted by Master MCU for the 8-byte Write command
   #define MSG_ID_RX_BL_RSP      0x2B1 // Received by Master MCU (responses from target MCU)

#else

   // Message Objects to use (range 0-31)
   #define MO_RX_BL_CMD    0x10
   #define MO_RX_BL_WRITE8 0x11
   #define MO_TX_BL_RSP    0x12

   // 11-bit Arbitration Message IDs
   #define MSG_ID_RX_BL_CMD      0x0B1 // Transmitted by Master MCU for all BL commands, except:
   #define MSG_ID_RX_BL_WRITE8   0x1B1 // Transmitted by Master MCU for the 8-byte Write command
   #define MSG_ID_TX_BL_RSP      0x2B1 // Received by Master MCU (responses from target MCU)

#endif

//-----------------------------------------------------------------------------
// Header File Preprocessor Directive
//-----------------------------------------------------------------------------
#endif

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
