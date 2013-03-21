//-----------------------------------------------------------------------------
// F39x_Validation - SMB_BL - Master_BL_FW
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
// Includes
//-----------------------------------------------------------------------------
#include "compiler_defs.h"
#include "F39x_Validation.h"

#ifndef MASTER_MCU_BL
   #define MASTER_MCU_BL
#endif

#include "F39x_Master_Interface.h"
#include "F39x_Main.h"
#include "F39x_CRC.h"

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
U8 Last_Error = 0;


//=============================================================================
// Function Definitions
//=============================================================================

//-----------------------------------------------------------------------------
// TGT_Validate_Response
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
//
//
//-----------------------------------------------------------------------------
void TGT_Validate_Response (U8 response, U8 command)
{
   switch (command)
   {
      case TGT_CMD_ENTER_BL_MODE:
         if(response != TGT_RSP_BL_MODE) 
         {
            Last_Error = ERR_TGT_BL_MODE;
         }
         break;
      case TGT_CMD_GET_INFO:
         // TODO: Add validation here to check TGT_Info
         if (response != TGT_RSP_OK)
         {
            Last_Error = ERR_TGT_UNEXPECTED_RSP;
         }
         break;
      case TGT_RSP_OK:
         if(response != TGT_RSP_OK)
         {
            Last_Error = ERR_TGT_UNEXPECTED_RSP;
         }
         break;
      case TGT_CMD_GET_PAGE_CRC:
         if(response != TGT_RSP_OK)
         {
            Last_Error = ERR_TGT_UNEXPECTED_RSP;
         }
         else if(SRC_Page_CRC != TGT_Page_CRC)
         {
            Last_Error = ERR_TGT_CRC_MISMATCH;
         }
         break;
      default:
         break;
   }
}

//-----------------------------------------------------------------------------
// SRC_Validate_Response
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
//
//
//-----------------------------------------------------------------------------
void SRC_Validate_Response (U8 response, U8 command)
{
   U16 buf_crc;

   switch (command)
   {
      case SRC_CMD_GET_INFO:
         // TODO: Add validation here to check SRC_Info
         if (response != SRC_RSP_OK)
         {
            Last_Error = ERR_SRC_UNEXPECTED_RSP;
         }
         break;
      case SRC_CMD_GET_PAGE_INFO:
         if ( !((response == SRC_RSP_OK) || (response == SRC_RSP_DATA_END)) )
         {
            Last_Error = ERR_SRC_UNEXPECTED_RSP;
         }
         break;
      case SRC_CMD_GET_PAGE:
         if (response != SRC_RSP_OK)
         {
            Last_Error = ERR_SRC_UNEXPECTED_RSP;
         }
         else
         {
            buf_crc = Get_Buf_CRC (&(Page_Buf[1]), PAGE_SIZE);
            if (buf_crc != SRC_Page_CRC)
            {
               Last_Error = ERR_SRC_CRC_MISMATCH;
            }
         }
         break;
      default:
         break;
   }
}

//-----------------------------------------------------------------------------
// Validate_TGT_SRC_Match
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
//
//
//-----------------------------------------------------------------------------
void Validate_TGT_SRC_Match (U32 *page_addr)
{
   U32 TGT_app_fw_last_page_start_addr;
   U32 SRC_app_fw_last_page_start_addr;

   TGT_app_fw_last_page_start_addr = 
   (
      (TGT_Info[TGT_App_FW_End_Addr2] << 16) | 
      (TGT_Info[TGT_App_FW_End_Addr1] << 8) | 
      TGT_Info[TGT_App_FW_End_Addr0]
   );

   TGT_app_fw_last_page_start_addr = TGT_app_fw_last_page_start_addr & 0xFE00;

   SRC_app_fw_last_page_start_addr = 
   (
      (SRC_Info[SRC_App_FW_End_Addr2] << 16) | 
      (SRC_Info[SRC_App_FW_End_Addr1] << 8) | 
      SRC_Info[SRC_App_FW_End_Addr0]
   );

   SRC_app_fw_last_page_start_addr = SRC_app_fw_last_page_start_addr & 0xFE00;

   if (  (TGT_Info[TGT_MCU_Code] == SRC_Info[SRC_MCU_Code]) &&
         (TGT_app_fw_last_page_start_addr == SRC_app_fw_last_page_start_addr)
      )
   {
      // Use the last page reported by the target MCU to ensure it gets
      // erased first in the bootloading process
      *page_addr = TGT_app_fw_last_page_start_addr;
   }
   else
   {
      Last_Error = ERR_TGT_SRC_INFO_MISMATCH;
   }
}

//-----------------------------------------------------------------------------
// Validate_Pages_Written
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
//
//
//-----------------------------------------------------------------------------
void Validate_Pages_Written (U8 num_pages_from_source)
{
   if (Pages_Written != (num_pages_from_source+1))
   {
      Last_Error = ERR_NUM_PAGES_MISMATCH;
   }
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
