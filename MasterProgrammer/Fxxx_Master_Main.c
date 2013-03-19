//-----------------------------------------------------------------------------
// Fxxx_Master_Main.c
//-----------------------------------------------------------------------------
// Copyright (C) 2010 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// The main routine for the Master Programmer Firmware.
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
// Global Variables (project-scope)
//-----------------------------------------------------------------------------
U8 Pages_Written;

U32 SRC_Page_CRC;
U32 TGT_Page_CRC;

//-----------------------------------------------------------------------------
// Global Variables (file-scope)
//-----------------------------------------------------------------------------
U16 Delay_Counter;


//-----------------------------------------------------------------------------
// Function Prototypes (Local)
//-----------------------------------------------------------------------------


//=============================================================================
// Main Routine
//=============================================================================
void main (void)
{
   //---------------------------------------
   // Local Variable Definitions
   //---------------------------------------
   U8 TGT_response;
   U8 SRC_response;

   U32 page_addr;

   //---------------------------------------
   // Check pin state and launch example
   //---------------------------------------

   #if EXAMPLE_MASTER_APP == 1
   if (Check_Pin_State () == PIN_ASSERTED)
   {
      Launch_Example_Master_Application ();
   }
   #endif

   //---------------------------------------
   // Perform MCU initialization
   //---------------------------------------

   Disable_Watchdog();
   Init_Device();
   Set_LED_State (LED_ON);
   EA = 1;

   //-------------------------------------------
   // Main Loop
   //-------------------------------------------

   while(1)
   {
      while (Last_Error == 0)
      {
         while (Check_Pin_State () != PIN_ASSERTED); // Wait for user input before starting firmware update 

         Set_LED_State (LED_OFF);  // Indicate progress

         SRC_response = SRC_Get_Info (SRC_Info);
         SRC_Validate_Response (SRC_response, SRC_CMD_GET_INFO);
         if (Last_Error != 0)
            break;

         // If the target is in app mode, this command will put it in BL mode
         // Ignore response first time because if it is in app mode, there will
         // be no response
         TGT_Enter_BL_Mode (TGT_ENTER_BL_MODE_NO_RSP_CODE, SRC_Serial);

         // Delay to allow target to reset from app mode to BL mode
         Delay_Counter = 0xFFFF;
         while(Delay_Counter-- != 0);

         // Reissue this command to confirm that it is now in BL mode
         TGT_response = TGT_Enter_BL_Mode (TGT_ENTER_BL_MODE_REQ_RSP_CODE, SRC_Serial);
         TGT_Validate_Response (TGT_response, TGT_CMD_ENTER_BL_MODE);
         if (Last_Error != 0)
            break;

         TGT_response = TGT_Get_Info (TGT_Info);
         TGT_Validate_Response (TGT_response, TGT_CMD_GET_INFO);
         if (Last_Error != 0)
            break;

         // Send target info to the PC for display
         if (TGT_Info[TGT_APP_INFOBLOCK_LENGTH] != 0)
         {
            SRC_response = SRC_Disp_TGT_Info ((TGT_Info[TGT_BL_INFOBLOCK_LENGTH]+TGT_Info[TGT_APP_INFOBLOCK_LENGTH]), TGT_Info);
         }
         else
         {
            // This will send the app infoblock length byte (which would have been set to 0 elsewhere)
            SRC_response = SRC_Disp_TGT_Info ((TGT_Info[TGT_BL_INFOBLOCK_LENGTH]+1), TGT_Info);
         }

         Validate_TGT_SRC_Match (SRC_response, &(page_addr));
         // After matching, the target app FW end address is stored in page_addr

         if (Last_Error != 0)
            break;

         // Target and Source are now ready for bootloading

         // Set target address to target app FW end addr (to erase existing signature)
         // The addr would be pre-set in page_addr by previous call Validate_TGT_SRC_Match()

         // Erase the last user page
         TGT_response = TGT_Erase_Page (FLASH_KEY0, FLASH_KEY1, page_addr);
         TGT_Validate_Response (TGT_response, TGT_RSP_OK);
         if (Last_Error != 0)
            break;

         // Now that the last app page has been erased, begin the page-by-page bootload process
         Pages_Written = 0;

         while (1)
         {
            // Request start data from source
            SRC_response = SRC_Get_NextPage_Info (&page_addr, &SRC_Page_CRC);
            SRC_Validate_Response (SRC_response, SRC_CMD_GET_NEXTPAGE_INFO);
            if (Last_Error != 0)
               break;

            // Exit this loop if no more pages are available from source
            if (SRC_response == SRC_RSP_DATA_END)
               break;

            // Request data from source
            SRC_response = SRC_Get_NextPage (); // Page data is stored in SRC_Rx_Buf
            SRC_Validate_Response (SRC_response, SRC_CMD_GET_NEXTPAGE);
            if (Last_Error != 0)
               break;

            // Set target page
            // The addr would be pre-set by SRC_Get_NextPage_Info

            // Erase the target application page
            TGT_response = TGT_Erase_Page (FLASH_KEY0, FLASH_KEY1, page_addr);
            TGT_Validate_Response (TGT_response, TGT_RSP_OK);
            if (Last_Error != 0)
               break;

            // Note: The TGT_Write_Flash function will ignore attempts to write to the signature byte addresses
            TGT_response = TGT_Write_Flash ((SRC_Rx_Buf+1), TGT_Page_Size, FLASH_KEY0, FLASH_KEY1, page_addr);
            TGT_Validate_Response (TGT_response, TGT_RSP_OK);
            if (Last_Error != 0)
               break;

            Pages_Written++;

            TGT_response = TGT_Get_Page_CRC (page_addr, &TGT_Page_CRC);
            TGT_Validate_Response (TGT_response, TGT_CMD_GET_PAGE_CRC); // this will check for CRC match
            if (Last_Error != 0)
               break;
         }

         if (Last_Error != 0)
            break;

         // Check if we wrote all the pages provided by the source
         // The number of app FW pages specified (from SRC) is provided in place 
         // of byte0 of page_addr as a result of the last call to SRC_Get_Page_Info()
         Validate_Pages_Written ((U8)(page_addr&0xFF));
         if (Last_Error != 0)
            break;

         // All pages were written successfully. Now write the signature:
         TGT_response = TGT_Write_Signature (FLASH_KEY0, FLASH_KEY1, SIG_BYTE0, SIG_BYTE1);
         TGT_Validate_Response (TGT_response, TGT_RSP_OK);
         if (Last_Error != 0)
            break;

         // Reset target
         TGT_response = TGT_Reset_MCU ();
         TGT_Validate_Response (TGT_response, TGT_RSP_OK);

         Set_LED_State (LED_ON);

         // Send message to PC to indicate successful completion
         SRC_Disp_Error (ERR_NO_ERRORS);
      }

      if (Last_Error != 0)
      {         
         SRC_Disp_Error (Last_Error);

         if (Last_Error == ERR_USER_CANCELED_UPDATE)
         {
            TGT_Reset_MCU (); // If the user canceled the update, reset the target
         }

         // Indicate error
         while (Check_Pin_State () != PIN_ASSERTED)
         {
            Set_LED_State (LED_TOGGLE);   
            // Delay to allow target to reset from app mode to BL mode
            Delay_Counter = 0xF000;
            while(Delay_Counter-- != 0);
         }

         Last_Error = 0;
      }
   }
}


//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
