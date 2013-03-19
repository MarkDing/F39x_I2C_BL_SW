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
SEGMENT_VARIABLE (Rx_Buf[TGT_COMM_RX_BUF_LENGTH], U8, SEG_XDATA);
SEGMENT_VARIABLE (Tx_Buf[TGT_COMM_TX_BUF_LENGTH], U8, SEG_XDATA);

SEGMENT_VARIABLE (TGT_Info[TGT_ENUM_END_VALUE], U8, SEG_XDATA);

U8 TGT_BL_InfoBlock_Len;
U8 TGT_App_InfoBlock_Len;

U16 TGT_Page_Size;
U16 TGT_BL_Buffer_Size;
U8 TGT_CRC_Is32bit = 0; // 0=16-bit; 1=32-bit

bit TGT_Write_Flash_Data_Bytes = 0;

U32 TGT_App_FW_End_Addr;

//-----------------------------------------------------------------------------
// Function Prototypes (Local)
//-----------------------------------------------------------------------------


//=============================================================================
// Function Definitions
//=============================================================================

//-----------------------------------------------------------------------------
// TGT_Enter_BL_Mode
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
//
//
//-----------------------------------------------------------------------------
U8 TGT_Enter_BL_Mode (U8 req_rsp_code, U8 *dev_serial)
{
   // Command Format:
   // [0] Command
   // [1] Code to request or prevent a response
   // [2] [dev_serial0]
   // ...
   // [7] [dev_serial5]

   // Response (only if 'request_response' is active):
   // [0] Response code
   // [1] BL InfoBlock Length
   // [2] App InfoBlock Length

   U8 index = 0;
   U8 retval = 0;
   U8 i;

   Tx_Buf[index++] = TGT_CMD_ENTER_BL_MODE;

   Tx_Buf[index++] = req_rsp_code; // Target will send a response only if byte1 is 0x52

   for (i = 0; i < NUM_SERIAL_BYTES; i++)
   {
      Tx_Buf[index + i] = *(dev_serial + i);
   }

   Comm_Send_Tx_Bytes (TGT_CMD_FIXED_LENGTH);

   if (req_rsp_code == TGT_ENTER_BL_MODE_REQ_RSP_CODE)
   {
      retval = Comm_Wait_For_Rx_Bytes (3);
      TGT_BL_InfoBlock_Len = Rx_Buf[1];
      TGT_App_InfoBlock_Len = Rx_Buf[2];
   }

   return (retval);
}

//-----------------------------------------------------------------------------
// TGT_Get_Info
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
//
//
//-----------------------------------------------------------------------------
U8 TGT_Get_Info (U8 *target_info)
{
   // Command Format:
   // [0] Command
   // [1..7] xx

   // Response:
   // [0] Response code (ACK/ERROR etc)
   // [1] BL InfoBlock byte0 (length)
   // [2] BL InfoBlock byte1
   // [.] ...
   // [N] BL InfoBlock byte(N-1)
   // [N+1] App InfoBlock byte0 (length)
   // [N+2] App InfoBlock byte1
   // [...] ...
   // [N+M] App InfoBlock byte(M-1)

   U8 index = 0;
   U8 retval = 0;
   
   Tx_Buf[index++] = TGT_CMD_GET_INFO;

   Comm_Send_Tx_Bytes (TGT_CMD_FIXED_LENGTH);   

   retval = Comm_Wait_For_Rx_Bytes (TGT_BL_InfoBlock_Len +TGT_App_InfoBlock_Len + 1);

   if (retval == TGT_RSP_OK)
   {
      for (index = 0; index < (TGT_BL_InfoBlock_Len + TGT_App_InfoBlock_Len); index++)
      {
         target_info[index] = Rx_Buf[index+1];
      }

      if (TGT_App_InfoBlock_Len == 0)
      {
         target_info[index] = 0; // Set target app infoblock length to 0
      }

      TGT_Page_Size = (1 << target_info[TGT_FLASH_PAGE_SIZE_CODE]); // if code=n, size=2^n
      TGT_BL_Buffer_Size = (1 << (target_info[TGT_BL_BUFFER_SIZE_CODE] & 0x0F)); // if code=n, size=2^n

      if ((target_info[TGT_CRC_TYPE] & 0x80) == 0)
      {
         TGT_CRC_Is32bit = 0;
      }
      else
      {
         TGT_CRC_Is32bit = 1;
      }
   }

   return (retval);
}

//-----------------------------------------------------------------------------
// TGT_Erase_Page
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
//
//
//-----------------------------------------------------------------------------
U8 TGT_Erase_Page (U8 flash_key0, U8 flash_key1, U32 addr)
{
   // Command Format:
   // [0] Command
   // [1] flash key code0
   // [2] flash key code1
   // [3] addr0 (LSB)
   // [4] addr1
   // [5] addr2
   // [6] RESERVED
   // [7] RESERVED

   // Response:
   // [0] Response code

   U8 index = 0;
   U8 retval = 0;
   UU32 addr_copy;

   addr_copy.U32 = addr;
   
   Tx_Buf[index++] = TGT_CMD_ERASE_FLASH_PAGE;
   Tx_Buf[index++] = flash_key0;
   Tx_Buf[index++] = flash_key1;
   Tx_Buf[index++] = addr_copy.U8[b0];
   Tx_Buf[index++] = addr_copy.U8[b1];
   Tx_Buf[index++] = addr_copy.U8[b2];

   Comm_Send_Tx_Bytes (TGT_CMD_FIXED_LENGTH);   

   retval = Comm_Wait_For_Rx_Bytes (1);
  
   return (retval);
}

//-----------------------------------------------------------------------------
// TGT_Write_Flash
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
//
//
//-----------------------------------------------------------------------------
U8 TGT_Write_Flash (U8 *buf, U16 numbytes, U8 flash_key0, U8 flash_key1, U32 addr)
{
   // Command Format:
   // [0] Command
   // [1] flash key code0
   // [2] flash key code1
   // [3] addr0 (LSB)
   // [4] addr1
   // [5] addr2
   // [6] numbytes0 (LSB)
   // [7] numbytes1
   
   // Response:
   // [0] Response code

   // Bytes to write:
   // [0] byte0
   // [1] byte1
   // [.] ...
   // [numbytes-1] byte(numbytes-1)

   U8 index;
   U8 retval = 0;
   UU32 addr_copy;
   UU16 numbytes_this_write;
   U16 buf_index = 0;

   addr_copy.U32 = addr;

   if ((addr + numbytes) == (TGT_App_FW_End_Addr + 1))
   {
      numbytes -= 2; // This prevents the signature bytes from being written using this function
   }
   
   while (numbytes > 0)
   {
      index = 0;
      Tx_Buf[index++] = TGT_CMD_WRITE_FLASH_BYTES;
      Tx_Buf[index++] = flash_key0;
      Tx_Buf[index++] = flash_key1;
      Tx_Buf[index++] = addr_copy.U8[b0];
      Tx_Buf[index++] = addr_copy.U8[b1];
      Tx_Buf[index++] = addr_copy.U8[b2];

      if (numbytes > TGT_BL_Buffer_Size)
      {
         numbytes_this_write.U16 = TGT_BL_Buffer_Size;
      }
      else
      {
         numbytes_this_write.U16 = numbytes;
      }

      Tx_Buf[index++] = numbytes_this_write.U8[LSB];
      Tx_Buf[index++] = numbytes_this_write.U8[MSB];

      Comm_Send_Tx_Bytes (TGT_CMD_FIXED_LENGTH);   

      // Get response for the command
      retval = Comm_Wait_For_Rx_Bytes (1);

      if (retval != TGT_RSP_OK)
      {
         Last_Error = ERR_TGT_UNEXPECTED_RSP;
         break;
      }

      TGT_Write_Flash_Data_Bytes = 1;

      for (index = 0; index < numbytes_this_write.U16; index++)
      {
         Tx_Buf[index] = *(buf + buf_index);
         buf_index++;
      }

      Comm_Send_Tx_Bytes (numbytes_this_write.U16);   

      TGT_Write_Flash_Data_Bytes = 0;

      numbytes -= numbytes_this_write.U16;

      if (numbytes > 0)
      {
         // Increment addr to point to next block of bytes
         addr_copy.U32 += numbytes_this_write.U16; 

         // Get response for the command
         retval = Comm_Wait_For_Rx_Bytes (1);

         if (retval != TGT_RSP_OK)
         {
            Last_Error = ERR_TGT_UNEXPECTED_RSP;
            break;
         }
      }
   }

   if (Last_Error == 0)
   {
      // Get final response if no errors above
      retval = Comm_Wait_For_Rx_Bytes (1);
   }
   
   return (retval);
}

//-----------------------------------------------------------------------------
// TGT_Get_Page_CRC
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
//
//
//-----------------------------------------------------------------------------
U8 TGT_Get_Page_CRC (U32 addr, U32 *target_page_crc)
{
   // Command Format:
   // [0] Command
   // [1] addr0 (LSB)
   // [2] addr1
   // [3] addr2
   // [4] RESERVED
   // [5] xx
   // [6] xx
   // [7] xx

   // Response:
   // [0] Response code
   // [1] crc0 (LSB)
   // [2] crc1
   // [3] [crc2]
   // [4] [crc3]

   U8 index = 0;
   U8 retval = 0;
   UU32 addr_copy;
   UU32 crc;

   addr_copy.U32 = addr;
   
   Tx_Buf[index++] = TGT_CMD_GET_PAGE_CRC;
   Tx_Buf[index++] = addr_copy.U8[b0];
   Tx_Buf[index++] = addr_copy.U8[b1];
   Tx_Buf[index++] = addr_copy.U8[b2];

   Comm_Send_Tx_Bytes (TGT_CMD_FIXED_LENGTH);   

   crc.U32 = 0;

   if (TGT_CRC_Is32bit == 0)
   {
      retval = Comm_Wait_For_Rx_Bytes (3);
      crc.U8[b0] = Rx_Buf[1];
      crc.U8[b1] = Rx_Buf[2];
   }
   else
   {
      retval = Comm_Wait_For_Rx_Bytes (5);
      crc.U8[b0] = Rx_Buf[1];
      crc.U8[b1] = Rx_Buf[2];
      crc.U8[b2] = Rx_Buf[3];
      crc.U8[b3] = Rx_Buf[4];
   }

   *target_page_crc = crc.U32;
  
   return (retval);
}

//-----------------------------------------------------------------------------
// TGT_Write_Signature
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
//
//
//-----------------------------------------------------------------------------
U8 TGT_Write_Signature (U8 flash_key0, U8 flash_key1, U8 sig_byte0, U8 sig_byte1)
{
   // Command Format:
   // [0] Command
   // [1] flash key code0
   // [2] flash key code1
   // [3] signature byte0
   // [4] signature byte1
   // [5] RESERVED
   // [6] RESERVED
   // [7] RESERVED

   // Response:
   // [0] Response code

   U8 index = 0;
   U8 retval = 0;
  
   Tx_Buf[index++] = TGT_CMD_WRITE_SIGNATURE;
   Tx_Buf[index++] = flash_key0;
   Tx_Buf[index++] = flash_key1;
   Tx_Buf[index++] = sig_byte0;
   Tx_Buf[index++] = sig_byte1;

   Comm_Send_Tx_Bytes (TGT_CMD_FIXED_LENGTH);   

   retval = Comm_Wait_For_Rx_Bytes (1);
  
   return (retval);
}

//-----------------------------------------------------------------------------
// TGT_Reset_MCU
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
//
//
//-----------------------------------------------------------------------------
U8 TGT_Reset_MCU (void)
{
   // Command Format:
   // [0] Command
   // [1..7] xx

   // Response:
   // [0] Response code

   U8 retval;

   Tx_Buf[0] = TGT_CMD_RESET_MCU;

   Comm_Send_Tx_Bytes (TGT_CMD_FIXED_LENGTH);   

   retval = Comm_Wait_For_Rx_Bytes (1);
  
   return (retval);
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
