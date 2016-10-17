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
  unsigned long speed = 0; 
  unsigned long TCNT_VAR = 0; 	
	  
	encodersInit();
	DCinit(); 
	
	EnableInterrupts;
	
	
	
  DC_MOTOR_PORT = 0b00001010;
  PWMDTY5 = ((-127*(10))/100)+127;
  PWMDTY4 = ((-127*(10))/100)+127;

  for(;;) {   
  
     if (!checkOverflows(RIGHT_WHEEL)) 
        ticks = getCount(RIGHT_WHEEL);
     else 
        ticks = 0;
     frequency = convertTCNT(ticks);
     speed = convertFrequency(frequency);   
     TCNT_VAR = TCNT;  
  
  
  
    _FEED_COP(); /* feeds the dog */
  } /* loop forever */
  /* please make sure that you never leave main */
}
