/*****************************************************************
*
* Title:    Project 2 RS232 Comunnication handler
* Authors:  James and Ovi and Kevin
* Purpose:  To control the robot
* Date:     March 8, 2016
*
*****************************************************************/
//INCLUDES
#include <hidef.h>           /* common defines and macros       */
#include "derivative.h"      /* derivative-specific definitions */
#include <limits.h>
#include "macros.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
//#include <string.h>

/*****************************************************************
*
* GLOBALS
*
*****************************************************************/
volatile int readFlg = FALSE;

int servoFlg = FALSE;
volatile int i = 0;
volatile char readChar;
volatile char writeChar = 's';
char inputBuf[32];
int offset = 0;
int testAtoi = 0;
extern rdg;
int target = 85;

//STEPPER MOTOR
extern unsigned char RTICTL_INIT;
extern int STEP_TYPE;
extern int NUMSTEPS;
extern int EXPECTED_STEPS;
extern int CURRENT_POSITION;
extern volatile int homingHit;
          

//DC MOTOR
//extern int DC_MOTOR_PORT;


/*****************************************************************
*
* FUNCTION DECLARATIONS
*
*****************************************************************/
//LCD FUNCTIONS 
extern void  setupTimer(void);
extern void  LCDinit(void);
extern void LCDprintf(char *fmt, ...);
extern void LCDclr(void);
extern void LCDcmd(unsigned char data);
extern void LCDprintCMD(char * buf);
extern void stepper_INIT(void);
extern void msDelay( int );
//extern void LCDclrBOT(void)
#define LCD_CURSOR_HOME      0b00000010    //reset cursor position to top left

//SERVO FUNCTIONS
extern void setupTimerServo(void);
extern void disableServo(void);
extern void enableServo(void);

//DC MOTOR FUNCTION
extern void DCinit(void);
extern void stopDCMotor(void);
/*****************************************************************
* MAIN()
* Purpose: INIT AND PROCESS INTERRUPTS 
* inputs: N/A
* outputs: N/A 
*
*****************************************************************/  
void main(void) 
{
//LOCALS
signed int  arguments[4];
unsigned char stringArg[100];
unsigned char discard[10];
int j = 0;
int k = 0;
int CMDRDYflg = 0;
SCIBD = 52; //baud clock for 9600 with 8MHz E-clock 
SCICR1 = 0; //N81 data

// turn on transmitter and receiver
SCICR2 = (SCICR2_RE_MASK | SCICR2_TE_MASK |SCICR2_RIE_MASK ); //SCICR2_TCIE_MASK);

testAtoi = atoi("F");


/******* SETUPS SERVO **********/

  setupTimerServo();

/******* SETUPS LCD **********/
  //setupTimer();
  LCDinit();
/*************************/
 
 
/****** SETUP STEPPER *********/
 stepper_INIT();
 stepper_homing();

/****** SETUP DC MOTOR *********/
DCinit(); 
 
  EnableInterrupts;
  LCDprintf("-- Hi I'm Rob --");
  
/****** SETUP ENCODERS ********/

  //encodersInit();
  
  enableServo();
  
  
  for(;;)

  

/* TRANSMIT TO PSP TESTING ***** 
  for(;;)
  {
      char * s = "HI OVI";
 
      for (i = 0 ; i < 6 ;){
             
         if ((SCISR1 & SCISR1_TDRE_MASK))
          SCIDRL = s[i++];
         
      }

      for (i = 0 ; i < INT_MAX ; i++) {         
        for (k = 0 ; k < 60 ; k++);
      }

  }     
*/  
  
   
 for(;;)
          
      {
      
     if(readFlg == TRUE){
           //If new message clear buffer
           if(readChar == '<')
           {      
                 CMDRDYflg = 0;          
                 for(;j<32;j++){
                    inputBuf[j]=0; 
                 }
             j = 0;
           }    
           //Write into our buffer
           if(readChar != '<' && readChar != '>')
           {
               inputBuf[offset] = readChar;
               offset++; 
           }           
           if(readChar == '>'){ 
               inputBuf[offset] = '\0';
               CMDRDYflg = 1;
               offset = 0;
               LCDclrBOT();       
               LCDprintCMD(inputBuf);
               
           }
          
          DisableInterrupts;
          readFlg = FALSE; 
          EnableInterrupts;
  
     } //end of readFLG
     
     if(CMDRDYflg == 1) {      
       
         DisableInterrupts;
         CMDRDYflg = 0;
         EnableInterrupts;
         
       switch(inputBuf[0]) 
       {
        case('R') :
          sscanf((const char *)inputBuf, "%s" "%d", discard, &target);
      
          // arguments[0] now holds the argument? Degrees?        
          if(target < 65)
            target = 65;
          else if(target > 170)
            target = 170;
          //enableServo();
          
        
          break;

        case('P'):    
        
          /* The +2 to the buffer (char * input) is there to skip
           * the L and start the copy at the actual string.  
           */        
          strcpy(stringArg, (const char *)inputBuf + 2);
          
          // stringArg now holds the string to be printed on the LCD
          LCDclrTOP();
          LCDcmd(LCD_CURSOR_HOME);
          LCDprintf(stringArg);
          
          break;
      
        case('S'):
       
         
         sscanf((const char *)inputBuf, "%s" "%d" "%d", discard, 
          &arguments[0], &arguments[1]);
         
      /* Arguments now hold, respectively, as integers:
       *    0: SPEED 
       *    1: DEGREE
       */ 
          DisableInterrupts;
               
         NUMSTEPS = 0;
         EXPECTED_STEPS = 0;
          
          if(arguments[0] == 1)
            RTICTL_INIT = 0x7F;
          else if(arguments[0] == 2)
            RTICTL_INIT = 0x5A;
          else
            RTICTL_INIT = 0x70;
          
          if (CURRENT_POSITION > arguments[1]) {       
             STEP_TYPE = -1;
             EXPECTED_STEPS = (CURRENT_POSITION - arguments[1]) * 2;             
          } else {
             STEP_TYPE = 1;
             EXPECTED_STEPS = (arguments[1] - CURRENT_POSITION) * 2;
          }  
          
          CURRENT_POSITION = arguments[1]; 
          RTICTL = RTICTL_INIT; 
          EnableInterrupts;
          
        //stepper_INIT();
      
          break;
          

        
        case('H'):
          
          homingHit = 0;
          
          stepper_homing();
          break;
     
        case('D'):   
        //arguments[0] = Direction
        //arguments[1] = % speed
          sscanf((const char *)inputBuf, "%s" "%d" "%d", discard, 
          &arguments[0], &arguments[1]);
          
          if(arguments[0] == 0)
            stopDCMotor();
          else if(arguments[0] == 1){
            DC_MOTOR_PORT = 0b00001010;
          } 
          else{
            DC_MOTOR_PORT = 0b00000101;
          }
          
          PWMDTY5 = ((-127*(arguments[1]))/100)+127;
          PWMDTY4 = ((-127*(arguments[1]))/100)+127;
            
          break;
        
        default:
          LCDprintCMD("- Error - ");
     }
        //This delay is to compensate for linux read
               for (i = 0 ; i < INT_MAX ; i++)
                  for (k = 0 ; k < 40 ; k++);
               //Handshake with linux supervisor
               for (i = 0 ; i < 6 ;){                       
                   if ((SCISR1 & SCISR1_TDRE_MASK))
                      SCIDRL = inputBuf[i++];              
               }
            
     }       
 }
  
}  //main



/*****************************************************************
* SCI INTTERUPT
* Purpose: CHECKS FOR DATA AT SCI
* inputs: N/A
* outputs: N/A 
*
*****************************************************************/
interrupt 20 void SCI_Interrupt(){

int j=0;

  i++;

   if ((SCISR1 & SCISR1_RDRF_MASK)) 
  //reading
  {
   readChar = SCIDRL;
   readFlg = TRUE;
  }   
}

