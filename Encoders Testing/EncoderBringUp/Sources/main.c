#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "macros.h"
#include "encoders.h"



void main(void) {
  /* put your own code here */
  
  int DistanceTravelled = 0;
  unsigned long frequency = 0;
  unsigned long ticks = 0;
  unsigned long speed = 0; 
  unsigned long TCNT_VAR = 0; 	
	
	encodersInit();
	
	EnableInterrupts;


  for(;;) {   
     
     DistanceTravelled = getDistance(RIGHT_WHEEL);        
     ticks = getTCNTDifference(RIGHT_WHEEL);
     frequency = convertTCNT(ticks);
     speed = convertFrequency(frequency);   
     TCNT_VAR = TCNT;  
  
  
  
    _FEED_COP(); /* feeds the dog */
  } /* loop forever */
  /* please make sure that you never leave main */
}
