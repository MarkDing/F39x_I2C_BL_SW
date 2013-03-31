//-----------------------------------------------------------------------------
// F39x_MSB.c - SMB_BL - Master_BL_FW
//-----------------------------------------------------------------------------
// Copyright (C) 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program performs as the Master for the SMB bootloader.
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
#include "F39x_Init.h"
#include "F39x_Master_Interface.h"
#include "F39x_SMB.h"

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

#define CMD_PKG_SIZE 6
SEGMENT_VARIABLE( SMB_Rx_Buf[MAX_BUF_BYTES + CMD_PKG_SIZE], U8, SEG_XDATA);
SEGMENT_VARIABLE( SMB_Tx_Buf[MAX_BUF_BYTES + CMD_PKG_SIZE], U8, SEG_XDATA);

#define WRITE 			0
U8 SMB_NUM_BYTES;
// Flash Write/Erase Key Codes for Target MCU
#define FLASH_KEY0      0xA5
#define FLASH_KEY1      0xF1

U8 TARGET = 0x20; // Target SMBus slave address

bit SMB_BUSY; // Software flag to indicate when the
// SMB_Read() or SMB_Write() functions
// have claimed the SMBus

bit SMB_RW; // Software flag to indicate the
// direction of the current transfer

U16 NUM_ERRORS; // Counter for the number of errors.

// Device addresses (7 bits, lsb is a don't care)
#define  SLAVE_ADDR     0x20           // Device address for slave target
// Status vector - top 4 bits only
#define  SMB_MTSTA      0xE0           // (MT) start transmitted
#define  SMB_MTDB       0xC0           // (MT) data byte transmitted
#define  SMB_MRDB       0x80           // (MR) data byte received
// End status vector definition

//-----------------------------------------------------------------------------
// Function Prototypes (Local)
//-----------------------------------------------------------------------------
void SMB0_Send_Message(U8 *buf, U8 msg_obj);

void Timer3_Init(void);

//INTERRUPT_PROTO (TIMER3_ISR, INTERRUPT_TIMER3);
INTERRUPT_PROTO (SMBUS0_ISR, INTERRUPT_SMBUS0);

void SMB_Write(U8 nbytes);
void SMB_Read(U8 nbytes);

//-----------------------------------------------------------------------------
// Main Routine
//-----------------------------------------------------------------------------

//=============================================================================
// Function Definitions
//=============================================================================

//-----------------------------------------------------------------------------
// SMBus_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// SMBus configured as follows:
// - SMBus enabled
// - Slave mode inhibited
// - Timer1 used as clock source. The maximum SCL frequency will be
//   approximately 1/3 the Timer1 overflow rate
// - Setup and hold time extensions enabled
// - Bus Free and SCL Low timeout detection enabled
//
//-----------------------------------------------------------------------------
void SMBus_Init(void)
{
    SMB0CF = 0x55; // Use Timer1 overflows as SMBus clock
    // source;
    // Disable slave mode;
    // Enable setup & hold time
    // extensions;
    // Enable SMBus Free timeout detect;

    SMB0CF |= 0x80; // Enable SMBus;
    EIE1 |= 0x01; // Enable the SMBus interrupt
}

//-----------------------------------------------------------------------------
// Timer3_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Timer3 configured for use by the SMBus low timeout detect feature as
// follows:
// - Timer3 in 16-bit auto-reload mode
// - SYSCLK/12 as Timer3 clock source
// - Timer3 reload registers loaded for a 25ms overflow period
// - Timer3 pre-loaded to overflow after 25ms
// - Timer3 enabled
//
//-----------------------------------------------------------------------------
void Timer3_Init(void)
{
    TMR3CN = 0x00; // Timer3 configured for 16-bit auto-
    // reload, low-byte interrupt disabled

    CKCON &= ~0x40; // Timer3 uses SYSCLK/12

    TMR3RL = -(SYSCLK / 12 / 40); // Timer3 configured to overflow after
    TMR3 = TMR3RL; // ~25ms (for SMBus low timeout detect):
    // 1/.025 = 40

    EIE1 |= 0x80; // Timer3 interrupt enable
    TMR3CN |= 0x04; // Start Timer3
}

//-----------------------------------------------------------------------------
// TGT_Enter_BL_Mode
//-----------------------------------------------------------------------------
//
// Return Value : Response
// Parameters   : None
//
//
//
//-----------------------------------------------------------------------------
U8 TGT_Enter_BL_Mode()
{
    // Command Format:
    // [0] Command

    SMB_Tx_Buf[0] = TGT_CMD_ENTER_BL_MODE;
    // Other bytes of the message are "don't care".

    SMB_Write(CMD_PKG_SIZE);
    SMB_Read(1);

    // Response:
    // [0] Return code (ACK/ERROR etc)/

    return SMB_Rx_Buf[0];
}

//-----------------------------------------------------------------------------
// TGT_Erase_Page
//-----------------------------------------------------------------------------
//
// Return Value : Response
// Parameters   : None
//
//
//
//-----------------------------------------------------------------------------
U8 TGT_Erase_Page(U32 addr)
{

    // Command Format:
    // [0] Command

    if ((addr < APP_FW_START_ADDR) || (addr > APP_FW_END_ADDR))
    {
        SMB_Rx_Buf[0] = TGT_RSP_PARAMETER_INVALID;
    }
    else
    {
        SMB_Tx_Buf[0] = TGT_CMD_ERASE_FLASH_PAGE;
        SMB_Tx_Buf[1] = FLASH_KEY0;
        SMB_Tx_Buf[2] = FLASH_KEY1;
        SMB_Tx_Buf[3] = addr & 0xFF;
        SMB_Tx_Buf[4] = (addr >> 8) & 0xFF;
        // Other bytes of the message are "don't care".

        SMB_Write(CMD_PKG_SIZE);
        SMB_Read(1);
    }
    // Response:
    // [0] Return code (ACK/ERROR etc)/

    return SMB_Rx_Buf[0];
}

//-----------------------------------------------------------------------------
// TGT_Write_Flash
//-----------------------------------------------------------------------------
//
// Return Value : Response
// Parameters   : None
//
// Write 32 bytes to bootloader
//
//-----------------------------------------------------------------------------
U8 TGT_Write_Flash(U8 *buf, U32 addr, U16 numbytes)
{
    U8 i;

    if ((addr < APP_FW_START_ADDR) || (addr > APP_FW_END_ADDR) || ((addr
            + numbytes - 1) > APP_FW_END_ADDR))
    {
        SMB_Rx_Buf[0] = TGT_RSP_PARAMETER_INVALID;
    }
    else
    {
        // Command Format:
        // [0] Command
        // [1] flash key code0
        // [2] flash key code1
        // [3] addr0 (LSB)
        // [4] addr1 (MSB)
        // [5] numbytes
        SMB_Tx_Buf[0] = TGT_CMD_WRITE_FLASH_BYTES;
        SMB_Tx_Buf[1] = FLASH_KEY0;
        SMB_Tx_Buf[2] = FLASH_KEY1;
        SMB_Tx_Buf[3] = addr & 0xFF;
        SMB_Tx_Buf[4] = (addr >> 8) & 0xFF;
        SMB_Tx_Buf[5] = numbytes;

        for (i = 0; i < numbytes; i++)
        {
            SMB_Tx_Buf[i + CMD_PKG_SIZE] = *(buf + i);
        }
        // Other bytes of the message are "don't care".

        SMB_Write(CMD_PKG_SIZE + numbytes);
        SMB_Read(1);
    }
    // Response:
    // [0] Return code (ACK/ERROR etc)

    return SMB_Rx_Buf[0];
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
void TGT_SW_Reset(void)
{

    // Command Format:
    // [0] Command

    SMB_Tx_Buf[0] = TGT_CMD_RESET_MCU;
    // Other bytes of the message are "don't care".

    SMB_Write(CMD_PKG_SIZE);
    SMB_Read(1);
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SMBus Interrupt Service Routine (ISR)
//-----------------------------------------------------------------------------
//
// SMBus ISR state machine
// - Master only implementation - no slave or arbitration states defined
// - All incoming data is written to global variable array <SMB_Rx_Buf>
// - All outgoing data is read from global variable array <SMB_Tx_Buf>
//
//-----------------------------------------------------------------------------
INTERRUPT(SMBUS0_ISR, INTERRUPT_SMBUS0)
{
    bit FAIL = 0; // Used by the ISR to flag failed
    // transfers

    static U8 sent_byte_counter;
    static U8 rec_byte_counter;

    if (ARBLOST == 0) // Check for errors

    {
        // Normal operation
        switch (SMB0CN & 0xF0) // Status vector

        {
            // Master Transmitter/Receiver: START condition transmitted.
            case SMB_MTSTA:
            SMB0DAT = TARGET | SMB_RW; // Load address of the target slave
            STA = 0; // Manually clear START bit
            rec_byte_counter = 1; // Reset the counter
            sent_byte_counter = 1; // Reset the counter
            break;

            // Master Transmitter: Data byte transmitted
            case SMB_MTDB:
            if (ACK) // Slave ACK?

            {
                if (SMB_RW == WRITE) // If this transfer is a WRITE,

                {
                    if (sent_byte_counter <= SMB_NUM_BYTES)
                    {
                        // send data byte
                        SMB0DAT = SMB_Tx_Buf[sent_byte_counter-1];
                        sent_byte_counter++;
                    }
                    else
                    {
                        STO = 1; // Set STO to terminate transfer
                        SMB_BUSY = 0; // And free SMBus interface
                    }
                }
                else
                {} // If this transfer is a READ,
                // proceed with transfer without
                // writing to SMB0DAT (switch
                // to receive mode)
            }
            else // If slave NACK,

            {
                STO = 1; // Send STOP condition, followed
                SMB_BUSY = 0; // And free SMBus interface
                //               STA = 1;                // By a START
                NUM_ERRORS++; // Indicate error
            }
            break;

            // Master Receiver: byte received
            case SMB_MRDB:
            if (rec_byte_counter < SMB_NUM_BYTES)
            {
                SMB_Rx_Buf[rec_byte_counter-1] = SMB0DAT; // Store received
                // byte
                ACK = 1; // Send ACK to indicate byte received
                rec_byte_counter++; // Increment the byte counter
            }
            else
            {
                SMB_Rx_Buf[rec_byte_counter-1] = SMB0DAT; // Store received
                // byte
                SMB_BUSY = 0; // Free SMBus interface
                ACK = 0; // Send NACK to indicate last byte
                // of this transfer

                STO = 1; // Send STOP to terminate transfer
            }
            break;

            default:
            FAIL = 1; // Indicate failed transfer
            // and handle at end of ISR
            break;

        } // end switch
    }
    else
    {
        // ARBLOST = 1, error occurred... abort transmission
        FAIL = 1;
    } // end ARBLOST if

    if (FAIL) // If the transfer failed,

    {
        SMB0CF &= ~0x80; // Reset communication
        SMB0CF |= 0x80;
        STA = 0;
        STO = 0;
        ACK = 0;

        SMB_BUSY = 0; // Free SMBus

        FAIL = 0;

        NUM_ERRORS++; // Indicate an error occurred
    }

    SI = 0; // Clear interrupt flag
}
//-----------------------------------------------------------------------------
// SMB_Write
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Writes a single byte to the slave with address specified by the <TARGET>
// variable.
// Calling sequence:
// 1) Write target slave address to the <TARGET> variable
// 2) Write outgoing data to the <SMB_Tx_Buf> variable array
// 3) Call SMB_Write()
//
//-----------------------------------------------------------------------------
void SMB_Write(U8 nbytes)
{
    while (SMB_BUSY)
        ; // Wait for SMBus to be free.
    SMB_NUM_BYTES = nbytes;
    SMB_BUSY = 1; // Claim SMBus (set to busy)
    SMB_RW = 0; // Mark this transfer as a WRITE
    STA = 1; // Start transfer
}

//-----------------------------------------------------------------------------
// SMB_Read
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Reads a single byte from the slave with address specified by the <TARGET>
// variable.
// Calling sequence:
// 1) Write target slave address to the <TARGET> variable
// 2) Call SMB_Write()
// 3) Read input data from <SMB_Rx_Buf> variable array
//
//-----------------------------------------------------------------------------
void SMB_Read(U8 nbytes)
{
    while (SMB_BUSY)
        ; // Wait for bus to be free.
    SMB_NUM_BYTES = nbytes;
    SMB_BUSY = 1; // Claim SMBus (set to busy)
    SMB_RW = 1; // Mark this transfer as a READ

    STA = 1; // Start transfer

    while (SMB_BUSY)
        ; // Wait for transfer to complete
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
