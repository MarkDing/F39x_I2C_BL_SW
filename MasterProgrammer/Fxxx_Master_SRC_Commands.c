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
SEGMENT_VARIABLE (SRC_Info[TGT_ENUM_END_VALUE], U8, SEG_XDATA);
SEGMENT_VARIABLE (SRC_Serial[NUM_SERIAL_BYTES], U8, SEG_XDATA);

SEGMENT_VARIABLE (SRC_Rx_Buf[SRC_COMM_RX_BUF_LENGTH], U8, SEG_XDATA);
SEGMENT_VARIABLE (SRC_Tx_Buf[SRC_COMM_TX_BUF_LENGTH], U8, SEG_XDATA);

U8 SRC_App_InfoBlock_Len;
U16 SRC_Page_Size;
U8 SRC_CRC_Is32bit = 0; // 0=16-bit; 1=32-bit

//-----------------------------------------------------------------------------
// Function Prototypes (Local)
//-----------------------------------------------------------------------------


//=============================================================================
// Function Definitions
//=============================================================================

//-----------------------------------------------------------------------------
// SRC_Get_Info
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
//
//
//-----------------------------------------------------------------------------
U8 SRC_Get_Info (U8 *src_info)
{
   // Command Format:
   // [0] Command

   // Response:
   // [0] Response code (ACK/ERROR etc)
   // [1] App InfoBlock byte0 (length)
   // *** SRC waits for RSP_OK from Master before sending rest of the bytes ***
   // [2] App InfoBlock byte1
   // [...] ...
   // [M] App InfoBlock byte(M-1)
   // [M+1] App FW Start Addr0 (LSB)
   // [M+2] App FW Start Addr1
   // [M+3] App FW Start Addr2
   // [M+4] App FW End Addr0 (LSB)
   // [M+5] App FW End Addr1
   // [M+6] App FW End Addr2

   U8 index = 0;
   U8 retval = 0;
   
   SRC_Tx_Buf[0] = SRC_CMD_GET_INFO;
   SRC_Comm_Send_Tx_Bytes (1);   

   retval = SRC_Comm_Wait_For_Rx_Bytes (2);

   if (retval == TGT_RSP_OK)
   {
      SRC_App_InfoBlock_Len = SRC_Rx_Buf[1];

      SRC_Tx_Buf[0] = SRC_RSP_OK;
      SRC_Comm_Send_Tx_Bytes (1);   
    
      SRC_Comm_Wait_For_Rx_Bytes (SRC_App_InfoBlock_Len-1+6); // Get remaining bytes

      src_info[0] = SRC_App_InfoBlock_Len;

      for (index = 0; index < (SRC_App_InfoBlock_Len-1+6); index++)
      {
         src_info[index+1] = SRC_Rx_Buf[index];
      }

      SRC_Page_Size = (1 << src_info[SRC_APP_FLASH_PAGE_SIZE_CODE]); // if code=n, size=2^n

      if ((src_info[SRC_APP_CRC_TYPE] & 0x80) == 0)
      {
         SRC_CRC_Is32bit = 0;
      }
      else
      {
         SRC_CRC_Is32bit = 1;
      }

      for (index = 0; index < NUM_SERIAL_BYTES; index++)
      {
         SRC_Serial[index] = src_info[SRC_APP_DEVICE_SERIAL0 + index];
      }
   }

   return (retval);
}

//-----------------------------------------------------------------------------
// SRC_Get_NextPage_Info
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
//
//
//-----------------------------------------------------------------------------
U8 SRC_Get_NextPage_Info (U32 *addr, U32 *src_page_crc)
{
   // Command Format:
   // [0] Command

   // Response:
   // [0] Response code (ACK/ERROR etc)
   // [1] Page Addr0 (LSB) -or- Num app FW pages specified
   // [2] Page Addr1
   // [3] Page Addr2
   // [4] Page CRC0 (LSB)
   // [5] Page CRC1
   // [6] [Page CRC2]
   // [7] [Page CRC3]

   U8 retval = 0;
   UU32 tempval;
   
   SRC_Tx_Buf[0] = SRC_CMD_GET_NEXTPAGE_INFO;
   SRC_Comm_Send_Tx_Bytes (1);   

   if (SRC_CRC_Is32bit == 0)
   {
      // 16-bit CRC
      retval = SRC_Comm_Wait_For_Rx_Bytes (6);
   }
   else
   {
      // 32-bit CRC
      retval = SRC_Comm_Wait_For_Rx_Bytes (8);
   }

   tempval.U32 = 0;

   // SRC_Rx_Buf[1] will contain 'Page addr0' if retval==RSP_OK
   // SRC_Rx_Buf[1] will contain 'Num app FW pages specified' if retval==SRC_RSP_DATA_END

   tempval.U8[b0] = SRC_Rx_Buf[1];
   tempval.U8[b1] = SRC_Rx_Buf[2];
   tempval.U8[b2] = SRC_Rx_Buf[3];

   *addr = tempval.U32;

   tempval.U32 = 0;

   tempval.U8[b0] = SRC_Rx_Buf[4];
   tempval.U8[b1] = SRC_Rx_Buf[5];

   if (SRC_CRC_Is32bit != 0)
   {
      tempval.U8[b2] = SRC_Rx_Buf[6];
      tempval.U8[b3] = SRC_Rx_Buf[7];
   }

   *src_page_crc = tempval.U32;

   return (retval);
}

//-----------------------------------------------------------------------------
// SRC_Get_NextPage
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
//
//
//-----------------------------------------------------------------------------
U8 SRC_Get_NextPage (void)
{
   // Command Format:
   // [0] Command

   // Response:
   // [0] Response code (ACK/ERROR etc)
   // [1] Page byte0
   // [...] ...
   // [T] Page byte(T-1)
   // [T+1] Response code (ACK/ERROR etc)

   U8 retval = 0;
   
   SRC_Tx_Buf[0] = SRC_CMD_GET_NEXTPAGE;
   SRC_Comm_Send_Tx_Bytes (1);   

   retval = SRC_Comm_Wait_For_Rx_Bytes (SRC_Page_Size+2);
   // Page data is stored in SRC_Rx_Buf by the SRC_Comm_Wait_For_Rx_Bytes function

   return (retval);
}

//-----------------------------------------------------------------------------
// SRC_Disp_TGT_Info
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
//
//
//-----------------------------------------------------------------------------
U8 SRC_Disp_TGT_Info (U8 buf_length, U8 *buf)
{
   // Command Format:
   // [0] Command
   // [1] BL InfoBlock byte0 (length)
   // [2] BL InfoBlock byte1
   // [.] ...
   // [N] BL InfoBlock byte(N-1)
   // [N+1] App InfoBlock byte0 (length)
   // [N+2] App InfoBlock byte1
   // [...] ...
   // [N+M] App InfoBlock byte(M-1)

   // Response:
   // [0] Response code (ACK/ERROR etc)

   U8 index = 0;
   U8 retval = 0;

   SRC_Tx_Buf[0] = SRC_CMD_DISP_TGT_INFO;

   for (index = 0; index < buf_length; index++)
   {
      SRC_Tx_Buf[index+1] = *(buf+index);
   }

   SRC_Comm_Send_Tx_Bytes (buf_length+1);   

   retval = SRC_Comm_Wait_For_Rx_Bytes (1);

   return (retval);
}

//-----------------------------------------------------------------------------
// SRC_Disp_Error
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
//
//
//-----------------------------------------------------------------------------
U8 SRC_Disp_Error (U8 error_code)
{
   // Command Format:
   // [0] Command

   U8 index = 0;
   U8 retval = 0;

   SRC_Tx_Buf[0] = SRC_CMD_DISP_ERROR | (error_code & 0x7F);

   SRC_Comm_Send_Tx_Bytes (1);   

   retval = SRC_Comm_Wait_For_Rx_Bytes (1);

   return (retval);
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
