/*
  Solar Mower Arduino Sensor reading
 */

void sensorInit()
{
   IPanelOffset=0;
   ICutOffset=0;
   for (i=0; i<50; i++)
   {                                    
      IPanelOffset += analogRead(IPanel_pin);
      delay(20);
      ICutOffset += analogRead(ICut_pin);      
      delay(20);
   }
   IPanelOffset /= i; 
   ICutOffset /= i;
}

void sensorReading()
{
  VBat=VBatScale*float(analogRead(VBat_pin));
  VBatPC = 100.0*(VBat-VBat_Level_Min)/(VBat_Level_Max-VBat_Level_Min);
  VBatPC = constrain(VBatPC, 0, 100);
  
  IPanel=IPanelOffset-float(analogRead(IPanel_pin));
  IPanel=(IPanel*5.0/1024.0)/IPanelScale;
  IPanel=constrain(IPanel, 0, 2.0);
 
  ICut=float(analogRead(ICut_pin))-ICutOffset;
  ICut=(ICut*5.0/1024.0)/ICutScale;   
  ICut=constrain(ICut, 0, 5.0);
  

  BWFL_count = FreqCounter(BWFL_pin, 100);
  BWFR_count = FreqCounter(BWFR_pin, 100); 
 
  SRF02::update(); //read ultrasonic sensor
}  

int Button(int buttonPin)
{
  int pushValue = analogRead(buttonPin);
  if ( pushValue < 164 ) 
    return 1;
  else if ( pushValue < 502 )
   return 2;
  else if ( pushValue < 850 )
     return 3;
  else
     return 0;  //nessun pulsante premuto
}

unsigned long FreqCounter(int pin, unsigned long gateTime)
{ 
  unsigned long time, timeInitial;
  unsigned long count=0;
  boolean level, lastLevel;
  
  timeInitial = millis();
  time=timeInitial;
    
  while((time-timeInitial)<gateTime) 
  {    
    level = digitalRead(pin);
    if((level != lastLevel) ) // if transient
    { 
       lastLevel = level;
       if(level == HIGH) count++; //rising front
    }
  time=millis();
  }
  return count;
}
