//-----------------------------------------------------------------------------
// Fnnn_TargetBL_DevSpecific.c
//-----------------------------------------------------------------------------
// Copyright (C) 2010 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// [*** TEMPLATE INFORMATION ***]
// Filename: Fnnn_TargetBL_DevSpecific.c
// nnn => Fill-in with the MCU family part number.
// Examples: F50x_TargetBL_DevSpecific.c, F34x_TargetBL_DevSpecific.c
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

// ------------------------------------------
// *** Device-Specific *** 
// Change this as necessary based on MCU
// ------------------------------------------
SBIT (P1_0_SWITCH, SFR_P1, 0);
SBIT (P1_1_LED, SFR_P1, 1);

#define LIN_TX_PUSH_PULL 0 // 0=Open-drain; 1=Push-pull output

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Function Prototypes (Local)
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void);
void PORT_Init (void);


//=============================================================================
// Function Definitions
//=============================================================================

//-----------------------------------------------------------------------------
// Disable_Watchdog
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
//
//
//-----------------------------------------------------------------------------
void Disable_Watchdog (void)
{
   // *** Device-Specific *** Change this as necessary based on MCU.
   PCA0MD &= ~0x40;
}

//-----------------------------------------------------------------------------
// Init_Device
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
//
//
//-----------------------------------------------------------------------------
void Init_Device (void)
{
   OSCILLATOR_Init ();
   PORT_Init ();

   SRC_Comm_Init ();

//   Comm_Init ();

   // *** Add calls to any other init functions here as necessary ***
}

//-----------------------------------------------------------------------------
// OSCILLATOR_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine initializes the system clock to use the internal oscillator
// at its maximum frequency (24 MHz); Also enables the VDD monitor set to its
// high threshold and selected as a reset source.
//
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void)
{
   OSCICN = 0x83;                      // Set system clock to 24.5 MHz
   VDM0CN = 0x80;                      // Enable VDD monitor and early warning
   RSTSRC = 0x02;                      // Enable VDD monitor as a reset source
}

//-----------------------------------------------------------------------------
// PORT_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure the Crossbar and GPIO ports.
//
// P0.4   digital   push-pull    UART0 TX
// P0.5   digital   open-drain   UART0 RX
//
// P0.0   digital  open-drain    SDA0
// P0.1   digital  open-drain    SCL0
//
// P1.1   digital  push-pull     LED
// P1.0   digital  open-drain    SW

//-----------------------------------------------------------------------------
void PORT_Init (void)
{
    P0MDOUT   = 0x10;					// Enable UART0 TX as push-pull output
    P1MDOUT   = 0x02;					// P1.1 (LED) is push-pull
    XBR0      = 0x05;					// Enable UART0 and SMBUS0
    XBR1      = 0x40;					// Enable crossbar
}

//-----------------------------------------------------------------------------
// Check_Pin_State
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
//
//-----------------------------------------------------------------------------
U8 Check_Pin_State (void)
{
   if (P1_0_SWITCH == 0)
   {
      return (PIN_ASSERTED); // Pin goes low when switch is pressed
   }
   else
   {
      return (PIN_NOT_ASSERTED);
   }
}

//-----------------------------------------------------------------------------
// Set_LED_State
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
//
//
//-----------------------------------------------------------------------------
void Set_LED_State (U8 state)
{
   switch (state)
   {
      case LED_OFF:
         P1_1_LED = 0;
         break;
      case LED_ON:
         P1_1_LED = 1;
         break;
      case LED_TOGGLE:
         P1_1_LED = !P1_1_LED;
         break;
      default:
         break;
   }
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
