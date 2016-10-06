#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "macros.h"
#include "encoders.h"



void main(void) {
  /* put your own code here */
  
  int DistanceTravelled = 0;


	
	
	encodersInit();
	
	EnableInterrupts;


  for(;;) {
  
     DistanceTravelled = getDistance(RIGHT_WHEEL);
  
  
    _FEED_COP(); /* feeds the dog */
  } /* loop forever */
  /* please make sure that you never leave main */
}
