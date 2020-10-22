
 /*!
  * file SetColor.ino
  * brief SetColor.
  *
  * Copyright   [DFRobot](https://www.dfrobot.com), 2016
  * Copyright   GNU Lesser General Public License
  *
  * version  V1.0
  * date  2017-2-10
  */

#include <Wire.h>
#include <DFRobot_LCD.h>

uint8_t r,g,b;
int t=0;
uint32_t sNeu=0;
uint32_t sOld=-1;
uint8_t stellen;
byte alarmSymbol[] = {
  B00100,
  B01110,
  B01110,
  B01110,
  B11111,
  B11111,
  B01010,
  B00100
};
byte speakerSymbol[] = {
  B00011,
  B01101,
  B11101,
  B11101,
  B11101,
  B11101,
  B01101,
  B00011
};

DFRobot_LCD lcd(16,2);  //16 characters and 2 lines of show

void setup() {
  // initialize
  lcd.init();
  lcd.clear();
  lcd.customSymbol(1,alarmSymbol);
  lcd.customSymbol(2,speakerSymbol);
  
  /*lcd.setCursor(0,0);
  lcd.print("    00:00:00    ");
  lcd.setCursor(0,1);
  lcd.print("                ");
  lcd.setRGB(127,127,127);//Set R,G,B Value*/
  lcd.setCursor(0,1);
  lcd.write(2);
  lcd.print("Mo 06:50");
}

void printTime(uint8_t x, uint8_t y, uint64_t seconds){
  char zeit[9];
  uint8_t h,m,s;
  s=seconds%60;
  m=(seconds/60)%60;
  h=(seconds/(60*60))%100;
  zeit[0] = (h/10)+0x30;
  zeit[1] = (h%10)+0x30;
  zeit[2] = ':';
  zeit[3] = (m/10)+0x30;
  zeit[4] = (m%10)+0x30;
  zeit[5] = ':';
  zeit[6] = (s/10)+0x30;
  zeit[7] = (s%10)+0x30;
  zeit[8] = 0;
  lcd.setCursor(x,y);
  lcd.print(zeit);
}





void loop() {
  sNeu=millis()/1000;
  if(sOld != sNeu){
    printTime(4,0,sNeu);
    sOld=sNeu;
  }
  r=(abs(sin(3.14*t/180)))*255;//get R,G,B value
  g=(abs(sin(3.14*(t+60)/180)))*255;
  b=(abs(sin(3.14*(t+120)/180)))*255;
  t=t+1;
  lcd.setRGB(r,g,b);//Set R,G,B Value
  delay(20);
}
