//-----------------------------------------------------------------------------
// F39x_Init.c - SMB_BL - Master_BL_FW
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

#ifndef MASTER_MCU_BL
#define MASTER_MCU_BL
#endif

#include "F39x_Master_Interface.h"

#include "F39x_UART.h"
#include "F39x_SMB.h"

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Function Prototypes (Local)
//-----------------------------------------------------------------------------
void OSCILLATOR_Init(void);
void PORT_Init(void);
void UART0_Init(void);
void TIMER0_Init(void);

//-----------------------------------------------------------------------------
// Main Routine
//-----------------------------------------------------------------------------

//=============================================================================
// Function Definitions
//=============================================================================
void Device_Init(void)
{
    OSCILLATOR_Init(); // Configure system clock

    PORT_Init(); // Initialize crossbar

    UART0_Init(); // Init UART0 & Timer1 (Data source comm)

    SMBus_Init(); // Init SMB0 (Target MCU comm) Mark.Ding

    TIMER0_Init(); // Init Timer0 for LED blinks
}

//-----------------------------------------------------------------------------
// OSCILLATOR_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine initializes the system clock to use the internal oscillator
// at its maximum frequency.
// Also enables the Missing Clock Detector.
//-----------------------------------------------------------------------------
void OSCILLATOR_Init(void)
{

    OSCICN = 0x83; // Set system clock to 24.5 MHz
    VDM0CN = 0x80; // Enable VDD monitor and early warning
    RSTSRC = 0x02; // Enable VDD monitor as a reset source
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
// P0.2   digital  open-drain    SDA0
// P0.3   digital  open-drain    SCL0
//
// P1.1   digital  push-pull     LED
// P1.0   digital  open-drain    SW

//-----------------------------------------------------------------------------
void PORT_Init(void)
{
    P0MDOUT = 0x10; // Enable UART0 TX as push-pull output
    P1MDOUT = 0x02; // P1.1 (LED) is push-pull
    P0SKIP = 0x03;
    XBR0 = 0x05; // Enable UART0 and SMBUS0
    XBR1 = 0x40; // Enable crossbar
}

//-----------------------------------------------------------------------------
// TIMER0_Init
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
// Configure Timer0 to 16-bit Timer mode and generate an interrupt
// every TIMER0_RL Timer0 cycles using SYSCLK/48 as the Timer0 time base.
//
//-----------------------------------------------------------------------------
void TIMER0_Init(void)
{

    TH0 = TIMER0_RL_SLOW_HIGH; // Init Timer0 High register
    TL0 = TIMER0_RL_SLOW_LOW; // Init Timer0 Low register

    TMOD |= 0x01; // Timer0 in 16-bit mode
    CKCON |= 0x02; // Timer0 uses a 1:48 prescaler
    ET0 = 1; // Timer0 interrupt enabled
}

//-----------------------------------------------------------------------------
// UART0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure the UART0 using Timer1, for <BAUDRATE> and 8-N-1.
//-----------------------------------------------------------------------------
void UART0_Init(void)
{
    SCON0 = 0x10; // SCON0: 8-bit variable bit rate
    //        level of STOP bit is ignored
    //        RX enabled
    //        ninth bits are zeros
    //        clear RI0 and TI0 bits
    if (SYSCLK / BAUDRATE / 2 / 256 < 1)
    {
        TH1 = -(SYSCLK / BAUDRATE / 2); // 0x96
        CKCON &= ~0x0B; // T1M = 1; SCA1:0 = xx
        CKCON |= 0x08;
    }
    else if (SYSCLK / BAUDRATE / 2 / 256 < 4)
    {
        TH1 = -(SYSCLK / BAUDRATE / 2 / 4);
        CKCON &= ~0x0B; // T1M = 0; SCA1:0 = 01
        CKCON |= 0x01;
    }
    else if (SYSCLK / BAUDRATE / 2 / 256 < 12)
    {
        TH1 = -(SYSCLK / BAUDRATE / 2 / 12);
        CKCON &= ~0x0B; // T1M = 0; SCA1:0 = 00
    }
    else
    {
        TH1 = -(SYSCLK / BAUDRATE / 2 / 48);
        CKCON &= ~0x0B; // T1M = 0; SCA1:0 = 10
        CKCON |= 0x02;
    }

    TL1 = TH1; // init Timer1
    TMOD &= ~0xf0; // TMOD: timer 1 in 8-bit autoreload
    TMOD |= 0x20;
    TR1 = 1; // START Timer1
    IP |= 0x10; // Make UART high priority
    ES0 = 1; // Enable UART0 interrupts
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
