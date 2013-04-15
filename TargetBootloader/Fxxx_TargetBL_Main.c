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
// Release 1.1 / 31Mar2013 (MarkD)
//    -F390 SMBUS Revision
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
#include "Fxxx_BL132_SMB0_Interface.h"
//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
SBIT(BL_Override_Pin, SFR_P1, 0);

#define APP_MODE        0
#define BOOTLOADER_MODE 1

// Bit masks for the RSTSRC SFR
#define PORSF  0x02
#define FERROR 0x40


//=============================================================================
// Main Routine
//=============================================================================
void main(void)
{
    U8 device_mode = BOOTLOADER_MODE;
	U8 code* codeptr;
    //---------------------------------------
    // Check the bootloader consition.
    //---------------------------------------
    codeptr = (U8 code*)(APP_FW_SIG0_ADDR);
   // The Signature (in Flash) should be valid to allow application FW execution.
   // This is written at the end of the bootloading process by the bootloader.
	if (*codeptr == SIG_BYTE0)
    {
		*codeptr--;
    	if (*codeptr == SIG_BYTE1)
     	{
			*codeptr--;
        	if (*codeptr == SIG_BYTE2)
        	{
				*codeptr--;
            	if (*codeptr == SIG_BYTE3)
		        {
	                // All signature bytes match. 
					device_mode = APP_MODE;
      		    }
        	}
      	}
	}
    if ((!BL_Override_Pin) || (((RSTSRC & PORSF) == 0) && ((RSTSRC & FERROR)
            != 0)))
    {
        device_mode = BOOTLOADER_MODE;
    }


    if (device_mode == APP_MODE)
    {
        // If not in BL Override, jump to application
        START_APPLICATION();
    }

    //-------------------------------------------
    // ** BL Mode ** Initialize MCU and Variables
    //-------------------------------------------
    Init_Device();

    //-------------------------------------------
    // Main Loop
    //-------------------------------------------

    while (1)
    {
        // Wait until a command is received
        SMB0_Op(6);

        switch (Rx_Buf[0])
        {
            case TGT_CMD_RESET_MCU:
                Set_TX_TGT_RSP_OK();
                SMB0_Op(1);
                RSTSRC = 0x12; // Initiate software reset with vdd monitor enabled
                break;
            case TGT_CMD_ERASE_FLASH_PAGE:
                TGT_Erase_Page();
                break;
            case TGT_CMD_WRITE_FLASH_BYTES:
                TGT_Write_Flash();
                break;
            case TGT_CMD_ENTER_BL_MODE:
                Set_TX_TGT_RSP_BL_MODE();
                break;
            default:
                Set_TX_TGT_RSP_UNSUPPORTED_CMD();
                break;
        }

        // Send response
        SMB0_Op(1);
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
void Set_TX_TGT_RSP_OK(void)
{
    Tx_Buf[0] = TGT_RSP_OK;
}

//-----------------------------------------------------------------------------
// Set_TX_TGT_RSP_BL_MODE
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
// Sets TX response code to TGT_RSP_BL_MODE.
//-----------------------------------------------------------------------------
void Set_TX_TGT_RSP_BL_MODE(void)
{
    Tx_Buf[0] = TGT_RSP_BL_MODE;
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
// Set_TX_TGT_RSP_UNSUPPORTED_CMD
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
// Sets TX response code to Set_TX_TGT_RSP_UNSUPPORTED_CMD.
//-----------------------------------------------------------------------------
void Set_TX_TGT_RSP_UNSUPPORTED_CMD (void)
{
   Tx_Buf[0] = TGT_RSP_UNSUPPORTED_CMD;
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
