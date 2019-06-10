





/*
  SolarMower V1.0
  SolarMower use mowershield_FE
  2014 Mirco Segatello
  For hardware see www.elettronicain.it
  
  Press PEN during power-on for ESC test  
  Press PUP during power-on for test motor
  Press PDW during power-on for test sensor
 */

#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <Servo.h> 
#include <LiquidCrystal_I2C.h>
#include "Wire.h"
#include "SRF02.h"
#include "Configuration.h"



Servo ESC;

//variables
float VBat;               // Battery voltage [V]
int VBatPC;               // Battery voltage percentage
float VBatScale=0.054;    // Battery scale for Volt converter
float IPanel;             // Panel current [A]
float IPanelOffset;       // Panel current offset
float IPanelScale=0.185;  // Panel current scale V/A
float ICut;               // Motor cut current [A]
float ICutOffset;         // Motor cut current offset
float ICutScale=0.185;    // Motor cut current scale V/A
int i;
int cutPower=0;           // Cut Power from 0%=ESC_MIN_SIGNAL to 100%=ESC_MAX_SIGNAL
int cutPower_uSec=ESC_MIN_SIGNAL;      // Cut Power from ESC_MIN_SIGNAL to ESC_MAX_SIGNAL
int oldSpeed=0;           // Speed of mower (0-255)
float IPanelMax=0;           // IPanel max current
unsigned long BWFL_count;
unsigned long BWFR_count;
unsigned long previousMillis = 0;
unsigned long currentMillis; 
volatile int mowerStatus=0; // 0=oncharge (press pen for run)
                            // 1=run
                            // 2=stuck
                            // 3=search
                            // 4=batlow
                            // 5=charge and restart when full
                            // 6=cuterror
volatile unsigned long wheelTime=0;
volatile unsigned long handleTime=0;


// LCD does not send data back to the Arduino, only define the txPin
//SoftwareSerial LCD = SoftwareSerial(0, LCD_pin);
int LCD_Page = 0;         // Page to display

LiquidCrystal_I2C lcd(0x27, 16, 2);

SRF02 US_SX(US_SX_address, SRF02_CENTIMETERS);
SRF02 US_DX(US_DX_address, SRF02_CENTIMETERS);




void setup() 
{
  Serial.begin(9600);
  Wire.begin();
  pinMode(Encoder_Pin, INPUT_PULLUP);
  pinMode(PWMA_pin, OUTPUT);
  pinMode(DIRA_pin, OUTPUT);  
  pinMode(BWFR_pin, INPUT);
  pinMode(BWFL_pin, INPUT);    
  pinMode(Panel_pin, OUTPUT);  
  pinMode(ESC_pin, OUTPUT);  
  pinMode(SWOR_pin, INPUT_PULLUP);
  pinMode(SWOL_pin, INPUT_PULLUP);       
  pinMode(PWMB_pin, OUTPUT);
  pinMode(DIRB_pin, OUTPUT);   
  pinMode(LCD_pin, OUTPUT);  

  digitalWrite(DIRA_pin, LOW); 
  digitalWrite(DIRB_pin, LOW);   
  analogWrite(PWMA_pin, 0);
  analogWrite(PWMB_pin, 0);  
  digitalWrite(Panel_pin, LOW);   

 
  // For SerialMonitor diagnostic 
  Serial.println("Solar Mower V1.0\n"); 
  Serial.println("Init ESC..."); 
  ESC.attach(ESC_pin);
  cutOFF(); 
  
  Serial.println("Init i2c_LCD...");       
  lcd.init(); //serLCDInit();  
  lcd.backlight();
  // Serial.println("Init i2c_LCD finished...");  
  lcd.clear(); //clearLCD();
//  Serial.println("Init i2c_LCD finished...");  
  lcd.setCursor(0,0); //lcdPosition(0,0);
  lcd.print("Solar Mower v1.0");
  lcd.setCursor(0,1); //lcdPosition(1,0);
  lcd.print("Init...");      
  

  Serial.println("Init Sensor");  
  sensorInit();  
  Serial.print("IPanelOffset= ");
  Serial.println(IPanelOffset);
  Serial.print("ICutOffset= ");
  Serial.println(ICutOffset);  
  Serial.println(); 


  // TEST MOTOR
  if (Button(Button_pin)==PUP) 
  {
    while(1)
    {
      lcd.clear(); //clearLCD();
      
      lcd.setCursor(0,0); //lcdPosition(0,0);
      lcd.print("Test motor");
      lcd.setCursor(0,1); //lcdPosition(1,0);  
      lcd.print("PEN begin test  ");  
      Serial.println("Press PEN for begin motor test"); 
      while (Button(Button_pin)!=3) { }
      testMotor();
    }   
  }   
  
  //TEST SENSOR
  if (Button(Button_pin)==PDW) 
  { 
      lcd.clear(); //clearLCD();
      lcd.setCursor(0,0); //lcdPosition(0,0);
      lcd.print("Test sensor");    
      Serial.println("Test sensor");
      delay(1000);
    while(1)
    {
      sensorReading();
      
      lcd.clear(); //clearLCD();
       lcd.backlight();
      lcd.setCursor(0,0); //lcdPosition(0,0);
      lcd.print("VB=");
      lcd.print(VBat);
      
      lcd.setCursor(9,0); 
      if ((US_DX.read()>0) && (US_DX.read()<USdistance))   {
            lcd.print("UsDX ");
            Serial.println("UsDX");
      }      
      if ((US_SX.read()>0) && (US_SX.read()<USdistance)) {
            lcd.print("UsSX ");   
            Serial.println("UsSX");            
      }      
      if ( (BWFL_count>3000) && (BWFL_count<4000) ) {
            lcd.print("BWFL");
            Serial.println("BWFL");
      }      
      if ( (BWFR_count>2000) && (BWFR_count<4000) ) {
            lcd.print("BWFR");    
            Serial.println("BWFR");            
      }           
      if (digitalRead(Encoder_Pin)==0) {
            lcd.print("Encoder");
            Serial.println("Encoder");            
      }           
      if (digitalRead(SWOR_pin)==0) {
            lcd.print("SWOR");
            Serial.println("SWOR");            
      }      
      if (digitalRead(SWOL_pin)==0) {
            lcd.print("SWOL");       
            Serial.println("SWOL");            
      }      
      if (Button(Button_pin)==1) {
            lcd.print("PUP");
            Serial.println("PUP");            
      }      
      if (Button(Button_pin)==2) {
            lcd.print("PDW");  
            Serial.println("PDW");            
      }            
      if (Button(Button_pin)==3) {
            lcd.print("PEN");
            Serial.println("PEN");            
      }      
      
      lcd.setCursor(0,1);
      lcd.print("IC=");
      lcd.print(ICut);  
      Serial.print("IC=");     
      Serial.println(ICut);       
      lcd.setCursor(9,1);
      lcd.print("IP=");
      lcd.print(IPanel);  
      Serial.print("IP=");     
      Serial.println(IPanel);        

      delay(250);  
    }
  }    
  
  //TEST ESC
  if (Button(Button_pin)==PEN)   
  {
    Serial.println("Test ESC...");   
    lcd.clear();
     lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print("Test ESC");
    while(1)
    {
        sensorReading();
        cutPower = map(cutPower_uSec, ESC_MIN_SIGNAL, ESC_MAX_SIGNAL, 0, 100);
        lcd.setCursor(9,0);
        lcd.print("IC=");
        lcd.print(ICut);          
        lcd.setCursor(0,1);  
        lcd.print("cutPower="); 
        lcd.print(cutPower);
        lcd.print("%  ");
        Serial.print("cutPower=");
        Serial.print(cutPower);
        Serial.print("%   usec=");
        Serial.println(cutPower_uSec);
        
        if (Button(Button_pin)==2) 
           if (cutPower_uSec<ESC_MAX_SIGNAL) cutPower_uSec += 20;
        if (Button(Button_pin)==1) 
           if (cutPower_uSec>ESC_MIN_SIGNAL) cutPower_uSec -= 20;      
           
        ESC.writeMicroseconds(cutPower_uSec);
        delay(200);   
    } 
  } 
  attachInterrupt(0, rotate, FALLING);
}


void loop() 
{
  //main program
  digitalWrite(Panel_pin, HIGH);  

  main: 
  //wait for PEN press
  while(Button(Button_pin)!=3) 
  { 
    lcd.backlight();
    handleTime=millis();
    currentMillis = millis();
    if(currentMillis - previousMillis > timeClock) 
    {    
        sensorReading(); 
        LCDdebug();
    }  
  }  
  
  Serial.println("GO!");      
  mowerStatus=1;
  IPanelMax=0;
  LCDdebug();  
  cutON();
  setMowerSpeed(255);


  //main loop    
  while(1)
  { 
    
    //ferma tutto
    if (Button(Button_pin)==PUP || Button(Button_pin)==PDW)
    {
       lcd.backlight();
       handleTime=millis();
      cutOFF();  
      setMowerSpeed(0);
      mowerStatus=0;
     
      goto main; 
    }

    //if not an interrupt wheels occurs within 10 seconds, the robot is blocked
    if (millis()>wheelTime+10000)
    {
      cutOFF();  
      setMowerSpeed(0);
      mowerStatus=2;
      goto main;       
    }

   if (millis()>handleTime+5000)
    {
      lcd.noBacklight();  
    }
    
    
    // gestione sensori ostacolo
    if (digitalRead(SWOL_pin)==LOW) obstacleAvoidSX();   
    if (digitalRead(SWOR_pin)==LOW) obstacleAvoidDX();       
       
    
    // reads the sensors every timeClock
    currentMillis = millis();
    if(currentMillis - previousMillis > timeClock) 
    {
      previousMillis = currentMillis; 

      sensorReading(); 
      
      if (digitalRead(Encoder_Pin)==0) {
            resetEncoder();           
      }      
        
      if ((US_DX.read()>0) && (US_DX.read()<USdistance)) 
      {
            obstacleAvoidSX();  
            resetEncoder();
      }      
      if ((US_SX.read()>0) && (US_SX.read()<USdistance)) 
      {
            obstacleAvoidDX();      
            resetEncoder();
      }      
      if ((BWFL_count>3000) && (BWFL_count<4000))
      {
            obstacleAvoidSX();  
            resetEncoder();
      }      
      if ((BWFR_count>2000) && (BWFR_count<4000)) 
      {       
            obstacleAvoidDX();     
            resetEncoder();
      }      
      
      
      //**********************************************************************
      if (IPanel>IPanelMax) IPanelMax=IPanel; //memory to max current on panel
      
      //battery lower then 10% then searching the point to charge
      if (VBatPC<10) 
      {                
        mowerStatus=3;
        cutOFF();  //stop cut
      }  
      
      //continues to advance until it finds a light source at least 80% of the previous maximum.
      if (mowerStatus==3)
      {     
        if  (IPanel>IPanelMax*0.8) 
        {
          setMowerSpeed(0);
          mowerStatus=5; //charge with restart
        }    
      }
      //**********************************************************************
      
      if (mowerStatus==5)
      {
        resetEncoder();
        // full charge then restart
        if (VBatPC>=100)
        {
            mowerStatus=1;
            IPanelMax=0;
            cutON();
            setMowerSpeed(255);         
        }    
      }  
        
      if (VBat > VBat_Level_Max)
      {
         //lead battery never stop charge 
         //disable charge only for lipo battery
         //digitalWrite(Panel_pin, LOW); 
      } 
      
      //battery is completely discharged      
      if (VBat < VBat_Level_Min) 
      {
        cutOFF();  
        setMowerSpeed(0);
        digitalWrite(Panel_pin, HIGH); 
        mowerStatus=4;
        goto main;       
      } 
      
      // ICut too high 
      if (ICut > ICut_Max) 
      {
        cutOFF();  
        setMowerSpeed(0);
        digitalWrite(Panel_pin, HIGH); 
        mowerStatus=6;
        goto main;       
      }       
      
      serialDebug();      
      LCDdebug();    
      }
  }  
}

void rotate()
{
  wheelTime=millis();   
}

void resetEncoder()  //reset value for block wheels
{
  wheelTime=millis();   
}

void obstacleAvoidSX()
{
      // obstacle avoid
      setMowerSpeed(-PWMSpeed);
      delay(timeReverse);
      setMowerRotate(-PWMSpeed); //gira a sinistra
      delay(timeRotate);
      setMowerSpeed(PWMSpeed);
}

void obstacleAvoidDX()
{
      // obstacle avoid     
      setMowerSpeed(-PWMSpeed);
      delay(timeReverse);
      setMowerRotate(PWMSpeed);
      delay(timeRotate);
      setMowerSpeed(PWMSpeed);   
}


void setMowerRotate(int newSpeed)
{ 
  //rotate mower (first set speed at zero) 
  constrain(newSpeed, -255, 255);
    for (i=oldSpeed; i>=0; i--)
    {
       analogWrite(PWMA_pin, i);
       analogWrite(PWMB_pin, i);
       delay(accelerateTime);
    }      
    oldSpeed=0;
    
  if (newSpeed<0) 
  {  
    digitalWrite(DIRA_pin, HIGH); 
    digitalWrite(DIRB_pin, LOW);   
    newSpeed=-newSpeed;    
  }
  else
  {
    digitalWrite(DIRA_pin, LOW); 
    digitalWrite(DIRB_pin, HIGH);    
  }  
  
  for (i=0; i<=newSpeed; i++)
  {
       analogWrite(PWMA_pin, i);
       analogWrite(PWMB_pin, i);
       delay(accelerateTime);
  }  
  oldSpeed=newSpeed;
}


void setMowerSpeed(int newSpeed)
{
  //set new speed for mower (first set speed at zero) 
  constrain(newSpeed, -255, 255);
    for (i=oldSpeed; i>=0; i--)
    {
       analogWrite(PWMA_pin, i);
       analogWrite(PWMB_pin, i);
       delay(accelerateTime);
    }    
    oldSpeed=0; 
  
  if (newSpeed<0)
  {  
    digitalWrite(DIRA_pin, HIGH); 
    digitalWrite(DIRB_pin, HIGH);    
    newSpeed=-newSpeed;
  }
  else
  {
    digitalWrite(DIRA_pin, LOW); 
    digitalWrite(DIRB_pin, LOW);    
  }
  
  if (newSpeed > oldSpeed)
  {
    for (i=oldSpeed; i<=newSpeed; i++)
    {
       analogWrite(PWMA_pin, i);
       analogWrite(PWMB_pin, i);
       delay(accelerateTime);
    }   
  }
  else
  {
    for (i=oldSpeed; i>=newSpeed; i--)
    {
       analogWrite(PWMA_pin, i);
       analogWrite(PWMB_pin, i);
       delay(accelerateTime);
    }        
  } 
  oldSpeed=newSpeed; 
}

 
void cutON()
{
  //cutter ON
  for (cutPower_uSec=ESC_MIN_SIGNAL; cutPower_uSec<=ESC_MAX_SIGNAL; cutPower_uSec += 10)
  {
    ESC.writeMicroseconds(cutPower_uSec);
    delay(50); 
  }
  wheelTime=millis(); 
}

void cutOFF()
{
  //cutter OFF
  ESC.writeMicroseconds(ESC_MIN_SIGNAL);      
}  

 


