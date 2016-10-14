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
volatile static unsigned int overflowcountR, overflowcountL;
volatile static unsigned int EdgeL_1, EdgeL_2, EdgeR_1, EdgeR_2, L_Period, R_Period;
volatile static unsigned int lastOverFlow;

void encodersInit(void) {

  EdgeL_1 = 0;
  EdgeL_2 = 0;
  L_Period = 0;
  EdgeR_1 = 0;
  EdgeR_2 = 0;
  R_Period = 0;
  
  lastOverFlow = 0;
  
  overflowcountR = 0;
  overflowcountL = 0;

  R_Count = 0; 
  L_Count = 0;
 
  
  MAKE_CHNL_IC(RIGHT_WHEEL);
  MAKE_CHNL_IC(LEFT_WHEEL);
  
  TCTL4 = 0b00001010;          // Hardware will detect only rising edges - Last 4 bits represent channel 1, 0 (respectively)              
  
  //REMOVE WHEN INTEGRATING
    TSCR1 = 0b10110000;        // starting Tcount , enabling freeze, and enabling fast clear 
    //TSCR2 = 0b00000011;        // setting prescaler to 8 (last 3 bits)  
    TSCR2 = 0x83;             
  
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

// Returns the TCNT ticks between two edges passed as arguments
// Arguments:
//    1. Edge1 --> First rising edge 
//    2. Edge2 --> Second rising edge 
//    3. overflows --> Overflowcount (PASS THIS GLOBAL VARIABLE - updated in timerOverFlow ISR)
/////////////////////

unsigned long getTCNTDifference(unsigned int Side) {
  unsigned int Edge2, Edge1, overflows;
  
 

  if (Side == RIGHT_WHEEL) {
      Edge2 = EdgeR_2;
      Edge1 = EdgeR_1;
      overflows = overflowcountR;
  } else if (Side == LEFT_WHEEL) {
      Edge2 = EdgeL_2;
      Edge1 = EdgeL_1;
      overflows = overflowcountL;
  }  
  
 //  if (overflows > 20) 
 //     return(0);
   
   lastOverFlow = overflows;
  
  if (overflows) {        //TCNT Overflowed
     if (Edge2 > Edge1)
        return(TCNTMAX*overflows - (Edge2 - Edge1));
     else 
        return(TCNTMAX*overflows - (Edge1 - Edge2));
     
  } else
     return(Edge2 - Edge1);
}

// Returns the TCNT ticks passed as the argument to vane frequency 
unsigned long convertTCNT(unsigned int TCNTDifference) {
   return(TCNTDifference*(ECLOCK/PRESCALER)); 
}

unsigned long convertFrequency(unsigned int frequency) {
                                                                                      // Carried over divide by 1000000
   unsigned long MotorEncoderShaftSpeed = frequency / NUM_ENCODER_VANES;               // 
   unsigned long MotorOutputFrequency = MotorEncoderShaftSpeed / MOTOR_GEAR_RATIO;     // (overall, divided by 10)       (for Motor Gear Ratio)
   unsigned long FinalSpeed = PI*WHEEL_DIAMETER*(MotorOutputFrequency);                // (overall, multiplied by 100)   (for PI) 
                                                                                      // ---------------------------
   return(FinalSpeed/100000);                                                         // (overall, divide by 10)        (to restore final value)

   //return(PI*WHEEL_DIAMETER*((frequency/27)/22.5)); 
}

/*****************************************************************
*/

interrupt 8 void RightWheel( void ) { 
   R_Count++;
   
   (void)TC0;                  // Pay the pizza guy
   
   if (R_Count == 1) {
    overflowcountR = 0;
    EdgeR_1 = TCNT;
   }
   else if (R_Count == 2) {
    R_Count = 0;
    EdgeR_2 = TCNT;
   }
        
}

interrupt 9 void LeftWheel( void ) {      
   L_Count++;                  // Increment Left Motor Vein Count Variable
   
   (void)TC1;                  // Pay the pizza guy
   
   if (L_Count == 1) {
    overflowcountL = 0;
    EdgeL_1 = TCNT;
   }
   else if (L_Count == 2) {
    L_Count = 0;
    EdgeL_2 = TCNT;
   }    
  
}

interrupt 16 void timerOverFlow(void) {
  (void)TCNT;
  overflowcountR++;
  overflowcountL++;
}


