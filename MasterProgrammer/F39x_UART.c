//-----------------------------------------------------------------------------
// F39x_UART.c - SMB_BL - Master_BL_FW
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

#ifndef MASTER_MCU_BL
#define MASTER_MCU_BL
#endif

#include "F39x_Master_Interface.h"

#include "F39x_UART.h"

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------
#define MAX_TX_BYTES             25
#define MAX_RX_BYTES_LOCAL       25

//-----------------------------------------------------------------------------
// Global Variables (Accessed by all modules)
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Global Variables (Accessed only by functions in this module)
//-----------------------------------------------------------------------------
U16 Rx_Num_Bytes = 0;
U8 Rx_Complete_Flag = 0;
U16 Rx_Buf_Index = 0;

U16 Tx_Num_Bytes = 0;
U8 Tx_Complete_Flag = 0;
U8 Tx_Buf_Index = 0;

SEGMENT_VARIABLE( Tx_Buf[MAX_TX_BYTES], U8, SEG_XDATA);

SEGMENT_VARIABLE( Rx_Buf_Local[MAX_RX_BYTES_LOCAL], U8, SEG_XDATA);

U8 *Rx_Buf_Ptr;

//-----------------------------------------------------------------------------
// Function Prototypes (Local)
//-----------------------------------------------------------------------------
INTERRUPT_PROTO (UART0_ISR, INTERRUPT_UART0);
void Setup_Rx(U16 num_bytes, U8 *buf_ptr);

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
U8 SRC_Get_Info(U8 *src_info)
{
    U8 index;

    // Command Format:
    // [0] Command

    Tx_Complete_Flag = 0;
    Tx_Buf[0] = SRC_CMD_GET_INFO;
    Tx_Num_Bytes = 1;
    Tx_Buf_Index = 0;

    Setup_Rx(SRC_CMD_GET_INFO_RX_SZ, Rx_Buf_Local);

    TI0 = 1; // Start transmission

    while (Tx_Complete_Flag == 0)
        ; // Wait for Tx complete

    while (Rx_Complete_Flag == 0)
        ; // Wait for Rx complete

    // Response:
    // [0] Return code (ACK/ERROR etc)
    // [1] InfoBlock Length
    // [2] MCU Code
    // [3] BL Type
    // [4] Flash Page Size Code
    // [5] Application FW Version Low (0)
    // [6] Application FW Version High (1)
    // [7] SMB Device Address
    // [8] Application FW Start Addr Low (0)
    // [9] Application FW Start Addr (1)
    // [10] Application FW Start Addr High (2)
    // [11] Application FW End Addr Low (0)
    // [12] Application FW End Addr (1)
    // [13] Application FW End Addr High (2)

    for (index = 0; index < SRC_CMD_GET_INFO_RX_SZ; index++)
    {
        src_info[index] = *(Rx_Buf_Ptr + 1 + index);
    }

    /*
     src_info->BL_FW_ver = 0;
     src_info->MCU_code = *(Rx_Buf_Ptr+1);
     src_info->App_FW_ver = *(Rx_Buf_Ptr+2);
     src_info->App_FW_last_page_addr.U8[LSB] = *(Rx_Buf_Ptr+3);
     src_info->App_FW_last_page_addr.U8[MSB] = *(Rx_Buf_Ptr+4);
     */

    return (*Rx_Buf_Ptr);
}

//-----------------------------------------------------------------------------
// SRC_Get_Page_Info
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
//
//
//-----------------------------------------------------------------------------
U8 SRC_Get_Page_Info(U32 *addr, U16 *src_page_crc)
{

    // Command Format:
    // [0] Command

    Tx_Complete_Flag = 0;
    Tx_Buf[0] = SRC_CMD_GET_PAGE_INFO;
    Tx_Num_Bytes = 1;
    Tx_Buf_Index = 0;

    Setup_Rx(SRC_CMD_GET_PAGE_INFO_RX_SZ, Rx_Buf_Local);

    TI0 = 1; // Start transmission

    while (Tx_Complete_Flag == 0)
        ; // Wait for Tx complete

    while (Rx_Complete_Flag == 0)
        ; // Wait for Rx complete

    // Response:
    // [0] Return code (ACK/ERROR etc)
    // [1] Page address byte 0 {or Num Pages, if return code = SRC_RSP_DATA_END}
    // [2] Page address byte 1
    // [3] Page address byte 2 <-- don't care for MCUs 64k and smaller
    // [4] Page CRC byte 0
    // [5] Page CRC byte 1
    // Note: If Num Pages is provided, add 1 to get actual num pages

    *addr = ((*(Rx_Buf_Ptr + 3)) << 16) | ((*(Rx_Buf_Ptr + 2)) << 8)
            | (*(Rx_Buf_Ptr + 1));
    //addr->U8[LSB] = *(Rx_Buf_Ptr+1);
    //addr->U8[MSB] = *(Rx_Buf_Ptr+2);

    *src_page_crc = ((*(Rx_Buf_Ptr + 5)) << 8) | (*(Rx_Buf_Ptr + 4));
    //src_page_crc->U8[LSB] = *(Rx_Buf_Ptr+3);
    //src_page_crc->U8[MSB] = *(Rx_Buf_Ptr+4);


    return (*Rx_Buf_Ptr);
}

//-----------------------------------------------------------------------------
// SRC_Get_Page
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
//
//
//-----------------------------------------------------------------------------
U8 SRC_Get_Page(U8 *buf)
{

    // Command Format:
    // [0] Command

    Tx_Complete_Flag = 0;
    Tx_Buf[0] = SRC_CMD_GET_PAGE;
    Tx_Num_Bytes = 1;
    Tx_Buf_Index = 0;

    Setup_Rx(SRC_CMD_GET_PAGE_RX_SZ, buf);

    TI0 = 1; // Start transmission

    while (Tx_Complete_Flag == 0)
        ; // Wait for Tx complete

    while (Rx_Complete_Flag == 0)
        ; // Wait for Rx complete

    // Response:
    // [0] Return code (ACK/ERROR etc)
    // [1..512] Page data
    // [513] Return code (ACK/ERROR etc)


    return (*(Rx_Buf_Ptr + 513));
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
U8 SRC_Disp_TGT_Info(U8 buf_length, U8 *buf)
{
    U8 index;

    // Command Format:
    // [0] Command
    // [1] Bytes...

    Tx_Complete_Flag = 0;
    Tx_Buf[0] = SRC_CMD_DISP_TGT_INFO;

    for (index = 0; index < buf_length; index++)
    {
        Tx_Buf[index + 1] = *(buf + index);
    }

    Tx_Num_Bytes = buf_length + 1; // Add 1 for command byte
    Tx_Buf_Index = 0;

    Setup_Rx(SRC_CMD_DISP_TGT_INFO_RX_SZ, Rx_Buf_Local);

    TI0 = 1; // Start transmission

    while (Tx_Complete_Flag == 0)
        ; // Wait for Tx complete

    while (Rx_Complete_Flag == 0)
        ; // Wait for Rx complete

    // Response:
    // [0] Return code (ACK/ERROR etc)


    return (*Rx_Buf_Ptr);
}

//-----------------------------------------------------------------------------
// SRC_Disp_Info_Code
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
//
//
//-----------------------------------------------------------------------------
U8 SRC_Disp_Info_Code(U8 info_code)
{

    // Command Format:
    // [0] Command
    // [1] Info Code

    Tx_Complete_Flag = 0;
    Tx_Buf[0] = SRC_CMD_DISP_INFO_CODE;
    Tx_Buf[1] = info_code;
    Tx_Num_Bytes = 2;
    Tx_Buf_Index = 0;

    Setup_Rx(SRC_CMD_DISP_INFO_CODE_RX_SZ, Rx_Buf_Local);

    TI0 = 1; // Start transmission

    while (Tx_Complete_Flag == 0)
        ; // Wait for Tx complete

    while (Rx_Complete_Flag == 0)
        ; // Wait for Rx complete

    // Response:
    // [0] Return code (ACK/ERROR etc)


    return (*Rx_Buf_Ptr);
}

//-----------------------------------------------------------------------------
// Setup_Rx
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
//
//
//-----------------------------------------------------------------------------
void Setup_Rx(U16 num_bytes, U8 *buf_ptr)
{
    Rx_Complete_Flag = 1;
    Rx_Num_Bytes = num_bytes;
    Rx_Buf_Index = 0;
    Rx_Buf_Ptr = buf_ptr;
    Rx_Complete_Flag = 0;
}

//=============================================================================
// Interrupt Service Routines
//=============================================================================

//-----------------------------------------------------------------------------
// UART0_Interrupt
//-----------------------------------------------------------------------------
//
// This routine is invoked whenever a character is sent or received via UART.
//
//-----------------------------------------------------------------------------
INTERRUPT (UART0_ISR, INTERRUPT_UART0)
{
    U8 rx_byte;

    if (RI0 == 1)
    {
        rx_byte = SBUF0; // Read a character from UART
        RI0 = 0; // Clear interrupt flag

        if (Rx_Complete_Flag == 0) // This means Rx is enabled

        {
            if (Rx_Buf_Index < Rx_Num_Bytes)
            {
                *(Rx_Buf_Ptr + Rx_Buf_Index) = rx_byte;
                Rx_Buf_Index++;
            }
            if (Rx_Num_Bytes == Rx_Buf_Index)
            {
                Rx_Complete_Flag = 1;
            }
        }
    }

    if (TI0 == 1) // Check if transmit flag is set

    {
        TI0 = 0; // Clear interrupt flag

        if (Tx_Complete_Flag == 0) // This means Tx is enabled

        {
            if (Tx_Buf_Index == Tx_Num_Bytes)
            {
                Tx_Complete_Flag = 1;
            }
            else
            {
                SBUF0 = Tx_Buf[Tx_Buf_Index];
                Tx_Buf_Index++;
            }
        }
    }
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
