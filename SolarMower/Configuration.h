/* SolarMower Arduino */
/* configuration file */

#define ESC_MAX_SIGNAL 2000
#define ESC_MIN_SIGNAL 1000
#define VBat_Level_Min  9.0
#define VBat_Level_Max 13.8
#define ICut_Min 0.4      // not used on version 1.0
#define ICut_Max 2.0      // defines the maximum permissible current to the cut motor
#define accelerateTime 2  //total time accelerate = accelerationtime*255 [msec]
#define USdistance 30     //obstacle distance for ultrasonic sensor
#define timeReverse 3000  // duration reverse motor when obstacle detect
#define timeRotate  2000  // duration rotate motor when obstacle detect
#define PWMSpeed    255   // PWM value for thrust motor
#define timeClock   500   // time reading sensor
int PUP=1;
int PDW=2;
int PEN=3;

//Analog Pin
#define Button_pin     0  // Push button pin
#define ICut_pin       1  // Motor cut current
#define IPanel_pin     2  // Panel current
#define VBat_pin       3  // Battery voltage
#define SDA_pin        4  // SDA for I2C
#define SCL_pin        5  // SCL for I2C

//Digital pin
#define RXD_pin        0  // Arduino serial hardware RX
#define TXD_pin        1  // Arduino serial hardware TX
#define Encoder_Pin    2  // Wheels encoder
#define PWMA_pin       3  // PWM for motor A
#define DIRA_pin       4  // motor DIRA
#define BWFR_pin       5  // BWFR signal
#define BWFL_pin       6  // BWFL signal
#define Panel_pin      7  // Panel on/off
#define ESC_pin        8  // ESC for motor cut
#define SWOL_pin       9  // switch for right obstacle
#define SWOR_pin      10  // switch for lift obstacle
#define PWMB_pin      11  // PWM for motor B
#define DIRB_pin      12  // motor DIRB
#define LCD_pin       13  // LCD data signal

#define US_SX_address 0x70 //ultrasonic sensor SX address
#define US_DX_address 0x79 //ultrasonic sensor DX address
