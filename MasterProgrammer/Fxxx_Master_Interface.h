//-----------------------------------------------------------------------------
// Fxxx_Master_Interface.h
//-----------------------------------------------------------------------------
// Copyright (C) 2010 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// [*** TEMPLATE INFORMATION ***]
// Filename: Fxxx_Master_Interface.h - DO NOT change this file name.
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

#ifndef _FXXX_MASTER_INTERFACE_H_
#define _FXXX_MASTER_INTERFACE_H_

//-----------------------------------------------------------------------------
// Global CONSTANTS - Available to all modules
//-----------------------------------------------------------------------------

// 'request_response' parameter values for TGT_Enter_BL_Mode
#define IGNORE_RESPONSE    0x01
#define REQUEST_RESPONSE   0x02

// Pin State Values
#define PIN_ASSERTED       0x00
#define PIN_NOT_ASSERTED   0x01

// LED State Values
#define LED_OFF            0x00
#define LED_ON             0x01
#define LED_TOGGLE         0x02

// Flash Write/Erase Key Codes for Target MCU
#define FLASH_KEY0         0xA5
#define FLASH_KEY1         0xF1

// Signature Bytes
#define SIG_BYTE0          0x3D
#define SIG_BYTE1          0xC2

// The following macros will allow inclusion of specific sections from Comm_Interface files
#define _INCLUDE_MASTER_ONLY_ITEMS_

//-----------------------------------------------------------------------------
// Structures, Unions, Enums & Typedefs
//-----------------------------------------------------------------------------
enum Target_Info_Rsp
{
   TGT_BL_INFOBLOCK_LENGTH = 0,
   TGT_BL_FW_VER_LOW,
   TGT_BL_FW_VER_HIGH,
   TGT_PRODUCT_CODE,
   TGT_BL_TYPE,
   TGT_FLASH_PAGE_SIZE_CODE,
   TGT_BL_BUFFER_SIZE_CODE,
   TGT_CRC_TYPE,
   TGT_APP_FW_START_ADDR0,
   TGT_APP_FW_START_ADDR1,
   TGT_APP_FW_START_ADDR2,
   TGT_APP_FW_END_ADDR0,
   TGT_APP_FW_END_ADDR1,
   TGT_APP_FW_END_ADDR2,
   TGT_DEVICE_SERIAL0,
   TGT_DEVICE_SERIAL1,
   TGT_APP_INFOBLOCK_LENGTH,
   TGT_APP_PRODUCT_CODE,
   TGT_APP_BL_TYPE,
   TGT_APP_FLASH_PAGE_SIZE_CODE,
   TGT_APP_BL_BUFFER_SIZE_CODE,
   TGT_APP_CRC_TYPE,   
   TGT_APP_FW_VER_LOW,
   TGT_APP_FW_VER_HIGH,
   TGT_APP_DEVICE_SERIAL0,
   TGT_APP_DEVICE_SERIAL1,
   TGT_ENUM_END_VALUE
} ;

enum Source_Info_Rsp
{
   SRC_INFO_RSP_LENGTH = 0,
   SRC_APP_PRODUCT_CODE,
   SRC_APP_BL_TYPE,
   SRC_APP_FLASH_PAGE_SIZE_CODE,
   SRC_APP_BL_BUFFER_SIZE_CODE,
   SRC_APP_CRC_TYPE,   
   SRC_APP_FW_VER_LOW,
   SRC_APP_FW_VER_HIGH,
   SRC_APP_DEVICE_SERIAL0,
   SRC_APP_DEVICE_SERIAL1,
   SRC_APP_FW_START_ADDR0,
   SRC_APP_FW_START_ADDR1,
   SRC_APP_FW_START_ADDR2,
   SRC_APP_FW_END_ADDR0,
   SRC_APP_FW_END_ADDR1,
   SRC_APP_FW_END_ADDR2,
   SRC_ENUM_END_VALUE
} ;

//-----------------------------------------------------------------------------
// "Public" Global Variables - Externs - Available to all modules
//-----------------------------------------------------------------------------
SEGMENT_VARIABLE (Rx_Buf[TGT_COMM_RX_BUF_LENGTH], extern U8, SEG_XDATA);
SEGMENT_VARIABLE (Tx_Buf[TGT_COMM_TX_BUF_LENGTH], extern U8, SEG_XDATA);

SEGMENT_VARIABLE (SRC_Rx_Buf[SRC_COMM_RX_BUF_LENGTH], extern U8, SEG_XDATA);
SEGMENT_VARIABLE (SRC_Tx_Buf[SRC_COMM_TX_BUF_LENGTH], extern U8, SEG_XDATA);

SEGMENT_VARIABLE (TGT_Info[TGT_ENUM_END_VALUE], extern U8, SEG_XDATA);
SEGMENT_VARIABLE (SRC_Info[TGT_ENUM_END_VALUE], extern U8, SEG_XDATA);

SEGMENT_VARIABLE (SRC_Serial[NUM_SERIAL_BYTES], extern U8, SEG_XDATA);

extern U16 TGT_Page_Size;

extern U8 Pages_Written;

extern U32 SRC_Page_CRC;
extern U32 TGT_Page_CRC;

extern bit TGT_Write_Flash_Data_Bytes;

extern U32 TGT_App_FW_End_Addr;

//-----------------------------------------------------------------------------
// "Public" Function Prototypes - Externs - Available to all modules
//-----------------------------------------------------------------------------

// ---------------------------------
// Device-Specific Functions
// Defined in "Fnnn_TargetBL_DevSpecific.c"
// ---------------------------------
void Disable_Watchdog(void);
void Init_Device(void);

U8 Check_Pin_State (void);
void Set_LED_State (U8 state);

// ---------------------------------
// Comm Functions (Target Comm - CAN0)
// Defined in "Fnnn_Comm_*.c"
// Comm-specific interface constants defined in "Fxxx_BL129_*.h"
// ---------------------------------
void Comm_Init (void);
U8 Comm_Wait_For_Rx_Bytes (U16 numbytes_to_read);
void Comm_Send_Tx_Bytes (U16 numbytes_to_send);

// ---------------------------------
// Comm Functions (DataSource Comm - UART0)
// Defined in "Fnnn_Comm_*.c"
// Comm-specific interface constants defined in "Fxxx_BL128_*.h"
// Note: SRC prefixes have been added to comm function names
// ---------------------------------
void SRC_Comm_Init (void);
U8 SRC_Comm_Wait_For_Rx_Bytes (U16 numbytes_to_read);
void SRC_Comm_Send_Tx_Bytes (U16 numbytes_to_send);

// ---------------------------------------
// Target Command Processors
// Defined in "Fxxx_Master_TGT_Commands.c"
// ---------------------------------------
U8 TGT_Enter_BL_Mode (U8 req_rsp_code, U8 *dev_serial);
U8 TGT_Get_Info (U8 *target_info);
U8 TGT_Erase_Page (U8 flash_key0, U8 flash_key1, U32 addr);
U8 TGT_Write_Flash (U8 *buf, U16 numbytes, U8 flash_key0, U8 flash_key1, U32 addr);
U8 TGT_Get_Page_CRC (U32 addr, U32 *target_page_crc);
U8 TGT_Write_Signature (U8 flash_key0, U8 flash_key1, U8 sig_byte0, U8 sig_byte1);
U8 TGT_Reset_MCU (void);

// ---------------------------------------
// DataSource Command Processors
// Defined in "Fxxx_Master_TGT_Commands.c"
// ---------------------------------------
U8 SRC_Get_Info (U8 *src_info);
U8 SRC_Get_NextPage_Info (U32 *addr, U32 *src_page_crc);
U8 SRC_Get_NextPage (void);
U8 SRC_Disp_TGT_Info (U8 buf_length, U8 *buf);
U8 SRC_Disp_Error (U8 error_code);

// ---------------------------------------
// Optional Example Master Application
// Defined in "Fnnn_Example_*.c"
// ---------------------------------------
#if EXAMPLE_MASTER_APP == 1
void Launch_Example_Master_Application (void);
#endif

//-----------------------------------------------------------------------------
// Header file include-once preprocessor directive
//-----------------------------------------------------------------------------

#endif // #ifndef _FXXX_MASTER_INTERFACE_H_

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
