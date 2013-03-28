//-----------------------------------------------------------------------------
// F33x_Comm_SMB0.c
//-----------------------------------------------------------------------------
// Copyright (C) 2010 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// [*** TEMPLATE INFORMATION ***]
// Filename: Fnnn_Comm_CommType.c
// nnn => Fill-in with the MCU family part number.
// CommType => Fill-in Comm interface name.
// Examples: F50x_Comm_CAN0.c, F34x_Comm_UART0.c
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

#include "Fxxx_BL131_SMB0_Interface.h"  // TODO: Change to actual file name - DONE

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------
#define RSP_OK 0x00

//-----------------------
// Bit Definition Masks
//-----------------------

// CAN0STAT
#define BOff  0x80                     // Busoff Status
#define EWarn 0x40                     // Warning Status
#define EPass 0x20                     // Error Passive
#define RxOk  0x10                     // Receive Message Successfully
#define TxOk  0x08                     // Transmitted Message Successfully
#define LEC   0x07                     // Last Error Code

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
bit   SMB_DATA_READY;
U8    SMB_NUM_BYTES;

//-----------------------------------------------------------------------------
// Function Prototypes (Local)
//-----------------------------------------------------------------------------

//=============================================================================
// Function Definitions
//=============================================================================

//-----------------------------------------------------------------------------
// SMB0_Op
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    1) SMB0_Num_Bytes - Length of pending SMB transaction
//
// Waits for an SMB transaction of length SMB0_Num_Bytes to complete before
// returning.
//-----------------------------------------------------------------------------
void SMB0_Op (U8 SMB0_Num_Bytes)
{
//   SMB_NUM_BYTES = SMB0_Num_Bytes;
   SMB_DATA_READY = 0;

   // Loop until all data sent.
   do
   {
      SMBus_Handler ();
   } while (!SMB_DATA_READY);
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SMBus Handler
//-----------------------------------------------------------------------------
//
// SMBus state machine
// - Slave only implementation - no master states defined
// - All incoming data is written to global variable <SMB_data_IN>
// - All outgoing data is read from global variable <SMB_data_OUT>
//
void SMBus_Handler (void)
{
   static U8 data * Rx_Buf_ptr;
   static U8 data * Tx_Buf_ptr;

   if (SI)
   {
      if (ARBLOST == 0)
      {
         switch (SMB0CN & 0xF0)           // Decode the SMBus status vector
         {
            // Slave Receiver: Start+Address received
            case  SMB_SRADD:

               STA = 0;                   // Clear STA bit

               Rx_Buf_ptr = &Rx_Buf[0];
               Tx_Buf_ptr = &Tx_Buf[0];
			   SMB_NUM_BYTES = 0;

               if((SMB0DAT&0xFE) == (SMB0_SLAVE_ADDRESS&0xFE)) // Decode address
               {                          // If the received address matches,
                  ACK = 1;                // ACK the received slave address

                  if((SMB0DAT&0x01) == READ) // If the transfer is a master READ,
                  {
                     // Prepare outgoing byte
                     SMB0DAT = *(Tx_Buf_ptr);
                     Tx_Buf_ptr++;
                  }
               }
               else                       // If received slave address does not
               {                          // match,
                  ACK = 0;                // NACK received address
               }
               break;

            // Slave Receiver: Data received
            case  SMB_SRDB:

	           if(SMB_NUM_BYTES < TGT_BL_BUF_SIZE)
			   {
                   *(Rx_Buf_ptr) = SMB0DAT;
  		           SMB_NUM_BYTES++;
	               Rx_Buf_ptr++;
			   }
               ACK = 1;                // ACK received data
               break;

            // Slave Transmitter: Data byte transmitted
            case  SMB_STDB:

               if (ACK == 1)              // If Master ACK's, send the next byte
               {
                  // Prepare next outgoing byte
                  SMB0DAT = *(Tx_Buf_ptr);
                  Tx_Buf_ptr++;
               }
               break;

            // Slave Receiver: Stop received while either a Slave Receiver or
            // Slave Transmitter
            case  SMB_SRSTO:

            // Slave Transmitter: Arbitration lost, Stop detected
            //
            // This state will only be entered on a bus error condition.
            // In normal operation, the slave is no longer sending data or has
            // data pending when a STOP is received from the master, so the TXMODE
            // bit is cleared and the slave goes to the SRSTO state.
            case  SMB_STSTO:

               SMB_DATA_READY = 1;     // Indicate new data fully received
               STO = 0;                   // STO must be cleared by software when
                                          // a STOP is detected as a slave
               break;

            // Default: all other cases undefined
            default:
               SMB_DATA_READY = 1;     // Indicate new data fully received
               SMB0CN &= ~0x32;    // STA, STO, and ACK set to 0
               break;
         }
      }
      // ARBLOST = 1, Abort failed transfer
      else
      {
         SMB0CN &= ~0x32;    // STA, STO, and ACK set to 0
      }

      SI = 0;                             // Clear SMBus interrupt flag
   }
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
