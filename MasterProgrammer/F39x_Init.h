//-----------------------------------------------------------------------------
// F39x_Init.h - SMB_BL - Master_BL_FW
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
// Header File Preprocessor Directive
//-----------------------------------------------------------------------------
#ifndef F39X_INIT_H
#define F39X_INIT_H

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------
#define SYSCLK           49000000/2      // SYSCLK frequency in Hz
// LED / Timer0 - Constants:

#define LED_SLOW_RATE   2              // Rate of LED blinks per sec.
// Minimum value is 1

#define LED_FAST_RATE   10             // Rate of LED blinks per sec.
// Minimum value is 1

// Reload value for Timer0 assuming a 1:48 scaling factor for Timer0 clock
#define TIMER0_RL_SLOW_HIGH  (0xFFFF -((SYSCLK/48/LED_SLOW_RATE >> 8) & 0x00FF))
#define TIMER0_RL_SLOW_LOW   (0xFFFF -(SYSCLK/48/LED_SLOW_RATE & 0x00FF))

// Reload value for Timer0 assuming a 1:48 scaling factor for Timer0 clock
#define TIMER0_RL_FAST_HIGH  (0xFFFF -((SYSCLK/48/LED_FAST_RATE >> 8) & 0x00FF))
#define TIMER0_RL_FAST_LOW   (0xFFFF -(SYSCLK/48/LED_FAST_RATE & 0x00FF))

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
void Device_Init(void);

//-----------------------------------------------------------------------------
// Header File Preprocessor Directive
//-----------------------------------------------------------------------------
#endif

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
