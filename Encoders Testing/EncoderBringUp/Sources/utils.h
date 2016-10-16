int strcmp(char *s1, char *s2);

#define commandLength 6

#define DC_MOTOR_PORT PORTB 
#define DC_MOTOR_DDR DDRB


 //Returns 0 if strings are the same, else 1
int strcmp(char *s1, char *s2) {
  int i;
  for (i = 0 ; i < commandLength ; i++) {    // Loop until a null is reached
     if (*s1 != *s2) {
         return(1);
     } else {
         s1++; s2++;
     }         
  }
  
  return(0);
  
}