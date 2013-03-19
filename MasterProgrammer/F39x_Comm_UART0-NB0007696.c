//-----------------------------------------------------------------------------
// F50x_Comm_UART0.c
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
// Note: The following changes were made to this file so it can be used 
// in the Master project:
// 1. The include header files were changed from 'TargetBL' to 'Master'.
// 2. "Rx_Buf" replaced with "SRC_Rx_Buf".
// 3. "Tx_Buf" replaced with "SRC_Tx_Buf".
// 4. All function names prefixed with "SRC_".
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
#include "Fxxx_BL128_UART0_Interface.h"  

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Function Prototypes (Local)
//-----------------------------------------------------------------------------
void SRC_Comm_Init (void);
U8 SRC_Comm_Wait_For_Rx_Bytes (U16 numbytes_to_read);
void SRC_Comm_Send_Tx_Bytes (U16 numbytes_to_send);

//=============================================================================
// Function Definitions
//=============================================================================

//-----------------------------------------------------------------------------
// SRC_Comm_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function initializes the UART0 peripheral.
// Interrupts are disabled and handled in polled mode instead.
//
// Configure the UART0 using Timer1, for <BAUDRATE> and 8-N-1.
//
//-----------------------------------------------------------------------------
void SRC_Comm_Init (void)
{
   // *** Comm-Specific / Device-Specific ***
   SCON0 = 0x10;                       // SCON0: 8-bit variable bit rate
                                       //        level of STOP bit is ignored
                                       //        RX enabled
                                       //        ninth bits are zeros
                                       //        clear RI0 and TI0 bits
   if (SYSCLK/BAUDRATE/2/256 < 1) {
      TH1 = -(SYSCLK/BAUDRATE/2);
      CKCON &= ~0x0B;                  // T1M = 1; SCA1:0 = xx
      CKCON |=  0x08;
   } else if (SYSCLK/BAUDRATE/2/256 < 4) {
      TH1 = -(SYSCLK/BAUDRATE/2/4);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 01
      CKCON |=  0x01;
   } else if (SYSCLK/BAUDRATE/2/256 < 12) {
      TH1 = -(SYSCLK/BAUDRATE/2/12);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 00
   } else {
      TH1 = -(SYSCLK/BAUDRATE/2/48);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 10
      CKCON |=  0x02;
   }

   TL1 = TH1;                          // init Timer1
   TMOD &= ~0xf0;                      // TMOD: timer 1 in 8-bit autoreload
   TMOD |=  0x20;
   TR1 = 1;                            // START Timer1
//   IP |= 0x10;                         // Make UART high priority
//   ES0 = 1;                            // Enable UART0 interrupts
// *** Do not enable UART0 interrupts -- Polled-mode ***
}

//-----------------------------------------------------------------------------
// SRC_Comm_Wait_For_Rx_Bytes
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
//
//-----------------------------------------------------------------------------
U8 SRC_Comm_Wait_For_Rx_Bytes (U16 numbytes_to_read)
{
   // *** Comm-Specific / Device-Specific ***
   // TODO: Add code here to read bytes from the comm interface and store them
   // in the "SRC_Rx_Buf" buffer (buffer variable defined elsewhere).
   // Return the 1st byet of SRC_Rx_Buf as the return value.
   // This should be a blocking function; wait until requested number of bytes
   // have been read from the comm interface.
   // Send an ack (RSP_OK) after receiving each packet (except for the last packet)
  
   U16 received_numbytes = 0;
   

   while (received_numbytes < numbytes_to_read)
   {
      while (RI0 == 0) {}              // Poll on receive interrupt flag

      // Read a byte from UART
      *(SRC_Rx_Buf + received_numbytes) = SBUF0; 
      RI0 = 0;                         // Clear interrupt flag
      
      received_numbytes++;
   }
   
   
   // Return the BL command code, which is always at byte0
   return (*(SRC_Rx_Buf + 0)); 
}

//-----------------------------------------------------------------------------
// SRC_Comm_Send_Tx_Bytes
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Send the data in the buffer using the message object. 
//
//-----------------------------------------------------------------------------
void SRC_Comm_Send_Tx_Bytes (U16 numbytes_to_send)
{
   // *** Comm-Specific / Device-Specific ***
   // TODO: Add code here to write bytes to the comm interface from
   // the "SRC_Tx_Buf" buffer (buffer variable defined elsewhere).
   // This should be a blocking function; wait until ALL of the requested number 
   // of bytes have been sent via the comm interface.

   // Wait for an ack (RSP_OK) after sending each packet (except for the last packet)
  
   U16 sent_numbytes = 0;
   
   U8 SFRPAGE_save = SFRPAGE;
   SFRPAGE  = ACTIVE_PAGE;

   while (sent_numbytes < numbytes_to_send)
   {
      TI0 = 0;                         // Clear interrupt flag
      
      // Place a byte in UART0 buffer to start tranmission
      SBUF0 = *(SRC_Tx_Buf + sent_numbytes);            

      sent_numbytes++;      
      
      while (TI0 == 0) {}              // Poll on transmit interrupt flag
   }
   
   SFRPAGE = SFRPAGE_save;             // Restore SFRPAGE
}
//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
