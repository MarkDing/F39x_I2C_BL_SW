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

#include <compiler_defs.h>
#include <C8051F390_defs.h>            // SFR declarations

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define  SYSCLK         49000000/2     // System clock frequency in Hz

#define  SMB_FREQUENCY  77044          // Target SMBus frequency
                                       // This example supports between 10kHz
                                       // and 100kHz

#define  WRITE          0x00           // SMBus WRITE command
#define  READ           0x01           // SMBus READ command

#define  SLAVE_ADDR     0x20           // Device addresses (7 bits,
                                       // lsb is a don't care)

// Status vector - top 4 bits only
#define  SMB_SRADD      0x20           // (SR) slave address received
                                       //    (also could be a lost
                                       //    arbitration)
#define  SMB_SRSTO      0x10           // (SR) STOP detected while SR or ST,
                                       //    or lost arbitration
#define  SMB_SRDB       0x00           // (SR) data byte received, or
                                       //    lost arbitration
#define  SMB_STDB       0x40           // (ST) data byte transmitted
#define  SMB_STSTO      0x50           // (ST) STOP detected during a
                                       //    transaction; bus error
// End status vector definition

#define  NUM_BYTES_WR   6              // Number of bytes to write
                                       // Slave <- Master
#define  NUM_BYTES_RD   6              // Number of bytes to read
                                       // Slave -> Master

//-----------------------------------------------------------------------------
// Global VARIABLES
//-----------------------------------------------------------------------------

// Global holder for SMBus data.
// All receive data is written here
// NUM_BYTES_WR used because an SMBus write is Master->Slave
U8 SMB_DATA_IN[NUM_BYTES_WR];

// Global holder for SMBus data.
// All transmit data is read from here
// NUM_BYTES_RD used because an SMBus read is Slave->Master
U8 SMB_DATA_OUT[NUM_BYTES_RD];

bit DATA_READY = 0;                    // Set to '1' by the SMBus ISR
                                       // when a new data byte has been
                                       // received.

SBIT (LED, SFR_P1, 1);                 // LED==1 means ON
SBIT (P1_0_SWITCH, SFR_P1, 0);

#define TGT_CMD_ENTER_BL_MODE       0x05
#define RESERVED_SPACE_ADDR         0x4000
//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------

void Timer2_Init (int counts);
void Timer2_ISR (void);
void SMBus_Init (void);
void Timer1_Init (void);
void Timer3_Init (void);
void Port_Init (void);
U8 Enter_BL_Mode (void);
INTERRUPT_PROTO (TIMER3_ISR, INTERRUPT_TIMER3);
INTERRUPT_PROTO (SMBUS0_ISR, INTERRUPT_SMBUS0);

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
//
// Main routine performs all configuration tasks, then waits for SMBus
// communication.
//
//-----------------------------------------------------------------------------
void main (void)
{
   U8 i;

   PCA0MD &= ~0x40;                    // WDTE = 0 (Disable watchdog
                                       // timer)


   OSCICN |= 0x03;                     // Set internal oscillator to
                                       // 24.5 MHz

   Port_Init();                        // Initialize Crossbar and GPIO
//   while(P1_0_SWITCH);
   Timer1_Init();                      // Configure Timer1 for use
                                       // with SMBus baud rate
   Timer2_Init (65535 );  				   // Init Timer2 to generate

   Timer3_Init ();                     // Configure Timer3 for use with
                                       // SCL low timeout detect

   SMBus_Init ();                      // Configure and enable SMBus

   EIE1 |= 0x01;                       // Enable the SMBus interrupt

   LED = 0;

   EA = 1;                             // Global interrupt enable

   // Initialize the outgoing data array in case a read is done before a
   // write
   for (i = 0; i < NUM_BYTES_RD; i++)
   {
      SMB_DATA_OUT[i] = 0xFD;
   }

   while(1)
   {
	   for (i = 0; i < NUM_BYTES_RD; i++)
	   {
	      SMB_DATA_IN[i] = 0xFD;
	   }
	  while(!DATA_READY);              // New SMBus data received?
      DATA_READY = 0;

      // Copy the data from the input array to the output array
      for (i = 0; i < NUM_BYTES_RD; i++)
      {
         SMB_DATA_OUT[i] = SMB_DATA_IN[i];
      }
	  if(SMB_DATA_IN[0] == TGT_CMD_ENTER_BL_MODE)
	  {
	      i = Enter_BL_Mode();
	  }
   }
}

//-----------------------------------------------------------------------------
// Initialization Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SMBus_Init()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// SMBus configured as follows:
// - SMBus enabled
// - Slave mode not inhibited
// - Timer1 used as clock source. The maximum SCL frequency will be
//   approximately 1/3 the Timer1 overflow rate
// - Setup and hold time extensions enabled
// - Bus Free and SCL Low timeout detection enabled
//
//-----------------------------------------------------------------------------
void SMBus_Init (void)
{
   SMB0CF = 0x1D;                      // Use Timer1 overflows as SMBus clock
                                       // source;
                                       // Enable slave mode;
                                       // Enable setup & hold time
                                       // extensions;
                                       // Enable SMBus Free timeout detect;
                                       // Enable SCL low timeout detect;

   SMB0CF |= 0x80;                     // Enable SMBus;
}

//-----------------------------------------------------------------------------
// Timer1_Init()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Timer1 configured as the SMBus clock source as follows:
// - Timer1 in 8-bit auto-reload mode
// - SYSCLK or SYSCLK / 4 as Timer1 clock source
// - Timer1 overflow rate => 3 * SMB_FREQUENCY
// - The resulting SCL clock rate will be ~1/3 the Timer1 overflow rate
// - Timer1 enabled
//
//-----------------------------------------------------------------------------
void Timer1_Init (void)
{

// Make sure the Timer can produce the appropriate frequency in 8-bit mode
// Supported SMBus Frequencies range from 10kHz to 100kHz.  The CKCON register
// settings may need to change for frequencies outside this range.
#if ((SYSCLK/SMB_FREQUENCY/3) < 255)
   #define SCALE 1
      CKCON |= 0x08;                   // Timer1 clock source = SYSCLK
#elif ((SYSCLK/SMB_FREQUENCY/4/3) < 255)
   #define SCALE 4
      CKCON |= 0x01;
      CKCON &= ~0x0A;                  // Timer1 clock source = SYSCLK / 4
#endif

   TMOD = 0x20;                        // Timer1 in 8-bit auto-reload mode

   // Timer1 configured to overflow at 1/3 the rate defined by SMB_FREQUENCY
   TH1 = -(SYSCLK/SMB_FREQUENCY/SCALE/3);

   TL1 = TH1;                          // Init Timer1

   TR1 = 1;                            // Timer1 enabled
}

//-----------------------------------------------------------------------------
// Timer3_Init()
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
void Timer3_Init (void)
{
   TMR3CN = 0x00;                      // Timer3 configured for 16-bit auto-
                                       // reload, low-byte interrupt disabled

   CKCON &= ~0x40;                     // Timer3 uses SYSCLK/12

   TMR3RL = -(SYSCLK/12/40);           // Timer3 configured to overflow after
   TMR3 = TMR3RL;                      // ~25ms (for SMBus low timeout detect):
                                       // 1/.025 = 40

   EIE1 |= 0x80;                       // Timer3 interrupt enable
   TMR3CN |= 0x04;                     // Start Timer3
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
// P0.2   digital   open-drain    SMBus SDA
// P0.3   digital   open-drain    SMBus SCL
//
// P1.1   digital   push-pull     LED
//
// all other port pins unused
//
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   P1MDOUT |= 0x02;                    // Make the LED (P1.1) a push-pull
   P0SKIP = 0x03;
   XBR0 = 0x04;                        // Route SMBus signals to port pins
   XBR1 = 0x40;                        // Enable crossbar
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
// Enter_BL_Mode
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function will cause a Flash Error Reset to enter BL mode.
//
//-----------------------------------------------------------------------------
U8 Enter_BL_Mode (void)
{
   // The return value is just to prevent the compiler from optimizing out
   // the code read

   return *(U8 code*)(RESERVED_SPACE_ADDR);
}



//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// SMBus Interrupt Service Routine (ISR)
//-----------------------------------------------------------------------------
//
// SMBus ISR state machine
// - Slave only implementation - no master states defined
// - All incoming data is written to global variable <SMB_data_IN>
// - All outgoing data is read from global variable <SMB_data_OUT>
//
//-----------------------------------------------------------------------------
INTERRUPT(SMBUS0_ISR, INTERRUPT_SMBUS0)
{
   static U8 sent_byte_counter;
   static U8 rec_byte_counter;

   if (ARBLOST == 0)
   {
      switch (SMB0CN & 0xF0)           // Decode the SMBus status vector
      {
         // Slave Receiver: Start+Address received
         case  SMB_SRADD:

            STA = 0;                   // Clear STA bit

            sent_byte_counter = 1;     // Reinitialize the data counters
            rec_byte_counter = 1;

            if((SMB0DAT&0xFE) == (SLAVE_ADDR&0xFE)) // Decode address
            {                          // If the received address matches,
               ACK = 1;                // ACK the received slave address

               if((SMB0DAT&0x01) == READ) // If the transfer is a master READ,
               {
                  // Prepare outgoing byte
                  SMB0DAT = SMB_DATA_OUT[sent_byte_counter-1];
                  sent_byte_counter++;
               }
            }
            else                       // If received slave address does not
            {                          // match,
               ACK = 0;                // NACK received address
            }
            break;

         // Slave Receiver: Data received
         case  SMB_SRDB:

            if (rec_byte_counter < NUM_BYTES_WR)
            {
               // Store incoming data
               SMB_DATA_IN[rec_byte_counter-1] = SMB0DAT;
               rec_byte_counter++;

               ACK = 1;                // ACK received data
            }
            else
            {
               // Store incoming data
               SMB_DATA_IN[rec_byte_counter-1] = SMB0DAT;

               DATA_READY = 1;         // Indicate new data fully received
               ACK = 1;                // ACK received data
            }

            break;

         // Slave Receiver: Stop received while either a Slave Receiver or
         // Slave Transmitter
         case  SMB_SRSTO:

            DATA_READY = 1;         // Indicate new data fully received
            STO = 0;                   // STO must be cleared by software when
                                       // a STOP is detected as a slave
            break;

         // Slave Transmitter: Data byte transmitted
         case  SMB_STDB:

            if (ACK == 1)              // If Master ACK's, send the next byte
            {
               if (sent_byte_counter <= NUM_BYTES_RD)
               {
                  // Prepare next outgoing byte
                  SMB0DAT = SMB_DATA_OUT[sent_byte_counter-1];
                  sent_byte_counter++;
               }
            }                          // Otherwise, do nothing
            break;

         // Slave Transmitter: Arbitration lost, Stop detected
         //
         // This state will only be entered on a bus error condition.
         // In normal operation, the slave is no longer sending data or has
         // data pending when a STOP is received from the master, so the TXMODE
         // bit is cleared and the slave goes to the SRSTO state.
         case  SMB_STSTO:
			DATA_READY = 1;
            STO = 0;                   // STO must be cleared by software when
                                       // a STOP is detected as a slave
            break;

         // Default: all other cases undefined
         default:

            SMB0CF &= ~0x80;           // Reset communication
            SMB0CF |= 0x80;
            STA = 0;
            STO = 0;
			DATA_READY = 1;
            ACK = 0;
            break;
      }
   }
   // ARBLOST = 1, Abort failed transfer
   else
   {
      STA = 0;
      STO = 0;
      ACK = 0;
	  DATA_READY = 1;
   }

   SI = 0;                             // Clear SMBus interrupt flag
}

//-----------------------------------------------------------------------------
// Timer3 Interrupt Service Routine (ISR)
//-----------------------------------------------------------------------------
//
// A Timer3 interrupt indicates an SMBus SCL low timeout.
// The SMBus is disabled and re-enabled here
//
//-----------------------------------------------------------------------------
INTERRUPT(TIMER3_ISR, INTERRUPT_TIMER3)
{
   SMB0CF &= ~0x80;                    // Disable SMBus
   SMB0CF |= 0x80;                     // Re-enable SMBus
   TMR3CN &= ~0x80;                    // Clear Timer3 interrupt-pending flag
}

//-----------------------------------------------------------------------------
// Timer2_ISR
//-----------------------------------------------------------------------------
// This routine changes the state of the LED whenever Timer2 overflows.
//
void Timer2_ISR (void) interrupt 5
{
   static count = 0;
   TF2H = 0;                              // clear Timer2 interrupt flag
   if(count++ > 10)
   {
   	  count = 0;
      LED = ~LED;                            // change state of LED
   }
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------