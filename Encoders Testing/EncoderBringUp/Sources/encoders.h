


int getDistance(int wheel);
unsigned long convertFrequency(unsigned int frequency, int side);
unsigned long convertTCNT(unsigned int TCNTDifference);
//unsigned long getTCNTDifference(int side);
int getCounter(void);
unsigned int getCount(int side);
int checkOverflows(int side);

#define RIGHT_WHEEL (0)        // Right wheel on timer channel 0
#define LEFT_WHEEL  (1)        // Left wheel on timer channel 1

#define WHEEL_DIAMETER_R (56)  // Wheel diameter in mm X10
#define WHEEL_DIAMETER_L (57)  // Wheel diameter in mm X10
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




