/*****************************************************************
*
* Title: Project 2 RS232 Comunnication handler
* Authors: James and Ovi and Kevin
* Purpose: To control the robot
* Date: April 16, 2016
*
*****************************************************************/


//INLCUDES
#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include <stdio.h>
#include "macros.h"

/*****************************************************************
*
* GLOBALS
*
*****************************************************************/
int DIRECTION_STEPPER;
int PATTERN = 0;
int STEP_TYPE = 0;
unsigned char RTICTL_INIT = 0x7F;     // try putting this in function down below instead of declaring it.....
int CUR_PAT = 0;
int value = 0;
int checkLIMIT = 0;
int NUMSTEPS = 0;        //number of steps for servo motor
int EXPECTED_STEPS = 0;  //expected number of steps for servo motor
int STEP_POSITION = 0;   //keep track of our current position
volatile int homingHit = 0;
int stepCount = 0;
volatile int CURRENT_POSITION = 0;
/*****************************************************************
*
* HARDWARE PORT SETUP
*
*****************************************************************/
#define STEPPER_MOTOR_PORT PTT
#define STEPPER_MOTOR_PORT_DDR DDRT

#define LIMIT_SWITCH_RIGHT 0b10000000
#define LIMIT_SWITCH_LEFT  0b01000000
#define LIMIT_SWITCH_DDR DDRAD 
#define LIMIT_SWITCH PTAD 

/*****************************************************************
*
* STEPPER MOTOR SETUP CONSTANT VARIABLES
*
*****************************************************************/
#define pos_0   8
#define pos_1   10
#define pos_2   2
#define pos_3   6
#define pos_4   4
#define pos_5   5
#define pos_6   1
#define pos_7   9

#define mCRGFLG_RTIF 0b10000000
#define mCRGINT_RTIE 0b10000000



int posTable[8] = {pos_0,pos_1,pos_2,pos_3,pos_4,pos_5,pos_6,pos_7};
  

/*****************************************************************
*
* FUNCTION DECLARATIONS
*
*****************************************************************/

void stepper_INIT(void);
void stepper_homing(void);


/*****************************************************************
* MAIN()
*
* inputs: N/A
* outputs: N/A 
*
*****************************************************************/
void stepper_INIT(void){
  
  ATDDIEN = 0b11000000;
  
  LIMIT_SWITCH_DDR = 0;
  STEPPER_MOTOR_PORT_DDR = 0xF0;
  
  RTICTL = RTICTL_INIT;  //set RTI Interval
  CRGFLG = mCRGFLG_RTIF;  //clear any possibly pending interrupts    //april 13 uncomment
  SET_BITS (CRGINT, mCRGINT_RTIE);   //set RTI to generate interrupt
}


/*****************************************************************
* stepper_homing()    //finds the center 
*
* inputs: N/A
* outputs: N/A 
*
*****************************************************************/

void stepper_homing(void){
 
    DisableInterrupts;
    EXPECTED_STEPS = 10000;
    RTICTL_INIT = 0x70;
    STEP_TYPE = 1;
    RTICTL = RTICTL_INIT;
    stepCount = 0;
    EnableInterrupts;

}



interrupt 7 void RTI_Interrupt(){

static int limitFLG = 0;  //check if limit flag has been set

CRGFLG = mCRGFLG_RTIF;  //clear any possibly pending interrupts    //april 13 uncomment

if(NUMSTEPS <= EXPECTED_STEPS) {

//check if limit switch is pressed, if pressed do nothing...........
if(   ( (LIMIT_SWITCH_LEFT & LIMIT_SWITCH) || (LIMIT_SWITCH_RIGHT & LIMIT_SWITCH))  &&  !limitFLG  )  {
checkLIMIT++;

if(STEP_TYPE > 0)
  CURRENT_POSITION = 180;
if(STEP_TYPE < 0)
  CURRENT_POSITION = 0;

NUMSTEPS = EXPECTED_STEPS;

STEP_TYPE = STEP_TYPE * -1;

NUMSTEPS = 0;
EXPECTED_STEPS = 10;

limitFLG = 10;

homingHit++;
if(homingHit == 1){
  //STEP_TYPE = -1;
  EXPECTED_STEPS = 10000;

} 
else if(homingHit == 2){
  EXPECTED_STEPS = stepCount/2;
  CURRENT_POSITION = 90;
}
}

else{
      
    CUR_PAT = ((CUR_PAT+STEP_TYPE) & 0b00000111) ;
    value = posTable[ CUR_PAT ];
    value <<= 4;

    STEPPER_MOTOR_PORT &= ~(0b11110000);
    STEPPER_MOTOR_PORT |= value; 

    NUMSTEPS = NUMSTEPS + abs(STEP_TYPE);
    if (limitFLG > 0)
    limitFLG--;
    
    if (homingHit == 1) {
      stepCount++;
    }
  }//end else
}

}
