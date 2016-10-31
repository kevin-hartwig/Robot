#include "macros.h"

    
word TCNTTEMP;
int DIRECTION;
int EDGE;
unsigned int offsetRDG;
unsigned int rdg = 85;    //change this to 0 when panning motor left to right 
extern int target;

void disableServo(void);
void enableServo(void);

void setupTimerServo (void){
  
TIE |= 0b00000100;     //enable interrupts on channel 2
TSCR1 = 0b10110000;   //starting Tcount , enabling freeze, and enabling fast clear 
TSCR2 = 0b00000011;   //setting prescaler to 8 (last 3 bits)
MAKE_CHNL_OC(2);      //make channel 2 output compare  

FORCE_OC_ACTION_NOW( 2, OC_GO_HI ); //set channel 2, to go high
EDGE = 0;
DIRECTION = 1;
TCNTTEMP = TCNT;      //made a copy of TCNT
TC2 = TCNTTEMP;       //write sum to output compare register

CLR_TIMER_CHNL_FLAG(2);  //clear channel 2  FLG1
SET_OC_ACTION( 2, OC_TOGGLE ); //set channel 2, to toggle
}

void disableServo(void){
   rdg = 0;
  // msDelay(1500); 
   SET_OC_ACTION( 2, OC_OFF );
   servoFlg = FALSE;
   TIE = 0; //disable interrupts on channel 2 
   
  
}

void enableServo(void){
 servoFlg = TRUE;
 //setupTimerServo();
 
 FORCE_OC_ACTION_NOW( 2, OC_GO_HI ); //set channel 2, to go high
 EDGE = 0;
 DIRECTION = 1;
 CLR_TIMER_CHNL_FLAG(2);  //clear channel 2  FLG1
 SET_OC_ACTION( 2, OC_TOGGLE ); //set channel 2, to toggle
}


/*****************************************************************
* SERVO INTTERUPT
* Purpose: 
* inputs: N/A
* outputs: N/A 
*
*****************************************************************/
interrupt 10 void timerFunction(){
    
  if(rdg > target)
    rdg--;
  else if(rdg < target)
    rdg++;

  offsetRDG = (600+(rdg*10));          //change this to *7 when panning motor left to right - leave at *10 for when you want a specific degree

  if (EDGE == 1) /* if channel is set to generate rising edge */
  {
      TC2 += offsetRDG;/* set up timer compare for mark time */
      /* relative to the last transition */
      EDGE = 0; /* set pin action to falling edge */             
      
  }
  else
  {
      TC2 += (20000-offsetRDG);/* set up timer compare for space time */
      /* relative to the last transition */
      EDGE = 1; /* set pin action to rising edge */
  }

}
  
