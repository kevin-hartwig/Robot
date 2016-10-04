#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "macros.h"

/*****************************************************************
*
* HARDWARE PORT SETUP
*
*****************************************************************/

#define RIGHT_WHEEL (0)        // Right wheel on timer channel 0
#define LEFT_WHEEL  (1)        // Left wheel on timer channel 1

//Globals
unsigned int R_Count;
unsigned int L_Count;

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


/*****************************************************************
*/

interrupt 8 void RightWheel( void ) {
   R_Count++;     
}

interrupt 9 void LeftWheel( void ) {
   L_Count++;
}