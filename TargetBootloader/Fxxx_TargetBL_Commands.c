//-----------------------------------------------------------------------------
// Fxxx_TargetBL_Commands.c
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
U8 Rx_Buf[TGT_BL_BUF_SIZE];
U8 Tx_Buf[TGT_BL_BUF_SIZE];

U8 Flash_Key0;
U8 Flash_Key1;

//-----------------------------------------------------------------------------
// Function Prototypes (Local)
//-----------------------------------------------------------------------------


//=============================================================================
// Function Definitions
//=============================================================================

//-----------------------------------------------------------------------------
// TGT_Pre_Flash
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
// Gets the FLKEY code out of Rx_Buf and clears Rx_Buf to help prevent flash
// corruption.
//-----------------------------------------------------------------------------
void TGT_Pre_Flash (void)
{
   // Store relevant information
   Flash_Key0 = Rx_Buf[1];
   Flash_Key1 = Rx_Buf[2];
   Rx_Buf[1] = 0;
   Rx_Buf[2] = 0;
}

//-----------------------------------------------------------------------------
// TGT_Erase_Page
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
// Erases one page of flash.
//-----------------------------------------------------------------------------
void TGT_Erase_Page (void)
{
   // Command Format:
   // [0] Command
   // [1] flash key code0
   // [2] flash key code1
   // [3] addr0 (LSB)
   // [4] addr1 (MSB)
   // [5] N/A

   // Response:
   // [0] Response code

   U8 AddrMSB = Rx_Buf[4];

   if ((AddrMSB < (APP_FW_START_ADDR >> 8)) || (AddrMSB > (APP_FW_END_ADDR >> 8)))
   {
      // Address out-of-bounds
      Set_TX_TGT_RSP_PARAMETER_INVALID ();
   }
   else
   {
      // Setup for flash operation
      TGT_Pre_Flash ();

      PSCTL |= 0x03;
      FLASH_Modify ((AddrMSB << 8), 0x00);
	  Set_TX_TGT_RSP_OK ();

   }
  // Send response
  SMB0_Op (1);
}

//-----------------------------------------------------------------------------
// TGT_Write_Flash
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
// Writes 1 - 32 bytes of flash.
//-----------------------------------------------------------------------------
void TGT_Write_Flash (void)
{
   // Command Format:
   // [0] Command
   // [1] flash key code0
   // [2] flash key code1
   // [3] addr0 (LSB)
   // [4] addr1 (MSB)
   // [5] numbytes
   
   // Bytes to write:
   // [0] byte0
   // [1] byte1
   // [.] ...
   // [numbytes-1] byte(numbytes-1)

   // Response:
   // [0] Response code

   UU16 start_addr;
   U8 numbytes;
   U8 data * Rx_Buf_ptr = &Rx_Buf[6];

   start_addr.U8[LSB] = Rx_Buf[3];
   start_addr.U8[MSB] = Rx_Buf[4];
   numbytes = Rx_Buf[5];

   if ((start_addr.U8[MSB] < (APP_FW_START_ADDR >> 8)) ||
      (start_addr.U8[MSB] > (APP_FW_END_ADDR >> 8)) ||
      ((U8)((start_addr.U16 + numbytes - 1) >> 8) > (APP_FW_END_ADDR >> 8)))
   {
      // Address out-of-bounds
      Set_TX_TGT_RSP_PARAMETER_INVALID ();
   }
   else
   {
      // Setup for flash operation
      TGT_Pre_Flash ();

      while (numbytes--)
      {
         PSCTL |= 0x01;
         FLASH_Modify (start_addr.U16, *(Rx_Buf_ptr));
         Rx_Buf_ptr++;
         start_addr.U16++;
      }      
	   Set_TX_TGT_RSP_OK ();
   } 
   // Send response
   SMB0_Op (1);
}

//-----------------------------------------------------------------------------
// TGT_Read_Flash
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
// Reads 1 - 32 bytes of flash.
//-----------------------------------------------------------------------------
void TGT_Read_Flash (void)
{
   // Command Format:
   // [0] Command
   // [1] N/A
   // [2] N/A
   // [3] addr0 (LSB)
   // [4] addr1 (MSB)
   // [5] numbytes
   
   // Response:
   // [0] Response code
   // [1] Flash byte 0
   // [2] Flash byte 1
   // ...
   // [numbytes] Flash byte numbytes - 1

   U8 code * start_addr;
   U8 data * Tx_Buf_ptr = &Tx_Buf[0];
   U8 numbytes = Rx_Buf[5];

   // Setup for flash operation
   TGT_Pre_Flash ();

   start_addr = ((Rx_Buf[4] << 8) | Rx_Buf[3]);

   do
   {
      *(Tx_Buf_ptr) = *start_addr;
      start_addr++;
      Tx_Buf_ptr++;
   } while (--numbytes);


   // Send the response code and flash bytes
   SMB0_Op (Rx_Buf[5]);
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
