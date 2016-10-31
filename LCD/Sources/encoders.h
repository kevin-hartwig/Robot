/******************************************************************
 * PID Struct
 *
 ******************************************************************/
typedef struct PID{
  int dState;            // Last position input
  int iState;            // Integrator state
  int iMax, iMin;        // Maximum and minimum allowable integrator state

  int iGain,             // integral gain
  pGain,                    // proportional gain
  dGain;                    // derivative gain
} SPid;


int getDistance(int wheel);
signed int convertFrequency(unsigned int frequency, int side);
unsigned long convertTCNT(unsigned int TCNTDifference);
int UpdatePID(struct SPid * pid, int error, int position);
//unsigned long getTCNTDifference(int side);
int getCounter(void);
unsigned int getCount(int side);
int checkOverflows(int side);

//interrupt 16 void timerOverFlow(void);

#define RIGHT_WHEEL (0)        // Right wheel on timer channel 0
#define LEFT_WHEEL  (1)        // Left wheel on timer channel 1

#define WHEEL_DIAMETER_R (56)  // Wheel diameter in mm X10
#define WHEEL_DIAMETER_L (56)  // Wheel diameter in mm X10
#define WHEEL_ROTATION (607)   // Number of encoder veins per wheel rotation
#define ENCODER_TO_MM  (166)   // Number of mm per one wheel rotation
#define VANE_DISTANCE  (0.2741)// Distance travelled per vane (mm)
#define NUM_ENCODER_VANES (27) // Number of encoder vanes
#define MOTOR_GEAR_RATIO (225) // Motor Gear Ratio * 10

#define PI 314                 // Pi * 100

#define ECLOCK (8000000)             // 8 MHz
#define PRESCALER (8)          // 
#define TCNTMAX (0xFFFF)

#define VANE_FREQUENCY



//Globals




