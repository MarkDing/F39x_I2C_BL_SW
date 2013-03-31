$NOMOD51
;------------------------------------------------------------------------------
;  This file is part of the C51 Compiler package
;  Copyright (c) 1988-2005 Keil Elektronik GmbH and Keil Software, Inc.
;  Version 8.01
;
;  *** <<< Use Configuration Wizard in Context Menu >>> ***
;------------------------------------------------------------------------------
;  STARTUP.A51:  This code is executed after processor reset.
;
;  To translate this file use A51 with the following invocation:
;
;     A51 STARTUP.A51
;
;  To link the modified STARTUP.OBJ file to your application use the following
;  Lx51 invocation:
;
;     Lx51 your object file list, STARTUP.OBJ  controls
;
;------------------------------------------------------------------------------
;******************************************************************************
; Copyright (c) 2009, Silicon Laboratories, Inc.
;
; Filename  : Fnnn_TargetBL_Startup.A51
; Devices   : C8051Fnnn
; Toolchain : Keil
;
; Description:
;
; [*** TEMPLATE INFORMATION ***]
; Filename: Fnnn_TargetBL_Startup.A51
; nnn => Fill-in with the MCU family part number.
; Examples: F50x_TargetBL_Startup.A51, F34x_TargetBL_Startup.A51
;
; Contains the power-on initialization and interrupt vector redirect code
; for the modular target bootloader project.
;
; Note: This file is a modified version of the original STARTUP.A51 file
; supplied with the Keil tool chain (Version 8).
;
; Revision History:
;
; Release 1.0 / 18NOV2009 (PKC)
;    -Initial Revision
;
;******************************************************************************
;
;  User-defined <h> Power-On Initialization of Memory
;
;  With the following EQU statements the initialization of memory
;  at processor reset can be defined:
;
; <o> IDATALEN: IDATA memory size <0x0-0x100>
;     <i> Note: The absolute start-address of IDATA memory is always 0
;     <i>       The IDATA space overlaps physically the DATA and BIT areas.
IDATALEN        EQU     00H
;
; <o> XDATASTART: XDATA memory start address <0x0-0xFFFF>
;     <i> The absolute start address of XDATA memory
XDATASTART      EQU     0
;
; <o> XDATALEN: XDATA memory size <0x0-0xFFFF>
;     <i> The length of XDATA memory in bytes.
XDATALEN        EQU     0
;
; <o> PDATASTART: PDATA memory start address <0x0-0xFFFF>
;     <i> The absolute start address of PDATA memory
PDATASTART      EQU     0H
;
; <o> PDATALEN: PDATA memory size <0x0-0xFF>
;     <i> The length of PDATA memory in bytes.
PDATALEN        EQU     0H
;
;</h>
;------------------------------------------------------------------------------
;
;<h> Reentrant Stack Initialization
;
;  The following EQU statements define the stack pointer for reentrant
;  functions and initialized it:
;
; <h> Stack Space for reentrant functions in the SMALL model.
;  <q> IBPSTACK: Enable SMALL model reentrant stack
;     <i> Stack space for reentrant functions in the SMALL model.
IBPSTACK        EQU     0       ; set to 1 if small reentrant is used.
;  <o> IBPSTACKTOP: End address of SMALL model stack <0x0-0xFF>
;     <i> Set the top of the stack to the highest location.
IBPSTACKTOP     EQU     0xFF +1     ; default 0FFH+1
; </h>
;
; <h> Stack Space for reentrant functions in the LARGE model.
;  <q> XBPSTACK: Enable LARGE model reentrant stack
;     <i> Stack space for reentrant functions in the LARGE model.
XBPSTACK        EQU     0       ; set to 1 if large reentrant is used.
;  <o> XBPSTACKTOP: End address of LARGE model stack <0x0-0xFFFF>
;     <i> Set the top of the stack to the highest location.
XBPSTACKTOP     EQU     0xFFFF +1   ; default 0FFFFH+1
; </h>
;
; <h> Stack Space for reentrant functions in the COMPACT model.
;  <q> PBPSTACK: Enable COMPACT model reentrant stack
;     <i> Stack space for reentrant functions in the COMPACT model.
PBPSTACK        EQU     0       ; set to 1 if compact reentrant is used.
;
;   <o> PBPSTACKTOP: End address of COMPACT model stack <0x0-0xFFFF>
;     <i> Set the top of the stack to the highest location.
PBPSTACKTOP     EQU     0xFF +1     ; default 0FFH+1
; </h>
;</h>
;------------------------------------------------------------------------------
;
;  Memory Page for Using the Compact Model with 64 KByte xdata RAM
;  <e>Compact Model Page Definition
;
;  <i>Define the XDATA page used for PDATA variables.
;  <i>PPAGE must conform with the PPAGE set in the linker invocation.
;
; Enable pdata memory page initalization
PPAGEENABLE     EQU     0       ; set to 1 if pdata object are used.
;
; <o> PPAGE number <0x0-0xFF>
; <i> uppermost 256-byte address of the page used for PDATA variables.
PPAGE           EQU     0
;
; <o> SFR address which supplies uppermost address byte <0x0-0xFF>
; <i> most 8051 variants use P2 as uppermost address byte
PPAGE_SFR       DATA    0A0H
;
; </e>
;------------------------------------------------------------------------------

; Standard SFR Symbols
ACC     DATA    0E0H
B       DATA    0F0H
SP      DATA    81H
DPL     DATA    82H
DPH     DATA    83H

;******************************************************************************
; Begin custom code for SMB_BL - Target_BL_FW ->
;******************************************************************************

;------------------------------------------------------------------------------
; Redirects the HW interrupt vectors to a jump table starting at INTVEC_TABLE.
; This table will always be part of the App_FW space and can be modified by the
; bootloader's ERASE and WRITE commands.

;HW Interrupt Entries
HW_INTVEC_TABLE       EQU 0003h     ;HW Interrupt vector table starts here
HW_INTVEC_SEPARATION  EQU 8         ;HW Interrupt vector separation is 8 Bytes

;Redirected Interrupt Entries
;Changes to INTVEC_TABLE location should be mirrored in:
;   BL FW's USB_BL_Defs.h (APP_START_PAGE)
;   App FW's STARTUP.A51, Compiler and Linker command lines
PUBLIC START_APPLICATION            ;The entry point (relocated reset vector)
START_APPLICATION     EQU  0200h    ;for the application FW project

INTVEC_TABLE          EQU  START_APPLICATION+3  ;Interrupt vector table starts here
INTVEC_SEPARATION     EQU  3        ;Interrupt vector separation is 3 Bytes

      CSEG  AT HW_INTVEC_TABLE + (HW_INTVEC_SEPARATION * 0)  ;03h (Intr. 00)
      AJMP  INTVEC_TABLE + (INTVEC_SEPARATION * 0)

      CSEG  AT HW_INTVEC_TABLE + (HW_INTVEC_SEPARATION * 1)  ;0Bh (Intr. 01)
      AJMP  INTVEC_TABLE + (INTVEC_SEPARATION * 1)

      CSEG  AT HW_INTVEC_TABLE + (HW_INTVEC_SEPARATION * 2)  ;13h (Intr. 02)
      AJMP  INTVEC_TABLE + (INTVEC_SEPARATION * 2)

      CSEG  AT HW_INTVEC_TABLE + (HW_INTVEC_SEPARATION * 3)  ;1Bh (Intr. 03)
      AJMP  INTVEC_TABLE + (INTVEC_SEPARATION * 3)

      CSEG  AT HW_INTVEC_TABLE + (HW_INTVEC_SEPARATION * 4)  ;23h (Intr. 04)
      AJMP  INTVEC_TABLE + (INTVEC_SEPARATION * 4)

      CSEG  AT HW_INTVEC_TABLE + (HW_INTVEC_SEPARATION * 5)  ;2Bh (Intr. 05)
      AJMP  INTVEC_TABLE + (INTVEC_SEPARATION * 5)

      CSEG  AT HW_INTVEC_TABLE + (HW_INTVEC_SEPARATION * 6)  ;33h (Intr. 06)
      AJMP  INTVEC_TABLE + (INTVEC_SEPARATION * 6)

      CSEG  AT HW_INTVEC_TABLE + (HW_INTVEC_SEPARATION * 7)  ;3Bh (Intr. 07)
      AJMP  INTVEC_TABLE + (INTVEC_SEPARATION * 7)

;      CSEG  AT HW_INTVEC_TABLE + (HW_INTVEC_SEPARATION * 8)  ;43h (Intr. 08)
;      AJMP  INTVEC_TABLE + (INTVEC_SEPARATION * 8)

      CSEG  AT HW_INTVEC_TABLE + (HW_INTVEC_SEPARATION * 9)  ;4Bh (Intr. 09)
      AJMP  INTVEC_TABLE + (INTVEC_SEPARATION * 9)

      CSEG  AT HW_INTVEC_TABLE + (HW_INTVEC_SEPARATION * 10) ;53h (Intr. 10)
      AJMP  INTVEC_TABLE + (INTVEC_SEPARATION * 10)

      CSEG  AT HW_INTVEC_TABLE + (HW_INTVEC_SEPARATION * 11) ;5Bh (Intr. 11)
      AJMP  INTVEC_TABLE + (INTVEC_SEPARATION * 11)

      CSEG  AT HW_INTVEC_TABLE + (HW_INTVEC_SEPARATION * 12) ;63h (Intr. 12)
      AJMP  INTVEC_TABLE + (INTVEC_SEPARATION * 12)

;      CSEG  AT HW_INTVEC_TABLE + (HW_INTVEC_SEPARATION * 13) ;6Bh (Intr. 13)
;      AJMP  INTVEC_TABLE + (INTVEC_SEPARATION * 13)

      CSEG  AT HW_INTVEC_TABLE + (HW_INTVEC_SEPARATION * 14) ;73h (Intr. 14)
      AJMP  INTVEC_TABLE + (INTVEC_SEPARATION * 14)


;******************************************************************************
;End of custom code <-
;Unmodified Keil STARTUP.A51 code continues below ->
;******************************************************************************

                NAME    ?C_STARTUP


?C_C51STARTUP   SEGMENT   CODE
?STACK          SEGMENT   IDATA

                RSEG    ?STACK
                DS      1

                EXTRN CODE (?C_START)
                PUBLIC  ?C_STARTUP

                CSEG    AT      0
?C_STARTUP:     AJMP    STARTUP1

                RSEG    ?C_C51STARTUP

STARTUP1:

IF IDATALEN <> 0
                MOV     R0,#IDATALEN - 1
                CLR     A
IDATALOOP:      MOV     @R0,A
                DJNZ    R0,IDATALOOP
ENDIF

IF XDATALEN <> 0
                MOV     DPTR,#XDATASTART
                MOV     R7,#LOW (XDATALEN)
  IF (LOW (XDATALEN)) <> 0
                MOV     R6,#(HIGH (XDATALEN)) +1
  ELSE
                MOV     R6,#HIGH (XDATALEN)
  ENDIF
                CLR     A
XDATALOOP:      MOVX    @DPTR,A
                INC     DPTR
                DJNZ    R7,XDATALOOP
                DJNZ    R6,XDATALOOP
ENDIF

IF PPAGEENABLE <> 0
                MOV     PPAGE_SFR,#PPAGE
ENDIF

IF PDATALEN <> 0
                MOV     R0,#LOW (PDATASTART)
                MOV     R7,#LOW (PDATALEN)
                CLR     A
PDATALOOP:      MOVX    @R0,A
                INC     R0
                DJNZ    R7,PDATALOOP
ENDIF

IF IBPSTACK <> 0
EXTRN DATA (?C_IBP)

                MOV     ?C_IBP,#LOW IBPSTACKTOP
ENDIF

IF XBPSTACK <> 0
EXTRN DATA (?C_XBP)

                MOV     ?C_XBP,#HIGH XBPSTACKTOP
                MOV     ?C_XBP+1,#LOW XBPSTACKTOP
ENDIF

IF PBPSTACK <> 0
EXTRN DATA (?C_PBP)
                MOV     ?C_PBP,#LOW PBPSTACKTOP
ENDIF

                MOV     SP,#?STACK-1

; This code is required if you use L51_BANK.A51 with Banking Mode 4
;<h> Code Banking
; <q> Select Bank 0 for L51_BANK.A51 Mode 4
#if 0
;     <i> Initialize bank mechanism to code bank 0 when using L51_BANK.A51 with Banking Mode 4.
EXTRN CODE (?B_SWITCH0)
                CALL    ?B_SWITCH0      ; init bank mechanism to code bank 0
#endif
;</h>
                AJMP    ?C_START

                END
