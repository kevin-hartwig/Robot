#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "macros.h"
#include "encoders.h"
#include "utils.h"

int SETPOINT = 10;
int SetSpeedR = 0;
int SetSpeedL = 0;             


void main(void) {
  /* put your own code here */
  
  int DistanceTravelled = 0;
  unsigned int frequencyR = 0, frequencyL = 0;
  unsigned long ticks = 0;
  unsigned long speedR = 0; 
  unsigned long speedL = 0;
  unsigned long TCNT_VAR = 0; 	
	  
	encodersInit();
	DCinit(); 
	
	EnableInterrupts;
	
	
	
  DC_MOTOR_PORT = 0b00001010;
  PWMDTY5 = ((-127*(SETPOINT))/100)+127;
  PWMDTY4 = ((-127*(SETPOINT))/100)+127;

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
    
    SetSpeedR = (((speedR-87)*100)/317);
    SetSpeedL = (((speedL-107)*100)/322);
  
    //PWMDTY5 = SetSpeedL;
    //PWMDTY4 = SetSpeedR;
    if(SETPOINT > SetSpeedL)
      PWMDTY5 = ((-127*(SETPOINT+(SETPOINT-SetSpeedL)))/100)+127;
    else if(SETPOINT <= SetSpeedL)
      PWMDTY5 = ((-127*(SETPOINT+(SetSpeedL-SETPOINT)))/100)+127;
    if(SETPOINT > SetSpeedR)
      PWMDTY4 = ((-127*(SETPOINT+(SETPOINT-SetSpeedR)))/100)+127;
    else if(SETPOINT <= SetSpeedR)
      PWMDTY4 = ((-127*(SETPOINT+(SetSpeedR-SETPOINT)))/100)+127;
        
    _FEED_COP(); /* feeds the dog */
  } /* loop forever */
  /* please make sure that you never leave main */
}
