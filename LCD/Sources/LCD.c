/*****************************************************************
*
* Title: LCD Display 
* Authors: James and Ovi
* Purpose: To print characters and strings to the LCD display..
* Date: March 8, 2016
*
*****************************************************************/


//INLCUDES
#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include <stdio.h>


/*****************************************************************
*
* GENERIC MACROS
*
*****************************************************************/
#define LOW(value) ((value) & 0xFF)
#define SET_BITS(PORT, MASK) ((PORT) |= (MASK))
#define CLR_BITS(port, bits) ((port) &= LOW(~(bits)))
#define FORCE_BITS(PORT, MASK, VALUE) (PORT) = (((PORT) & LOW(~MASK)) | ((VALUE) & MASK))


/*****************************************************************
*
* HARDWARE PORT SETUP
*
*****************************************************************/
#define LCD_PORT PORTA 
#define LCD_PORT_DDR DDRA


/*****************************************************************
*
* LCD DEFINES/MACROS
*
*****************************************************************/
#define LCD_E_BIT PORTA_BIT4_MASK
#define LCD_RS_BIT PORTA_BIT5_MASK
#define LCD_RW_BIT PORTA_BIT6_MASK
#define LCD_BUS_BITS 0x0F
#define LO_NYBBLE(value) ((value) & 0x0F)
#define HI_NYBBLE(value) (((value)>>4) & 0x0F)
#define LCD_E_LO CLR_BITS(LCD_PORT, LCD_E_BIT)
#define LCD_E_HI SET_BITS(LCD_PORT, LCD_E_BIT)
#define LCD_RS_DR SET_BITS(LCD_PORT, LCD_RS_BIT)
#define LCD_RS_IR CLR_BITS(LCD_PORT, LCD_RS_BIT)
#define LCD_RW_W (CLR_BITS(LCD_PORT, LCD_RW_BIT))
#define LCD_BUS(VALUE) FORCE_BITS(LCD_PORT, LCD_BUS_BITS,(VALUE))



/*****************************************************************
*
* LCD SETUP CONSTANT VARIABLES
*
*****************************************************************/
#define LCD_CMD_FUNCTION     0b00100000  //enable ?
#define LCD_FUNCTION_4BIT    0b00000000   //4 -bit mode
#define LCD_FUNCTION_2LINES  0b00001000   //2 lines
#define LCD_FUNCTION_5X8FONT 0b00000000   //5 x 7 dots

#define LCD_CMD_DISPLAY      0b00001000    //enables the display
#define LCD_DISPLAY_OFF      0b00000000    //turns off display

#define LCD_DISPLAY_CLR      0b00000001    //clears screen
#define LCD_CURSOR_HOME      0b00000010    //reset cursor position to top left

#define LCD_CMD_ENTRY         0b00000100   //entry enable
#define LCD_ENTRY_MOVE_CURSOR 0b00000010   //move cursor to the right
#define LCD_ENTRY_INC         0b00000000   //do not shift the entire display


#define LCD_DISPLAY_ON       0b00000100    //turns on the display
#define LCD_DISPLAY_NOCURSOR 0b00000010    //cursor off
#define LCD_DISPLAY_NOBLINK  0b00000001    //cursor blinks




/*****************************************************************
*
* TIMER CHANNEL SETUP VARIABLES
*
*****************************************************************/
// Valid action values for the above macro
#define OC_OFF                0x00
#define OC_TOGGLE             0x01
#define OC_GO_LO              0x02
#define OC_GO_HI              0x03






/*****************************************************************
*
* TIMER CHANNEL MACROS
*
*****************************************************************/
//To make a timer channel OC, output compare
#define MAKE_CHNL_OC( chnl ) SET_BITS( TIOS, 1 << (chnl) )

//To make a timer channel IC, input compare
//#define MAKE_CHNL_IC( chnl ) CLR_BITS( TIOS, 1 << (chnl) )

#define CLR_TIMER_CHNL_FLAG( chnl )	TFLG1 = (1 << (chnl))

// Name definition to access the TCTL1 & TCTL2 as a single 16-bit word
#define TCTL_1_2 (*(volatile word * const)&TCTL1)
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





/*****************************************************************
*
* FUNCTION DECLARATIONS
*
*****************************************************************/
void LCDputs(unsigned char * buf);
void LCDputc(unsigned char data);
void LCDdata(unsigned char data); 
void LCDinit(void);
void msDelay( int k );
void LCDcmd(unsigned char data);
void LCDprintf(char *fmt, ...);
void LCDclr(void);
void LCDprintCMD(char * buf);
void LCDclrTOP(void);
void LCDclrBOT(void);

/*****************************************************************
* SETUPTIMER()
*
* inputs: N/A
* outputs: N/A 
*
*****************************************************************/
void setupTimer(void){
  
  
//TSCR1 = 0xB0; 	/* enable TCNT, fast timer flag clear, freeze */
//TSCR2 = 0x03; 	//Sets prescaler to 8,...../*0x06 set prescaler to 64, no interrupt */

//make channel 0 output compare  
MAKE_CHNL_OC(0);
}
void LCDclrBOT(void){

LCDcmd(0b11000000); 
LCDprintf("                "); //16 spaces  
  
  
}
void LCDclrTOP(void){

LCDcmd(LCD_CURSOR_HOME); 
LCDprintf("                "); //16 spaces  
  
  
}

/*****************************************************************
* LCDPUTC()
*
* inputs: unsigned char
* outputs: prints character to screen 
*
*****************************************************************/

void LCDputc(unsigned char data) {
  LCDdata(data);
}

/*****************************************************************
* LCDPRINTF()
*
* inputs: string
* outputs: prints string to screen 
*
*****************************************************************/
void LCDprintf(char *fmt, ...) {
 
  va_list myArgs;
  char buffer[32];
  va_start(myArgs, fmt);
  vsprintf(buffer,fmt,myArgs);
  LCDputs(buffer);
  va_end(myArgs);
}

/*****************************************************************
* LCDPUTs()
*
* inputs: string
* outputs: prints string to screen 
*
*****************************************************************/
void LCDputs(unsigned char * buf) {
   
   unsigned char count = 0;
   
   while(*buf){
     count++;
     if(*buf == '\n') {
       LCDcmd(0b11000000);     //move to line 2
     buf++;
      } 
      
      else if(count == 17) {
       LCDcmd(0b11000000);     //move to line 2
      }
           
      else {
        
      LCDputc(*buf++);
      }
   }
}
/*****************************************************************
* LCDprintfCMD()
*
* inputs: string
* outputs: prints last command to screen (2nd row)
*
*****************************************************************/
void LCDprintCMD(char *buf)
{
   LCDcmd(0b11000000);
         
   
   LCDprintf(buf);
}
  

/*****************************************************************
* LCDdata()
* Purpose: Sends to DR register of LCD 
* inputs: unsigned char
* outputs: N/A 
*
*****************************************************************/
void LCDdata(unsigned char data) {
  
  LCD_E_LO;
  LCD_RS_DR;
  LCD_RW_W;
  LCD_E_HI;
  LCD_BUS(HI_NYBBLE(data));
  LCD_E_LO;
  LCD_E_HI;
  LCD_BUS(LO_NYBBLE(data));
  LCD_E_LO;
  //msDelay(3); 
  
}

/*****************************************************************
* LCDcmd()
* Purpose: Sends to IR register of LCD 
* inputs: unsigned char
* outputs: N/A 
*
*****************************************************************/
void LCDcmd(unsigned char data) {
  
  LCD_E_LO;
  LCD_RS_IR;
  LCD_RW_W;
  LCD_E_HI;
  LCD_BUS(HI_NYBBLE(data));
  LCD_E_LO;
  LCD_E_HI;
  LCD_BUS(LO_NYBBLE(data));
  LCD_E_LO;
  //msDelay(3); 
  
}


/*****************************************************************
* LCDinit()
* Purpose: Initial Setup to enable LCD display 
* inputs: unsigned char
* outputs: N/A 
*
*****************************************************************/
void LCDinit(void){

  LCD_PORT_DDR = 0x7F;
  TIOS   |= TIOS_IOS0_MASK;	/* ready to go - enable TC0 as OC */

  LCD_E_HI;
  LCD_BUS(0x03); //wake up display & sync
  LCD_E_LO;

  msDelay(5);
  
  LCD_E_HI;
  LCD_BUS(0x03); //wake up display & sync
  LCD_E_LO;

  msDelay(1);
  
  LCD_E_HI;
  LCD_BUS (0x03);
  LCD_E_LO;
  
  LCD_E_HI;
  LCD_BUS(0x02);
  LCD_E_LO;
  
  msDelay(3);
  
 // now that we're sync'd and in 4-bit mode, issue commands to configure the display
  LCDcmd( LCD_CMD_FUNCTION | LCD_FUNCTION_4BIT | LCD_FUNCTION_2LINES | LCD_FUNCTION_5X8FONT );
 
  LCDcmd( LCD_CMD_DISPLAY | LCD_DISPLAY_OFF );
   
  //this is LCDclear function ()
  LCDclr();
  //LCD_CURSOR_HOME );
  
  LCDcmd( LCD_CMD_ENTRY | LCD_ENTRY_MOVE_CURSOR | LCD_ENTRY_INC );
  
  LCDcmd( LCD_CMD_DISPLAY | LCD_DISPLAY_ON | LCD_DISPLAY_NOCURSOR | LCD_DISPLAY_NOBLINK );

}

/*****************************************************************
* LCDclr()
* Purpose: Clears LCD screen 
* inputs: N/A
* outputs: N/A 
*
*****************************************************************/
void LCDclr(void) 
{
  LCDcmd(LCD_DISPLAY_CLR);  
}
  

/*****************************************************************
* msDelay()
* Purpose: Delays for input number of mili seconds 
* inputs: integer (ms delay)
* outputs: N/A 
*
*****************************************************************/
void msDelay( int k )
{
   	int ix;			/* counter for number of ms delayed */

 	TC0 	= TCNT + 125;		/* preset TC0 for first OC event */

	for (ix = 0; ix < k*8; ix++) {	/* for number of ms to delay… */
		while(!(TFLG1 & TFLG1_C0F_MASK));	/* wait for OC event */
          	TC0 += 125;		/* rearm the OC register, this clears TFLG1 */
   	}

   
} /* end of msDelay() */
