//-----------------------------------------------------------------------------
// F39x_Main.c - SMB_BL - Master_BL_FW
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
#include "C8051F390_defs.h"

#ifndef MASTER_MCU_BL
   #define MASTER_MCU_BL
#endif

#include "F39x_Master_Interface.h"

#include "F39x_Main.h"
#include "F39x_Init.h"
#include "F39x_UART.h"
#include "F39x_SMB.h"
#include "F39x_Validation.h"

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------
SBIT (P1_0_SWITCH, SFR_P1, 0);
SBIT (P1_1_LED, SFR_P1, 1);

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
U8 TGT_Response;
U8 SRC_Response;

U8 Pages_Written = 0;

U32 Page_Addr;
U16 SRC_Page_CRC;
U16 TGT_Page_CRC;

U8 LED_State;

SEGMENT_VARIABLE (Page_Buf[PAGE_SIZE+2], U8, SEG_XDATA);
SEGMENT_VARIABLE (TGT_Info[TGT_Enum_End_Value], U8, SEG_XDATA);
SEGMENT_VARIABLE (SRC_Info[SRC_Enum_End_Value], U8, SEG_XDATA);

U16 Temp_Counter;

//-----------------------------------------------------------------------------
// Function Prototypes (Local)
//-----------------------------------------------------------------------------
INTERRUPT_PROTO (TIMER0_ISR, INTERRUPT_TIMER0);
void Set_LED_State (U8 LED_State);

//-----------------------------------------------------------------------------
// Main Routine
//-----------------------------------------------------------------------------
void main (void)
{
   U16 index;

   PCA0MD &= ~0x40;                    // Disable the watchdog timer

   Device_Init ();

   Set_LED_State (LED_OFF);

   EA = 1;                             // Enable global interrupts

   while(1) // Spin forever in this outer loop
   {
      while (Last_Error == 0)          // Wait for switch press...
      {
         if (P1_0_SWITCH == 0)         // Begin bootload if switch is pressed
         {
            Set_LED_State (LED_BLINK_SLOW);  // Indicate progress

            SRC_Response = SRC_Get_Info (SRC_Info);
            SRC_Validate_Response (SRC_Response, SRC_CMD_GET_INFO);
            if (Last_Error != 0)
               break;
#if 0  // Mark.Ding
            // If the target is in app mode, this command will put it in BL mode
            // Ignore response first time because if it is in app mode, there will
            // be no response
            TGT_Enter_BL_Mode (IGNORE_RESPONSE, SRC_Info[SRC_CAN_Device_Addr]);

            // Delay to allow target to reset from app mode to BL mode
            Temp_Counter = 0xFFFF;
            while(Temp_Counter-- != 0);

            // Reissue this command to confirm that it is now in BL mode
            TGT_Response = TGT_Enter_BL_Mode (REQUEST_RESPONSE, SRC_Info[SRC_CAN_Device_Addr]);
            TGT_Validate_Response (TGT_Response, TGT_CMD_ENTER_BL_MODE);
            if (Last_Error != 0)
               break;

            TGT_Response = TGT_Get_Info (TGT_Info);
            TGT_Validate_Response (TGT_Response, TGT_CMD_GET_INFO);
            if (Last_Error != 0)
               break;

            // TODO: Send target info to the PC for display
            SRC_Disp_TGT_Info (TGT_Info[TGT_Info_Rsp_Length], TGT_Info);

            Validate_TGT_SRC_Match (&(Page_Addr));
            // After matching, the last user page address is stored in Page_Addr
            if (Last_Error != 0)
               break;

            // Target and Source are now ready for bootloading
            TGT_Response = TGT_Set_Flash_Keys (FLASH_KEY0, FLASH_KEY1);
            TGT_Validate_Response (TGT_Response, TGT_RSP_OK);
            if (Last_Error != 0)
               break;

            // Set target address to last user page (to erase existing signature)
            // The addr would be pre-set in Page_Addr by previous call Validate_TGT_SRC_Match()
            TGT_Response = TGT_Set_Addr (0, Page_Addr);
            TGT_Validate_Response (TGT_Response, TGT_RSP_OK);
            if (Last_Error != 0)
               break;

            // Erase the last user page
            TGT_Response = TGT_Erase_Page ();
            TGT_Validate_Response (TGT_Response, TGT_RSP_OK);
            if (Last_Error != 0)
               break;
#endif
            // Now that the last app page has been erased, begin the page-by-page bootload process
            Pages_Written = 0;

            while (1)
            {
               // Request start data from source
               SRC_Response = SRC_Get_Page_Info (&Page_Addr, &SRC_Page_CRC);
               SRC_Validate_Response (SRC_Response, SRC_CMD_GET_PAGE_INFO);
               if (Last_Error != 0)
                  break;

               // Exit this loop if no more pages are available from source
               if (SRC_Response == SRC_RSP_DATA_END)
                  break;

               // Request data from source
               SRC_Response = SRC_Get_Page (Page_Buf);
               SRC_Validate_Response (SRC_Response, SRC_CMD_GET_PAGE);  // this will check for CRC match
               if (Last_Error != 0)
                  break;
#if 0 // Mark.Ding
               // Set target page
               // The addr would be pre-set by SRC_Get_Page_Info
               TGT_Response = TGT_Set_Addr (0, Page_Addr);
               TGT_Validate_Response (TGT_Response, TGT_RSP_OK);
               if (Last_Error != 0)
                  break;

               // Erase the target application page
               TGT_Response = TGT_Erase_Page ();
               TGT_Validate_Response (TGT_Response, TGT_RSP_OK);
               if (Last_Error != 0)
                  break;

               for (index = 0; index < PAGE_SIZE; index += TGT_MAX_BYTES_PER_WRITE)
               {
                  TGT_Response = TGT_Write_Flash ((Page_Buf+1), index, TGT_MAX_BYTES_PER_WRITE);
                  TGT_Validate_Response (TGT_Response, TGT_RSP_OK);
                  if (Last_Error != 0)
                     break;
               }
               Pages_Written++;

               if (Last_Error != 0)
                  break;

               TGT_Response = TGT_Get_Page_CRC (&TGT_Page_CRC);
               TGT_Validate_Response (TGT_Response, TGT_CMD_GET_PAGE_CRC); // this will check for CRC match
               if (Last_Error != 0)
                  break;
#endif
            }

            if (Last_Error != 0)
               break;
#if 0 // Mark.Ding
            // Check if we wrote all the pages provided by the source
            // The number of pages is provided in place of the last byte of Page_Addr
            // as a result of the last call to SRC_Get_Page_Info()
            Validate_Pages_Written ((U8)(Page_Addr&0xFF));

            // All pages were written successfully. Now write the signature:
            TGT_Response = TGT_Write_Signature (SIG_BYTE0, SIG_BYTE1, SIG_BYTE2, SIG_BYTE3);
            TGT_Validate_Response (TGT_Response, TGT_RSP_OK);
            if (Last_Error != 0)
               break;

            // Clear flash keys on target
            TGT_Response = TGT_Set_Flash_Keys (0x00, 0x00);
            TGT_Validate_Response (TGT_Response, TGT_RSP_OK);
            if (Last_Error != 0)
               break;

            // Reset target
            TGT_SW_Reset ();
            //TGT_Validate_Response (TGT_Response, TGT_RSP_OK);
#endif
            Set_LED_State (LED_ON);
            // TODO: Send message to PC to indicate successful completion

            //while (P1_0_SWITCH == 0);  // Wait till switch is released
         }
      }

      if (Last_Error != 0)
      {
         Set_LED_State (LED_BLINK_FAST);   // Indicate error
         SRC_Response = SRC_Disp_Info_Code (Last_Error);
         Last_Error = 0;
      }

   }
}

//=============================================================================
// Function Definitions
//=============================================================================

//-----------------------------------------------------------------------------
// Set_LED_State
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
//
//
//-----------------------------------------------------------------------------
void Set_LED_State (U8 state)
{

   LED_State = state;

   switch (LED_State)
   {
      case LED_OFF:
         TR0 = 0;                      // Timer0 OFF
         P1_1_LED = 0;
         break;
      case LED_ON:
         TR0 = 0;                      // Timer0 OFF
         P1_1_LED = 1;
         break;
      case LED_BLINK_SLOW:
         TR0 = 0;                      // Timer0 OFF
         TH0 = TIMER0_RL_SLOW_HIGH;    // Init Timer0 High register
         TL0 = TIMER0_RL_SLOW_LOW;     // Init Timer0 Low register
         TR0 = 1;                      // Timer0 ON
         break;
      case LED_BLINK_FAST:
         TR0 = 0;                      // Timer0 OFF
         TH0 = TIMER0_RL_FAST_HIGH;    // Init Timer0 High register
         TL0 = TIMER0_RL_FAST_LOW;     // Init Timer0 Low register
         TR0 = 1;                      // Timer0 ON
         break;
      default:
         break;
   }

}

//=============================================================================
// Interrupt Service Routines
//=============================================================================

//-----------------------------------------------------------------------------
// TIMER0_ISR
//-----------------------------------------------------------------------------
//
// The ISR is triggered upon Timer0 overflow.
//
//-----------------------------------------------------------------------------

INTERRUPT (TIMER0_ISR, INTERRUPT_TIMER0)
{
   // Timer0 ISR pending flag is automatically cleared by vectoring to ISR

   //if ((LED_State == LED_BLINK_SLOW) || (LED_State == LED_BLINK_FAST))
   {
      P1_1_LED = !P1_1_LED;
   }
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
