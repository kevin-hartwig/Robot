#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "macros.h"
#include "encoders.h"

/*****************************************************************
*
* HARDWARE PORT SETUP
*
*****************************************************************/
volatile static unsigned int R_Count, L_Count;                                              // Track first vs second rising edge
volatile static unsigned int overflowcountR, overflowcountL, periodticksR, periodticksL;    // Count overflows, store period calculations in TCNT ticks
volatile static unsigned int EdgeL_1, EdgeL_2, EdgeR_1, EdgeR_2;                            // Store TCNT ticks for each rising edge


unsigned long MotorEncoderShaftSpeed;
unsigned long MotorOutputFrequency;
unsigned long FinalSpeed;

/******************************************************************
 * This function initializes timer channels for each wheel and
 * initializes all ISR variables to 0.
 *
 ******************************************************************/
void encodersInit(void) { 

  EdgeL_1 = 0;                 // Initialize variables used in ISRs
  EdgeL_2 = 0;
  EdgeR_1 = 0;
  EdgeR_2 = 0;                   
  periodticksR = 0;
  periodticksL = 0;              
  overflowcountR = 0;
  overflowcountL = 0;          
  R_Count = 0; 
  L_Count = 0;
 
  
  MAKE_CHNL_IC(RIGHT_WHEEL);   // Configure timer channels for input capture
  MAKE_CHNL_IC(LEFT_WHEEL);
  
  TCTL4 = 0b00001010;          // Hardware will detect only rising edges - Last 4 bits represent timer channels 1, 0 (respectively)              
  
  //REMOVE WHEN INTEGRATING
    TSCR1 = 0b10110000;        // starting Tcount , enabling freeze, and enabling fast clear 
    //TSCR2 = 0b00000011;        // setting prescaler to 8 (last 3 bits)  
    TSCR2 = 0x83;             
  
  TIE = 0b00000011;            // Enable interrupts on channel 0 & 1
  
  CLR_TIMER_CHNL_FLAG(0);      // Clear pending interrupts
  CLR_TIMER_CHNL_FLAG(1);
  
  
}

/******************************************************************
 * This function returns TRUE if TCNT has overflowed 5 times since
 * the last encoder interrupt fired.  Use to determine if a single
 * wheel has come to a halt.
 *
 * Pass RIGHT_WHEEL or LEFT_WHEEL
 *
 ******************************************************************/
int checkOverflows(int side) {
  
  if (side == RIGHT_WHEEL) {     
      if (overflowcountR > 5)
        return 1;
      else
        return 0;
  }
  
  if (side == LEFT_WHEEL) {
      if (overflowcountL > 5)
        return 1;
      else
        return 0;
  }      
}

/******************************************************************
 * This function teturns the TCNT ticks passed as the argument 
 * to vane frequency
 *
 ******************************************************************/
//  
unsigned long convertTCNT(unsigned int TCNTDifference) {
   if (TCNTDifference==0) 
      return(0);      
   return(ECLOCK/(TCNTDifference*PRESCALER)); 
}

unsigned long convertFrequency(unsigned int frequency, int side) {
                                                                                      // Carried over divide by 1000000
   MotorEncoderShaftSpeed = frequency*100 / NUM_ENCODER_VANES;               // x27 
   MotorOutputFrequency = MotorEncoderShaftSpeed / MOTOR_GEAR_RATIO;     // /225(overall, divided by 10)       (for Motor Gear Ratio)
   
   if (side == RIGHT_WHEEL)
        FinalSpeed = PI*WHEEL_DIAMETER_R*(MotorOutputFrequency);                // (overall, multiplied by 100)   (for PI) 
   else if (side == LEFT_WHEEL)
        FinalSpeed = PI*WHEEL_DIAMETER_L*(MotorOutputFrequency);                                                                                   // ---------------------------
   return(FinalSpeed/1000); //divide by 10000                                        // (overall, divide by 10)        (to restore final value)

}

unsigned int getCount(int side) {   
  if (side == RIGHT_WHEEL)
      return(periodticksR);
  
  else if (side == LEFT_WHEEL)
      return(periodticksL);
}


/*****************************************************************
*/

interrupt 8 void RightWheel( void ) { 
   R_Count++;                  // Increment right rising edge count variable 
   
   (void)TC0;                  // Pay the pizza guy
   
   if (R_Count == 1) {
    overflowcountR = 0;
    EdgeR_1 = TCNT;
   }
   else if (R_Count == 2) {
    R_Count = 0;
    EdgeR_2 = TCNT;
    
     if (overflowcountR) {        //TCNT Overflowed
        periodticksR = (TCNTMAX*overflowcountR)-EdgeR_1+EdgeR_2;             
     } else
        periodticksR = (EdgeR_2 - EdgeR_1);
    
   }
        
}

interrupt 9 void LeftWheel( void ) {      
   L_Count++;                  // Increment left rising edge count variable
   
   (void)TC1;                  // Pay the pizza guy
   
   if (L_Count == 1) {
    overflowcountL = 0;
    EdgeL_1 = TCNT;
   }
   else if (L_Count == 2) {
    L_Count = 0;
    EdgeL_2 = TCNT;
    
    if (overflowcountL) {        //TCNT Overflowed
        periodticksL = (TCNTMAX*overflowcountL)-EdgeL_1+EdgeL_2;      
     } else
        periodticksL = (EdgeL_2 - EdgeL_1);
     
   }    
  
}

interrupt 16 void timerOverFlow(void) {
  (void)TCNT;
  overflowcountR++;
  overflowcountL++;
}


