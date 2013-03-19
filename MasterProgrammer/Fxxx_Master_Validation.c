//-----------------------------------------------------------------------------
// Fxxx_Master_Validation.c
//-----------------------------------------------------------------------------
// Copyright (C) 2010 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// 
//
// How To Test:
//
// 
//
//
// Target:         C8051Fxxx (Any Silicon Labs Flash MCU)
// Tool chain:     Generic
// Command Line:   None
//
//
// Release 1.0 / 01SEP2010 (PKC)
//    -Initial Revision
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>
#include "Fxxx_SFR_Defs_Wrapper.h"     // This header file will include the
                                       // real MCU register definition file

#include "Fxxx_Master_Config.h"
#include "Fxxx_Master_Interface.h"
#include "Fxxx_Target_Interface.h"
#include "Fxxx_DataSource_Interface.h"
#include "Fxxx_Master_Validation.h"

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
U8 Last_Error = 0;

//-----------------------------------------------------------------------------
// Function Prototypes (Local)
//-----------------------------------------------------------------------------


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
   switch (command)
   {
      case SRC_CMD_GET_INFO:
         // TODO: Add validation here to check SRC_Get_Info
         if (response != SRC_RSP_OK)
         {
            Last_Error = ERR_SRC_UNEXPECTED_RSP;
         }
         break;
      case SRC_CMD_GET_NEXTPAGE_INFO:
         if ( !((response == SRC_RSP_OK) || (response == SRC_RSP_DATA_END)) )
         {
            Last_Error = ERR_SRC_UNEXPECTED_RSP;
         }
         break;
      case SRC_CMD_GET_NEXTPAGE:
         if (response != SRC_RSP_OK)
         {
            Last_Error = ERR_SRC_UNEXPECTED_RSP;
         }
         else
         {
            // TODO: [Optional] Add code here to check the CRC sent by SRC
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
void Validate_TGT_SRC_Match (response, U32 *page_addr)
{
   UU32 TGT_app_fw_end_addr;
   UU32 SRC_app_fw_end_addr;

   if (response == SRC_RSP_OK)
   {
      TGT_app_fw_end_addr.U8[b0] = TGT_Info[TGT_APP_FW_END_ADDR0];
      TGT_app_fw_end_addr.U8[b1] = TGT_Info[TGT_APP_FW_END_ADDR1];
      TGT_app_fw_end_addr.U8[b2] = TGT_Info[TGT_APP_FW_END_ADDR2];
      TGT_app_fw_end_addr.U8[b3] = 0;

      SRC_app_fw_end_addr.U8[b0] = SRC_Info[SRC_APP_FW_END_ADDR0];
      SRC_app_fw_end_addr.U8[b1] = SRC_Info[SRC_APP_FW_END_ADDR1];
      SRC_app_fw_end_addr.U8[b2] = SRC_Info[SRC_APP_FW_END_ADDR2];
      SRC_app_fw_end_addr.U8[b3] = 0;

      if (  (TGT_Info[TGT_PRODUCT_CODE] == SRC_Info[SRC_APP_PRODUCT_CODE]) &&
            (TGT_app_fw_end_addr.U32 == SRC_app_fw_end_addr.U32)
         )
      {
         // Use the last page reported by the target MCU to ensure it gets
         // erased first in the bootloading process
         *page_addr = TGT_app_fw_end_addr.U32;
         TGT_App_FW_End_Addr = TGT_app_fw_end_addr.U32;
      }
      else
      {
         Last_Error = ERR_TGT_SRC_INFO_MISMATCH;
      }
   }
   else if (response == SRC_RSP_DATA_END)
   {
      Last_Error = ERR_USER_CANCELED_UPDATE;
   }
   else
   {
      Last_Error = ERR_SRC_UNEXPECTED_RSP;
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
   if (Pages_Written != num_pages_from_source)
   {
      Last_Error = ERR_NUM_PAGES_MISMATCH;
   }
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
