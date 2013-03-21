//-----------------------------------------------------------------------------
// F390_Blinky.c
//-----------------------------------------------------------------------------
// Copyright (C) 2007 Silicon Laboratories, Inc.
//
// AUTH: HF
// DATE: 04 FEB 2003
//
// This program flashes the green LED on the C8051F39x target board about
// five times a second using the interrupt handler for Timer2.
//
// Target: C8051F390
//
// Tool chain: KEIL Eval 'c'
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>  // this file
#include <C8051F390_defs.h>                    // SFR declarations

//-----------------------------------------------------------------------------
// 16-bit SFR Definitions for 'F33x
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define SYSCLK       24500000 / 8         // SYSCLK frequency in Hz

sbit LED = P1^1;                          // LED='1' means ON

//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------
void SYSCLK_Init (void);
void PORT_Init (void);
void Timer2_Init (int counts);
void Timer2_ISR (void);

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
void main (void) {

   // disable watchdog timer
   PCA0MD &= ~0x40;                       // WDTE = 0 (clear watchdog timer
                                          // enable)

   SYSCLK_Init ();                        // Initialize system clock to
                                          // 24.5MHz
   PORT_Init ();                          // Initialize crossbar and GPIO
   Timer2_Init (SYSCLK / 12 / 10);        // Init Timer2 to generate
                                          // interrupts at a 10Hz rate.

   EA = 1;							      // enable global interrupts

   while (1) {                            // spin forever
   }
}

//-----------------------------------------------------------------------------
// SYSCLK_Init
//-----------------------------------------------------------------------------
//
// This routine initializes the system clock to use the internal 24.5MHz / 8
// oscillator as its clock source.  Also enables missing clock detector reset.
//
void SYSCLK_Init (void)
{

   OSCICN = 0x80;                         // configure internal oscillator for
                                          // its lowest frequency
   RSTSRC = 0x04;                         // enable missing clock detector
}

//-----------------------------------------------------------------------------
// PORT_Init
//-----------------------------------------------------------------------------
//
// Configure the Crossbar and GPIO ports.
//
void PORT_Init (void)
{

   P1SKIP   = 0x02;                       // skip the LED pin from crossbar
   P1MDOUT |= 0x02;                       // enable LED as a push-pull output
   XBR0     = 0x00;                       // no digital peripherals selected
   XBR1     = 0x40;                       // Enable crossbar and weak pull-ups
}

//-----------------------------------------------------------------------------
// Timer2_Init
//-----------------------------------------------------------------------------
//
// Configure Timer2 to 16-bit auto-reload and generate an interrupt at
// interval specified by <counts> using SYSCLK/48 as its time base.
//
void Timer2_Init (int counts)
{
   TMR2CN  = 0x00;                        // Stop Timer2; Clear TF2;
                                          // use SYSCLK/12 as timebase
   CKCON  &= ~0x60;                       // Timer2 clocked based on T2XCLK;

   TMR2RL  = -counts;                     // Init reload values
   TMR2    = 0xffff;                      // set to reload immediately
   ET2     = 1;                           // enable Timer2 interrupts
   TR2     = 1;                           // start Timer2
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Timer2_ISR
//-----------------------------------------------------------------------------
// This routine changes the state of the LED whenever Timer2 overflows.
//
void Timer2_ISR (void) interrupt 5
{
   TF2H = 0;                              // clear Timer2 interrupt flag
   LED = ~LED;                            // change state of LED
}

