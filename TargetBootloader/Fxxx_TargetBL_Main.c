//-----------------------------------------------------------------------------
// Fxxx_TargetBL_Main.c
//-----------------------------------------------------------------------------
// Copyright (C) 2010 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// The main routine for the Target Bootloader Firmware.
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

#include "Fxxx_Target_Config.h"
#include "Fxxx_Target_Interface.h"

#include "Fxxx_TargetBL_Config.h"
#include "Fxxx_TargetBL_Interface.h"

#include "Fxxx_BL131_SMB0_Interface.h"

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
SBIT(BL_Override_Pin, SFR_P1, 0);

//=============================================================================
// Main Routine
//=============================================================================
void main (void)
{
   //---------------------------------------
   // Check the override pin.
   //---------------------------------------
   if (BL_Override_Pin)
   {
      // If not in BL Override, jump to application
      START_APPLICATION ();
   }

   //-------------------------------------------
   // ** BL Mode ** Initialize MCU and Variables
   //-------------------------------------------
   Init_Device ();

   //-------------------------------------------
   // Main Loop
   //-------------------------------------------

   while(1)
   {
      // Wait until a command is received
      SMB0_Op (6);

      switch (Rx_Buf[0])
      {
         case TGT_CMD_RESET_MCU:
            Set_TX_TGT_RSP_OK ();
            SMB0_Op (1);
            RSTSRC = 0x12;          // Initiate software reset with vdd monitor enabled  
            break;

         case TGT_CMD_ERASE_FLASH_PAGE:
            TGT_Erase_Page ();
            break;

         case TGT_CMD_WRITE_FLASH_BYTES:
            TGT_Write_Flash ();
            break;

         case TGT_CMD_READ_FLASH_BYTES:
            TGT_Read_Flash ();
            break;

         default:
            Set_TX_TGT_RSP_UNSUPPORTED_CMD ();
            SMB0_Op (1);
            break;
      }

      // Set flash keys to 0
      Flash_Key0 = 0;
      Flash_Key1 = 0;
   }
}

//-----------------------------------------------------------------------------
// Set_TX_TGT_RSP_OK
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
// Sets TX response code to TGT_RSP_OK.
//-----------------------------------------------------------------------------
void Set_TX_TGT_RSP_OK (void)
{
   Tx_Buf[0] = TGT_RSP_OK;
}

//-----------------------------------------------------------------------------
// Set_TX_TGT_RSP_PARAMETER_INVALID
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
// Sets TX response code to TGT_RSP_PARAMETER_INVALID.
//-----------------------------------------------------------------------------
void Set_TX_TGT_RSP_PARAMETER_INVALID (void)
{
   Tx_Buf[0] = TGT_RSP_PARAMETER_INVALID;
}

//-----------------------------------------------------------------------------
// Set_TX_TGT_RSP_PARAMETER_INVALID
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
// Sets TX response code to TGT_RSP_PARAMETER_INVALID.
//-----------------------------------------------------------------------------
void Set_TX_TGT_RSP_UNSUPPORTED_CMD (void)
{
   Tx_Buf[0] = TGT_RSP_UNSUPPORTED_CMD;
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
