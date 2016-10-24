#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "macros.h"
#include "encoders.h"
#include "utils.h"



void main(void) {
  /* put your own code here */
  
  int DistanceTravelled = 0;
  unsigned int frequency = 0;
  unsigned long ticks = 0;
  unsigned long speedR = 0; 
  unsigned long speedL = 0;
  unsigned long TCNT_VAR = 0; 	
	  
	encodersInit();
	DCinit(); 
	
	EnableInterrupts;
	
	
	
  DC_MOTOR_PORT = 0b00001010;
  PWMDTY5 = ((-127*(100))/100)+127;
  PWMDTY4 = ((-127*(100))/100)+127;

  for(;;) { 
  
     //Right Wheel  
     if (!checkOverflows(RIGHT_WHEEL)) 
        ticks = getCount(RIGHT_WHEEL);
     else 
        ticks = 0;
     frequency = convertTCNT(ticks);
     speedR = convertFrequency(frequency, RIGHT_WHEEL);  
     
     
     
     //Left Wheel
     if (!checkOverflows(LEFT_WHEEL)) 
        ticks = getCount(LEFT_WHEEL);
     else 
        ticks = 0;
     //frequency = convertTCNT(ticks);
     speedL = convertFrequency(frequency, LEFT_WHEEL);   
    
  
  
    _FEED_COP(); /* feeds the dog */
  } /* loop forever */
  /* please make sure that you never leave main */
}
