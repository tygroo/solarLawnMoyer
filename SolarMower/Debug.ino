/*
  SolarMower Arduino serialDebug
 */

void serialDebug()
{
  Serial.print("VBat= ");
  Serial.print(VBat);
  Serial.println(" V");

  Serial.print("IPanel= ");  
  Serial.print(IPanel);
  Serial.println(" A");
  
  Serial.print("ICut= ");
  Serial.print(ICut); 
  Serial.println(" A");
  
  Serial.print("BWFR count= ");
  Serial.println(BWFR_count);
  Serial.print("BWFL count= ");  
  Serial.println(BWFL_count);  
  Serial.print("US_DX=");
  Serial.print(US_DX.read());
  Serial.print("   US_SX=");
  Serial.println(US_SX.read());  
  
  Serial.println();
}  

void LCDdebug()
{
      lcd.clear(); //clearLCD();
      lcd.setCursor(0,0); //lcdPosition(0,0);
    switch (mowerStatus)
    {
    case 0:
      lcd.print("CHARGE ");
      break;
    case 1:
      lcd.print("RUN    ");   
      break;
    case 2:
      lcd.print("STUCK  ");
      break;
    case 3:
      lcd.print("SEARCH ");  
      break;
    case 4:
      lcd.print("BATLOW ");  
      break;
    case 5:
      lcd.print("CHR RS ");
      break;
    case 6:
      lcd.print("CUT ER ");
      break;  
    }  
    
    lcd.print(VBatPC);
    lcd.print("%  ");
     
    lcd.setCursor(0,1);
    lcd.print("IC=");
    lcd.print(ICut);  
  
    lcd.setCursor(9,1);
    lcd.print("IP=");
    lcd.print(IPanel);     
} 
