//-----------------------------------------------------------------------------
// F39x_MSB.c - SMB_BL - Master_BL_FW
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

#include "F39x_SMB.h"

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
bit CAN_Error = 0;                     // 0 = No Errors during transmission
                                       // 1 = Some error(s) occurred

SEGMENT_VARIABLE (CAN_Rx_Buf[8], U8, SEG_XDATA);
SEGMENT_VARIABLE (CAN_Tx_Buf[8], U8, SEG_XDATA);

bit CAN_Rx_Complete_Flag = 0;          // CAN Rx Complete Flag

#if 0 // Mark.Ding hide it
//-----------------------------------------------------------------------------
// Function Prototypes (Local)
//-----------------------------------------------------------------------------
INTERRUPT_PROTO (CAN0_ISR, INTERRUPT_CAN0);
void CAN0_Send_Message (U8 *buf, U8 msg_obj);

//-----------------------------------------------------------------------------
// Main Routine
//-----------------------------------------------------------------------------

//=============================================================================
// Function Definitions
//=============================================================================

//-----------------------------------------------------------------------------
// CAN0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function initializes the CAN peripheral and three message objects
//
// CAN Bit Clock : 1 Mbps
// Auto Retransmit : Automatic Retransmission is enabled
// MsgVal        : Set to Valid based on the #define MESSAGE_OBJECTS
// Filtering     : Enabled for all valid message objects
// Message Identifier : 11-bit standard; Each message object is only used by
//                      one message ID
// Direction     : Two buffers for transmit and one for receive are configured
// End of Buffer : All message objects are treated as separate buffers
//
// The following interrupts are enabled and are handled by CAN0_ISR
//
// Error Interrupts
// Status Change Interrupt
// Receive Interrupt
//
//-----------------------------------------------------------------------------

void CAN0_Init (void)
{
   U8 SFRPAGE_save = SFRPAGE;
   SFRPAGE  = CAN0_PAGE;               // All CAN register are on page 0x0C

   CAN0CN |= 0x01;                     // Start Intialization mode

   //---------Initialize general CAN peripheral settings

   CAN0CN |= 0x4E;                     // Enable Error and Module
                                       // Enable access to bit timing register

   // See the CAN Bit Timing Spreadsheet for how to calculate this value
   CAN0BT = 0x1402;                    // Based on 24 Mhz CAN clock, set the
                                       // CAN bit rate to 1 Mbps

   //---------Initialize settings for Transmit Message Object 1

   // Command Mask Register
   CAN0IF1CM = 0x00F0;                 // Write Operation
                                       // Transfer ID Mask, MDir, MXtd
                                       // Transfer ID, Dir, Xtd, MsgVal
                                       // Transfer Control Bits
                                       // Don't set TxRqst or transfer data

   // Mask Registers
   CAN0IF1M1 = 0x0000;                 // Mask Bits 15-0 not used for filtering
   CAN0IF1M2 = 0x5FFC;                 // Ignore Extended Identifier for
                                       // filtering
                                       // Used Direction bit for filtering
                                       // Use ID bits 28-18 for filtering

   // Message Control Registers
   CAN0IF1MC = 0x0080 | MESSAGE_SIZE;  // Disable Transmit Interrupt
                                       // Message Object is a Single Message
                                       // Message Size set by #define
   // Arbitration Registers
   CAN0IF1A1 = 0x0000;                 // 11-bit ID, so lower 16-bits not used

   // Arbitration Registers
   CAN0IF1A2 = 0xA000 | (MSG_ID_TX_BL_CMD << 2);   // Set MsgVal to valid
                                                   // Set Direction to write
                                                   // Set 11-bit Identifier

   CAN0IF1CR = MO_TX_BL_CMD;           // Start command request

   while (CAN0IF1CRH & 0x80) {}        // Poll on Busy bit

   //---------Initialize settings for Transmit Message Object 2

   // Can use the same settings for this transmit object, so no need reinitalize
   // the first five CAN registers again

   // Command Mask Register
   // CAN0IF1CM = 0x00F0;

   // Mask Registers
   // CAN0IF1M1 = 0x0000;
   // CAN0IF1M2 = 0x5FFC;

   // Message Control Registers
   // CAN0IF1MC = 0x0080 | MESSAGE_SIZE;

   // Arbitration Registers
   // CAN0IF1A1 = 0x0000;

   // Arbitration Registers
   CAN0IF1A2 = 0xA000 | (MSG_ID_TX_BL_WRITE8 << 2);   // Set MsgVal to valid
                                                      // Set Direction to write
                                                      // Set 11-bit Identifier

   CAN0IF1CR = MO_TX_BL_WRITE8;        // Start command request

   while (CAN0IF1CRH & 0x80) {}        // Poll on Busy bit


   //---------Initialize settings for Receive Message Object

   // Can use the same settings for Receive object, so no need reinitalize the
   // first four CAN registers again

   // Command Mask Register
   //   CAN0IF1CM = 0x00F0;

   // Mask Registers
   //  CAN0IF1M1 = 0x0000;
   //  CAN0IF1M2 = 0x5FFC;

   // Arbitration Registers
   //  CAN0IF1A1 = 0x0000;


   // Message Control Registers
   CAN0IF1MC = 0x1480 | MESSAGE_SIZE;  // Enable Receive Interrupt
                                       // Message Object is a Single Message
                                       // Message Size set by #define
   // Arbitration Registers
   CAN0IF1A2 = 0x8000 | (MSG_ID_RX_BL_RSP << 2);   // Set MsgVal to valid
                                                   // Set Object Direction to read
                                                   // Set 11-bit Identifier

   CAN0IF1CR = MO_RX_BL_RSP;              // Start command request

   while (CAN0IF1CRH & 0x80) {}           // Poll on Busy bit

   //--------- CAN Initalization is complete

   CAN0CN &= ~0x41;                    // Return to Normal Mode and disable
                                       // access to bit timing register

   EIE2 |= 0x02;                       // Enable CAN interupts

   SFRPAGE = SFRPAGE_save;
}

//-----------------------------------------------------------------------------
// CAN0_SendMessage
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Send the data in the buffer using the message object. This function
// assumes 8 bytes of data for all messages.
//
//-----------------------------------------------------------------------------

void CAN0_Send_Message (U8 *buf, U8 msg_obj)
{
   // This function assumes that the message object is fully initialized
   // in CAN0_Init and so all it has to do is fill the data registers and
   // initiate transmission

   U8 SFRPAGE_save = SFRPAGE;
   SFRPAGE  = CAN0_PAGE;               // All CAN register are on page 0x0C

   CAN0IF1DA1H = *buf;                 // Initialize data registers
   CAN0IF1DA1L = *(buf + 1);
   CAN0IF1DA2H = *(buf + 2);
   CAN0IF1DA2L = *(buf + 3);
   CAN0IF1DB1H = *(buf + 4);
   CAN0IF1DB1L = *(buf + 5);
   CAN0IF1DB2H = *(buf + 6);
   CAN0IF1DB2L = *(buf + 7);

   CAN0IF1CM = 0x0087;                 // Set Direction to Write
                                       // Write TxRqst, all 8 data bytes

   CAN0IF1CR = msg_obj;                // Start command request

   while (CAN0IF1CRH & 0x80) {}        // Poll on Busy bit

   SFRPAGE = SFRPAGE_save;             // Restore SFRPAGE
}

//-----------------------------------------------------------------------------
// TGT_Enter_BL_Mode
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
//
//
//-----------------------------------------------------------------------------
U8 TGT_Enter_BL_Mode (U8 request_response, U8 CAN_dev_addr)
{
   U8 SFRPAGE_save = SFRPAGE;
   SFRPAGE = CAN0_PAGE;

   // Command Format:
   // [0] Command
   // [1] CAN Device Address

   CAN_Tx_Buf[0] = TGT_CMD_ENTER_BL_MODE;
   CAN_Tx_Buf[1] = CAN_dev_addr;
   
   //CAN_Tx_Buf[1] = 0x01;   // CAN Device Addr = 0x01
   // Other bytes of the message are "don't care".

   CAN_Rx_Complete_Flag = 0;
   CAN0_Send_Message (CAN_Tx_Buf, MO_TX_BL_CMD);

   if (request_response == REQUEST_RESPONSE)
   {
      while (CAN_Rx_Complete_Flag == 0);  // Wait till a response is received

      // Response Format:
      // [0] Return code (ACK/ERROR etc)
   }

   SFRPAGE = SFRPAGE_save;

   return CAN_Rx_Buf[0];
}

//-----------------------------------------------------------------------------
// TGT_Get_Info
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
//
//
//-----------------------------------------------------------------------------
U8 TGT_Get_Info (U8 *target_info)
{
   U8 packet_num;
   U8 index;

   U8 SFRPAGE_save = SFRPAGE;
   SFRPAGE = CAN0_PAGE;

   // Command Format:
   // [0] Command

   CAN_Tx_Buf[0] = TGT_CMD_GET_INFO;
   // Other bytes of the message are "don't care".

   for (packet_num = 0; packet_num < 3; packet_num++)
   {
      CAN_Rx_Complete_Flag = 0;
      CAN0_Send_Message (CAN_Tx_Buf, MO_TX_BL_CMD);

      while (CAN_Rx_Complete_Flag == 0);  // Wait till a response is received

      // Response:
      // [0] Return code (ACK/ERROR etc)
      // [1] Packet Number
      // [2] InfoBlock byte (0 + (packetnum*6))
      // [3] InfoBlock byte (1 + (packetnum*6))
      // [4] ...
      // [5] 
      // [6] 
      // [7]

      if (CAN_Rx_Buf[0] != TGT_RSP_OK)
         break;

      if (CAN_Rx_Buf[1] != packet_num)
      {
         CAN_Rx_Buf[0] = TGT_RSP_ERROR;
         break;
      }

      for (index = 0; index < (MESSAGE_SIZE-2); index++)
      {
         target_info[index + (packet_num*6)] = CAN_Rx_Buf[index + 2];
      }
   }

   SFRPAGE = SFRPAGE_save;

   return CAN_Rx_Buf[0];
}

//-----------------------------------------------------------------------------
// TGT_Set_Flash_Keys
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
//
//
//-----------------------------------------------------------------------------
U8 TGT_Set_Flash_Keys (U8 flash_key0, U8 flash_key1)
{
   U8 SFRPAGE_save = SFRPAGE;
   SFRPAGE = CAN0_PAGE;

   // Command Format:
   // [0] Command
   // [1] Flash Key byte 0
   // [2] Flash Key byte 1

   CAN_Tx_Buf[0] = TGT_CMD_SET_FLASH_KEYS;
   CAN_Tx_Buf[1] = flash_key0;
   CAN_Tx_Buf[2] = flash_key1;
   // Other bytes of the message are "don't care".

   CAN_Rx_Complete_Flag = 0;
   CAN0_Send_Message (CAN_Tx_Buf, MO_TX_BL_CMD);

   while (CAN_Rx_Complete_Flag == 0);  // Wait till a response is received

   // Response:
   // [0] Return code (ACK/ERROR etc)

   SFRPAGE = SFRPAGE_save;

   return CAN_Rx_Buf[0];
}

//-----------------------------------------------------------------------------
// TGT_Set_Addr
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
//
//
//-----------------------------------------------------------------------------
U8 TGT_Set_Addr (U8 bank, U32 addr)
{
   UU32 addr_copy;

   U8 SFRPAGE_save = SFRPAGE;
   SFRPAGE = CAN0_PAGE;

   addr_copy.U32 = addr;

   // Command Format:
   // [0] Command
   // [1] Flash code bank number
   // [2] Flash address byte 0
   // [3] Flash address byte 1
   // [4] Flash address byte 2 <-- don't care for MCUs 64k and smaller

   CAN_Tx_Buf[0] = TGT_CMD_SET_ADDR;
   CAN_Tx_Buf[1] = bank;
   CAN_Tx_Buf[2] = addr_copy.U8[b0];
   CAN_Tx_Buf[3] = addr_copy.U8[b1];
   CAN_Tx_Buf[4] = addr_copy.U8[b2];
   // Other bytes of the message are "don't care".

   CAN_Rx_Complete_Flag = 0;
   CAN0_Send_Message (CAN_Tx_Buf, MO_TX_BL_CMD);

   while (CAN_Rx_Complete_Flag == 0);  // Wait till a response is received

   // Response:
   // [0] Return code (ACK/ERROR etc)

   SFRPAGE = SFRPAGE_save;

   return CAN_Rx_Buf[0];
}

//-----------------------------------------------------------------------------
// TGT_Erase_Page
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
//
//
//-----------------------------------------------------------------------------
U8 TGT_Erase_Page (void)
{
   U8 SFRPAGE_save = SFRPAGE;
   SFRPAGE = CAN0_PAGE;

   // Command Format:
   // [0] Command

   CAN_Tx_Buf[0] = TGT_CMD_ERASE_PAGE;
   // Other bytes of the message are "don't care".

   CAN_Rx_Complete_Flag = 0;
   CAN0_Send_Message (CAN_Tx_Buf, MO_TX_BL_CMD);

   while (CAN_Rx_Complete_Flag == 0);  // Wait till a response is received

   // Response:
   // [0] Return code (ACK/ERROR etc)

   SFRPAGE = SFRPAGE_save;

   return CAN_Rx_Buf[0];
}

//-----------------------------------------------------------------------------
// TGT_Write_Flash
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
//
//
//-----------------------------------------------------------------------------
U8 TGT_Write_Flash (U8 *buf, U16 index, U8 numbytes)
{
   U8 i;

   U8 SFRPAGE_save = SFRPAGE;
   SFRPAGE = CAN0_PAGE;

   // numbytes needs to be 8 because this message is preconfigured for 8 bytes

   // Command Format: [No command byte because this uses a unique message ID]
   // [0] Flash byte 0
   // [1] Flash byte 1
   // [2] Flash byte 2
   // [3] Flash byte 3
   // [4] Flash byte 4
   // [5] Flash byte 5
   // [6] Flash byte 6
   // [7] Flash byte 7

   for (i = 0; i < numbytes; i++)
   {
      CAN_Tx_Buf[i] = *(buf + index + i);
   }
   // Other bytes of the message are "don't care".

   CAN_Rx_Complete_Flag = 0;
   CAN0_Send_Message (CAN_Tx_Buf, MO_TX_BL_WRITE8);

   while (CAN_Rx_Complete_Flag == 0);  // Wait till a response is received

   // Response:
   // [0] Return code (ACK/ERROR etc)

   SFRPAGE = SFRPAGE_save;

   return CAN_Rx_Buf[0];
}

//-----------------------------------------------------------------------------
// TGT_Get_Page_CRC
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
//
//
//-----------------------------------------------------------------------------
U8 TGT_Get_Page_CRC (U16 *target_page_crc)
{
   U8 SFRPAGE_save = SFRPAGE;
   SFRPAGE = CAN0_PAGE;

   // Command Format:
   // [0] Command

   CAN_Tx_Buf[0] = TGT_CMD_GET_PAGE_CRC;
   // Other bytes of the message are "don't care".

   CAN_Rx_Complete_Flag = 0;
   CAN0_Send_Message (CAN_Tx_Buf, MO_TX_BL_CMD);

   while (CAN_Rx_Complete_Flag == 0);  // Wait till a response is received

   // Response:
   // [0] Return code (ACK/ERROR etc)
   // [1] CRC byte 0
   // [2] CRC byte 1

   *target_page_crc = (U16)((CAN_Rx_Buf[2] << 8) | CAN_Rx_Buf[1]);

   SFRPAGE = SFRPAGE_save;

   return CAN_Rx_Buf[0];
}

//-----------------------------------------------------------------------------
// TGT_Write_Signature
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
//
//
//-----------------------------------------------------------------------------
U8 TGT_Write_Signature (U8 sig_byte0, U8 sig_byte1, U8 sig_byte2, U8 sig_byte3)
{
   U8 SFRPAGE_save = SFRPAGE;
   SFRPAGE = CAN0_PAGE;

   // Command Format:
   // [0] Command
   // [1] Signature byte 0
   // [2] Signature byte 1
   // [3] Signature byte 2
   // [4] Signature byte 3

   CAN_Tx_Buf[0] = TGT_CMD_WRITE_SIGNATURE;
   CAN_Tx_Buf[1] = sig_byte0;
   CAN_Tx_Buf[2] = sig_byte1;
   CAN_Tx_Buf[3] = sig_byte2;
   CAN_Tx_Buf[4] = sig_byte3;
   // Other bytes of the message are "don't care".

   CAN_Rx_Complete_Flag = 0;
   CAN0_Send_Message (CAN_Tx_Buf, MO_TX_BL_CMD);

   while (CAN_Rx_Complete_Flag == 0);  // Wait till a response is received

   // Response:
   // [0] Return code (ACK/ERROR etc)


   SFRPAGE = SFRPAGE_save;

   return CAN_Rx_Buf[0];
}

//-----------------------------------------------------------------------------
// TGT_SW_Reset
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
//
//
//-----------------------------------------------------------------------------
void TGT_SW_Reset (void)
{
   U8 SFRPAGE_save = SFRPAGE;
   SFRPAGE = CONFIG_PAGE;

   // Command Format:
   // [0] Command

   CAN_Tx_Buf[0] = TGT_CMD_SW_RESET;
   // Other bytes of the message are "don't care".

   CAN_Rx_Complete_Flag = 0;
   CAN0_Send_Message (CAN_Tx_Buf, MO_TX_BL_CMD);


   // No response should be expected for this command

   /*
   while (CAN_Rx_Complete_Flag == 0);  // Wait till a response is received
   */

   // Response:
   // [0] Return code (ACK/ERROR etc)


   SFRPAGE = SFRPAGE_save;
}

//=============================================================================
// Interrupt Service Routines
//=============================================================================

//-----------------------------------------------------------------------------
// CAN0_ISR
//-----------------------------------------------------------------------------
//
// The ISR is triggered upon any CAN errors or upon a complete reception.
//
// If an error occurs, a global flag is updated
//
//-----------------------------------------------------------------------------

INTERRUPT (CAN0_ISR, INTERRUPT_CAN0)
{
   // SFRPAGE is set to CAN0_Page automatically when ISR starts

   U8 status = CAN0STAT;               // Read status, which clears the Status
                                       // Interrupt bit pending in CAN0IID

   U8 Interrupt_ID = CAN0IID;          // Read which message object caused
                                       // the interrupt

   CAN0IF1CM = 0x007F;                 // Read all of message object to IF1
                                       // Clear IntPnd and newData


   CAN0IF1CR = Interrupt_ID;           // Start command request to actually
                                       // clear the interrupt

   while (CAN0IF1CRH & 0x80) {}        // Poll on Busy bit

   // If receive completed successfully
   if ((status & RxOk) && (Interrupt_ID == MO_RX_BL_RSP))
   {
       // Read all 8 data bytes to CAN_Rx_Buf, even though they might not be valid

      CAN_Rx_Buf[0] = CAN0IF1DA1H;
      CAN_Rx_Buf[1] = CAN0IF1DA1L;
      CAN_Rx_Buf[2] = CAN0IF1DA2H;
      CAN_Rx_Buf[3] = CAN0IF1DA2L;
      CAN_Rx_Buf[4] = CAN0IF1DB1H;
      CAN_Rx_Buf[5] = CAN0IF1DB1L;
      CAN_Rx_Buf[6] = CAN0IF1DB2H;
      CAN_Rx_Buf[7] = CAN0IF1DB2L;

      CAN_Rx_Complete_Flag = 1;       // Indicate Rx Complete
   }

   // If an error occured, simply update the global variable and continue
   if (status & LEC)
   {
       // The LEC bits identify the type of error, but those are grouped here
      if ((status & LEC) != 0x07)
      {
          CAN_Error = 1;
      }
   }

   if (status & BOff)
   {
      CAN_Error = 1;
   }

   if (status & EWarn)
   {
      CAN_Error = 1;
   }

   // Old SFRPAGE is popped off the SFR stack when ISR exits
}
#endif
//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
