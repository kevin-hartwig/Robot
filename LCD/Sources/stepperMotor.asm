;******************************************************************************
; Program Name - Quick Description
;
; Detailed description with purpose and revision history
;
; Name:   James Sonnenberg
; Date    March 11, 2016
;
; Purpose: This program demonstrates pulse width modulation from HCS12C as a direct 
;          hardware means of generating a digital waveforms on the 6 PWM channels.
;          The output of this program is to brighten and dim two LEDs over a period
;          of 1 second. 
;
; (Header - purpose -  author - date - change history)
;******************************************************************************

;*************** Housekeeping *************************************************
            XDEF        Entry             ; make 'Entry' symbol available to debugger
            ABSENTRY    Entry             ; identify 'Entry' as program entry point
            INCLUDE     'derivative.inc'  ; include derivative-specific definitions    
ROMStart    EQU         ROM_4000Start     ; absolute ROM address to place code/constant data
; RAMStart defined in 'derivative.inc' as $800

;**************************************************************
;*                 Equates 
;* (style - ALL CAPS)
;**************************************************************

MOTOR       EQU     PTT                         ; I/O port that LEDS are on
MOTOR_DDR   EQU     DDRT                        ; DDR for I/O port with LEDs

LEDBANK             EQU     PTS                         ; I/O port that LEDS are on
LEDBANK_DDR         EQU     DDRS                        ; DDR for I/O port with LEDs
LEDBANK_DDR_INIT    EQU     (mDDRS_DDRS2 | mDDRS_DDRS3) ; init value for DDR - both LEDs are outputs
LEDBANK_INIT        EQU     (mPTS_PTS2)                 ; init value for LED port - only 1 on
LEDBANK_TOGGLE_MASK EQU     (mPTS_PTS2 | mPTS_PTS3)     ; mask to select LED bits for toggling


POS_0   EQU %10000000   ; .. 8
POS_1   equ %10100000   ; .. A
POS_2   equ %00100000   ; .. 2
POS_3   equ %01100000   ; .. 6 
POS_4   equ %01000000   ; .. 4 
POS_5   equ %01010000   ; .. 5
POS_6   equ %00010000   ; .. 1
POS_7   equ %10010000   ; .. 9

RTICTL_INIT         EQU     $7F     ; init value for RTICTL reg (16 x 2^10)

;BACKWARDS           EQU     1
;FORWARDS            EQU     0
;***********************************s***************************
;*                 RAM-resident variables
;* (style - initial lower case)
;**************************************************************
            ORG RAMStart

CUR_PAT   ds.b 1
STEP_TYPE ds.b 1           
            
;shared_byte         ds.b    1           ; var updated by ISR and used by mainline   
;nextval             ds.b    1         
;UpOrDown            ds.b    1           ; indicate direction of duty cycle change            
;**************************************************************
;*                 ROM-resident constants
;* (style - initial lower case)
;**************************************************************
            ORG ROMStart
            
PosTable dc.b POS_0,POS_1,POS_2,POS_3,POS_4,POS_5,POS_6,POS_7                                                
;**************************************************************
;*                 Code
;* (style - labels:    initial cap, mixed case, 
;*          mnemonics: lower case)
;**************************************************************           
            
Entry: 


        movb    #LEDBANK_DDR_INIT, LEDBANK_DDR  ; enable LEDs
        movb    #LEDBANK_INIT, LEDBANK
       ; clr     shared_byte                     ; clear out vars
        
;1.In your mainline init code
;  Initialize the stack to point to RAM with LDS instruction
        lds     #(RAMEnd + 1)
  
;  Configure the module as needed
        movb    #RTICTL_INIT, RTICTL    ; set RTI interval
        
;  Clear any possibly pending interrupts         
        movb    #mCRGFLG_RTIF,CRGFLG    ; clear any possibly pending interrupts
                
;  Enable interrupts from the module
        bset    CRGINT, mCRGINT_RTIE    ; set RTI to generate interrupt
        
;  Unmask interrupt handling with the CLI instruction
        
        BSR INIT        
        CLI                             ; ready to roll - enable interrupts
        
;  Do other stuff  (loop forever?)
        ldx     #$1234          ; put known values in registers so we can see on stack
        ldy     #$5678
        clra
     ;   movb #1, UpOrDown
        
loopMain:			
        ;LDAB    shared_byte     ; read the var updated by the ISR
        ;STAB    LEDBANK         ; write it's value to the LEDs

        BRA     loopMain        ; ... and around we go



; *** end of main code starting at Entry: ***

;**************************************************************
;*                 Initialization
;**************************************************************

INIT:   

        movb #0,CUR_PAT
        movb #1,STEP_TYPE
        movb #$F0,MOTOR_DDR
 
 ;  Setup done ... now do other stuff       
        rts        


;**************************************************************
;*                 Code -- ISRs
;**************************************************************
ISR_Heartbeat:

;  Clear any possibly pending interrupts         
        movb    #mCRGFLG_RTIF,CRGFLG    ; clear any possibly pending interrupts
            
;2.In your Interrupt Service Routine (ISR)
; ISR for /IRQ pin
; on falling edge on external /IRQ pin, move to next position 

; Stomp on the register values
        ldy     #0
        ldx     #0
        ldd     #0
        
        ldab CUR_PAT           ; Load current posiition
        ldx  #PosTable         ; Load table into register x
        ABX                    ; Move to current location in table
        ldaa STEP_TYPE         ; Prepare step type
        aba                    ; Add step type to position to offset properly
        
        anda #%00000111        ; Set unused bits to 0
        staa CUR_PAT           ; Save result

        ldab x                 ; Loab x into b
        stab MOTOR             ; Write to motor
        
;  Toggle LEDs on PORT S
        ldaa    LEDBANK
        eora    #LEDBANK_TOGGLE_MASK
        staa    LEDBANK
        
;  End with the RTI instruction		
;End_ISR_IRQ
        RTI                             ; ... back to where we came from 
;***************************************
;*                 Interrupt Vectors                          
;**************************************************************
;3.In your vector table
;  Set the appropriate vector location to point to your ISR
            ORG Vrti
RTI_VECTOR     dc.w	ISR_Heartbeat   ; real-time interrupt

            ORG Vreset
RESET_VECTOR    dc.w    Entry           ; Reset
;**************************************************************
;*                 End of Program
;**************************************************************