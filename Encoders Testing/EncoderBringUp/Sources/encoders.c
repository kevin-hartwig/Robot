#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "macros.h"
#include "encoders.h"

/*****************************************************************
*
* HARDWARE PORT SETUP
*
*****************************************************************/
volatile static unsigned int R_Count;
volatile static unsigned int L_Count;

void encodersInit(void) {

  R_Count = 0; 
  L_Count = 0;
  
  MAKE_CHNL_IC(RIGHT_WHEEL);
  MAKE_CHNL_IC(LEFT_WHEEL);
  
  TCTL4 = 0b00001010;          // Hardware will detect only rising edges - Last 4 bits represent channel 1, 0 (respectively)              
  
  //REMOVE WHEN INTEGRATING
    TSCR1 = 0b10110000;        // starting Tcount , enabling freeze, and enabling fast clear 
    TSCR2 = 0b00000011;        // setting prescaler to 8 (last 3 bits)               
  
  TIE = 0b00000011;            // Enable interrupts on channel 0 & 1
  
  CLR_TIMER_CHNL_FLAG(0);      // Clear pending interrupts
  CLR_TIMER_CHNL_FLAG(1);
  
  
}


int getDistance(int wheel) {

  if (wheel == RIGHT_WHEEL) {
    return((((R_Count*100)/WHEEL_ROTATION)*ENCODER_TO_MM)/100);
  } else {      
    return((L_Count/WHEEL_ROTATION)*ENCODER_TO_MM);
  }
  
}


/*****************************************************************
*/

interrupt 8 void RightWheel( void ) {
   (void)TC0;                  // Pay the pizza guy
   
   
   R_Count++;     
}

interrupt 9 void LeftWheel( void ) {
   (void)TC1;                  // Pay the pizza guy
   
   
   L_Count++;
}


