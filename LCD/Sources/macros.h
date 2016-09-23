#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include <stdio.h>





#define LOW(value)  ((value) & 0xFF)
#define SET_BITS( port, bits )   ((port) |= (bits))         //setting bits in a register
#define CLR_BITS( port, bits )   ((port) &= LOW(~(bits)))   //clearing bits in a reg
#define FLIP_BITS( port, bits )   ((port) ^= (bits))        //toggling bits in a reg

extern int servoFlg;

//Write a macro to force a subset of the bits in any 
//specified register to a specific value that may include both 1’s and 0’s

#define FORCE_BITS( port, mask, value ) ((port) = ((port) & LOW(~(mask))) | ((value) & (mask)))



//enable a PWM channel
#define ENABLE_PWM_CHNL( chnl )	SET_BITS( PWME, 1 << (chnl) )

//disable a PWM channel
#define DISABLE_PWM_CHNL( chnl ) CLR_BITS( PWME, 1 << (chnl) )


//To make a timer channel OC, output compare

#define MAKE_CHNL_OC( chnl )	SET_BITS( TIOS, 1 << (chnl) )


//To make a timer channel IC, input compare

#define MAKE_CHNL_IC( chnl )		CLR_BITS( TIOS, 1 << (chnl) )


#define CLR_TIMER_CHNL_FLAG( chnl )	TFLG1 = (1 << (chnl))






// Name definition to access the TCTL1 & TCTL2 as a single 16-bit word
#define TCTL_1_2 (*(volatile word * const)&TCTL1)

// Valid action values for the above macro
#define OC_OFF                0x00
#define OC_TOGGLE             0x01
#define OC_GO_LO              0x02
#define OC_GO_HI              0x03

// Macro to force subset of word to specified value
#define FORCE_WORD( port, mask, value)    			\
	(port) = ((port) & ((~(mask)) & 0xFFFF) | ((value) & (mask)))


// Macro to set output compare event action for a specified channel
#define SET_OC_ACTION( chnl, action ) FORCE_WORD( TCTL_1_2,	\
			(0x03 << ((chnl) * 2)), 		\
			((action) << ((chnl) * 2)) )
			
#define FORCE_OC_ACTION_NOW( chnl, action )    	\
 SET_OC_ACTION( (chnl), (action) );	 	\
	 SET_BITS( CFORC, 1 << (chnl)) 
