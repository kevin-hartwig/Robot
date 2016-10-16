#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "macros.h"
#define DC_MOTOR_DDR DDRB
#define DC_MOTOR_PORT PORTB

/*****************************************************************
*
* HARDWARE PORT SETUP
*
*****************************************************************/

void DCinit(void);


void DCinit(void) {
 int temp = 0;

DC_MOTOR_DDR =  0b00001111;




PWMCTL = 0;        // 8-bit mode
PWMPRCLK = 0;      // count E-clocks directly – don’t prescale
PWMSCLA = 1;
PWMCLK = 3;        // Set scaling factor to divide by 2 (channel 0 and 1)




//CHANNEL 4
CLR_BITS(PWMCLK, PWMCLK_PCLK4_MASK);        // select clock A as the clock source for PWM4
CLR_BITS(PWMPOL, PWMPOL_PPOL4_MASK);        // channel 4 output low at the start of the period
SET_BITS(PWMCAE, PWMCAE_CAE4_MASK);         // select center-aligned mode
PWMPER4 = 255;                              // set period value
PWMDTY4 = 127;                              // set duty value
PWMCNT4 = 0;                                // reset the PWM4 counter


//CHANNEL 5
CLR_BITS(PWMCLK, PWMCLK_PCLK5_MASK );       // select clock A as the clock source for PWM4
CLR_BITS(PWMPOL, PWMPOL_PPOL5_MASK);        // channel 5 output low at the start of the period
SET_BITS(PWMCAE, PWMCAE_CAE5_MASK);         // select center-aligned mode
PWMPER5 = 255;                              // set period value
PWMDTY5 = 127;                              // set duty value
PWMCNT5 = 0;                                // reset the PWM4 counter


SET_BITS(PWME, 0b00110000); 

//DC_MOTOR_PORT = 0b00001010;

}

void stopDCMotor(void){

  DC_MOTOR_PORT = 0b00001111;
}


