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
#include "encoders.h"
#include "utils.h"
//#include <string.h>

/*****************************************************************
*
* GLOBALS
*
*****************************************************************/
volatile int readFlg = FALSE;

//DC Motor Setpoint
int SETPOINT = 50;

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

//ENCODERS
extern int getDistance(int wheel);
extern signed int convertFrequency(unsigned int frequency, int side);
extern unsigned long convertTCNT(unsigned int TCNTDifference);
extern int UpdatePID(struct SPid * pid, int error, int position);
extern int getCounter(void);
extern unsigned int getCount(int side);
extern int checkOverflows(int side);
//extern interrupt 16 void timerOverFlow(void);
          

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
// ENCODER AND DC MOTOR VARIABLES / / / / / / / / / / / / / / / / / /
static long SetSpeedR = 0;
static long SetSpeedL = 0;

static int DistanceTravelled = 0;
static unsigned int frequencyR = 0, frequencyL = 0;
static unsigned long ticks = 0;

static unsigned long TCNT_VAR = 0; 
static int iterationCounter = 0;

SPid PIDControl;

long errorR = 0;
long errorL = 0;
// / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /
void main(void) 
{
  int servo_value = 90;
  int last_servo_value = 100;
  int last_stepper_value = 100;
  int counter = 0; 
  //LOCALS
  signed int  arguments[4];
  unsigned char stringArg[100];
  unsigned char discard[10];
  int j = 0;
  int k = 0;
  int CMDRDYflg = 0;
  
  signed int speedR = 0; 
  signed int speedL = 0;





  SCIBD = 13; //baud clock for 9600 (52 i think?) or 38400 (13) with 8MHz E-clock 
  SCICR1 = 0; //N81 data

  // turn on transmitter and receiver
  SCICR2 = (SCICR2_RE_MASK | SCICR2_TE_MASK |SCICR2_RIE_MASK ); //SCICR2_TCIE_MASK);

  testAtoi = atoi("F");

/******* ENCODER SETUP *********/
  PIDControl.pGain = 1;
  PIDControl.iGain = 1;
  PIDControl.iMin = 0;
  PIDControl.iMax = 59;
  
  


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

  LCDprintf("-- Hi I'm Rob --");
  
/****** SETUP ENCODERS ********/

  
  enableServo();
  
  PWMDTY4 = 140 - (((900/59)*(SETPOINT))/10); 
  PWMDTY5 = 140 - (((900/59)*(SETPOINT))/10);
  DC_MOTOR_PORT = 0b00001010;
  
  encodersInit();
  
  EnableInterrupts;
  
 /** Main Application Infinite Loop START **/ 
 for(;;) {
  
 
    /****************************************************
    Calculations for Wheel Speeds
    *****************************************************/
     //Right Wheel  
    if (!checkOverflows(RIGHT_WHEEL)) 
       ticks = getCount(RIGHT_WHEEL);                             
    else 
       ticks = 0;
    frequencyR = convertTCNT(ticks);
    speedR = convertFrequency(frequencyR, RIGHT_WHEEL);  
    
    //Left Wheel
    if (!checkOverflows(LEFT_WHEEL)) 
       ticks = getCount(LEFT_WHEEL);
    else 
       ticks = 0;
    frequencyL = convertTCNT(ticks);
    speedL = convertFrequency(frequencyL, LEFT_WHEEL);   
   /*****************************************************/ 

   /* PID Control Block *********************************/
   iterationCounter++;
   
   if (iterationCounter == 200) {
    iterationCounter = 0;
    errorR = SETPOINT - speedR;
    errorL = SETPOINT - speedL;
    SetSpeedR = UpdatePID(&PIDControl, errorR, speedR);
    SetSpeedL = UpdatePID(&PIDControl, errorL, speedL);    
    PWMDTY4 = 140 - (((900/59)*(SetSpeedR))/10); 
    PWMDTY5 = 140 - (((900/59)*(SetSpeedL))/10);        
   }/* PID Control Block END ****************************/  

 
      counter++;
 
      if (counter == 10000){
          counter = 0;
            
          // Check last_value's and modify flags as appropriate ///////////////////////
          // SERVO //        
          if (last_servo_value > 180 && target < 170 ) {
            target++;
          } else if (last_servo_value < 20 && target > 65) {
            target--;
          } 
          
          
          // STEPPER //
          if ((last_stepper_value > 150 || last_stepper_value < 50)){   
          
              counter = 0;   
              
              if (last_stepper_value > 150)
                 STEP_TYPE = 1;
              else 
                 STEP_TYPE = -1;
                       
              NUMSTEPS = 0;
              EXPECTED_STEPS = 1;  // (5 degrees per main loop iteration, have to multiply desired degree rotation by 2)

              CURRENT_POSITION = CURRENT_POSITION + (STEP_TYPE*1); 
              RTICTL = RTICTL_INIT; 
              
          } 
          
      }
        
     /** Read a command from the RS-232 Port START *************************************/ 
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
  
     } /** read a command from the RS-232 Port END **/
     
     
     /** Command Parsing START **********************************************************/
     if(CMDRDYflg == 1) {      
       
     DisableInterrupts;
     CMDRDYflg = 0;
     EnableInterrupts;
         
         switch(inputBuf[0]) 
         {
          case('R') :
            sscanf((const char *)inputBuf, "%s" "%d", discard, &last_servo_value);
                     
          
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
         
           
           sscanf((const char *)inputBuf, "%s" "%d", discard, &last_stepper_value);
        
        
            break;      

          
          case('U'):  // U is sent when Y button pressed!!! (Y used for DC motor Y-axis)
            
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
        
            
     }  /** Command Parsing END **/ 
         
 }  /** Main Application Infinite Loop END **/
  
}  /* end of main() */



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

