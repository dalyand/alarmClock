/*
* uCBoardBeispiele.c
*
* Created: 26.10.2020 08:43:59
* Author : Dario Dündar
*/

#include <avr/io.h>
#include "ucBoardDriver.h"
#include "rtc_DS3231_i2c.h"
#include "AT24C32.h"
#include "DFRobot_LCD.h"

// i2c slave address
#define DS3231_I2C_ADDR             0b11010000  // DS3231 (RTC) 208
#define AT24C32_I2C_ADDR            0b10101110  // AT24C32 (32k Memory) 174
                    //6   0b00000110
#define LCD_I2C_ADDR                0b01111100  // LCD 124 
#define LCD_RGB_I2C_ADDR            0b11000000  // LCD-RGB 192 
                    //224 0b11100000
                    

#define RTC_POLL_TIME_MS            1000

void ERROR (void){
    lcdClear();
    lcdWriteText(0,0,"i2c ERROR!!!");
    lcdLight(255);
    while(1){
        ledWrite(0xffff);
        _delay_ms(500);
        ledWrite(0);
        _delay_ms(50);
    }
}

void i2cLcdWriteZeit(bcdTime_t bcdTime)
{
    i2cLcdSetCursor(0,0);
    i2cLcdWriteText("    ");
    //Zeit Stunden
    i2cLcdWriteChar(((bcdTime.hourBcd>>4)&0x0f)+0x30);
    i2cLcdWriteChar(((bcdTime.hourBcd)&0x0f)+0x30);
    i2cLcdWriteChar(':');
    //Zeit Minuten
    i2cLcdWriteChar(((bcdTime.minuteBcd>>4)&0x0f)+0x30);
    i2cLcdWriteChar(((bcdTime.minuteBcd)&0x0f)+0x30);
    i2cLcdWriteChar(':');
    //Zeit Sekunden
    i2cLcdWriteChar(((bcdTime.secondBcd>>4)&0x0f)+0x30);
    i2cLcdWriteChar(((bcdTime.secondBcd)&0x0f)+0x30);
    i2cLcdWriteText("    ");
    
    i2cLcdSetCursor(0,1);
    i2cLcdWriteText("  ");
    //Tag
    switch (bcdTime.dayBcd)
    {
        case MONTAG:
        i2cLcdWriteText("Mo, ");
        break;
        case DIENSTAG:
        i2cLcdWriteText("Di, ");
        break;
        case MITTWOCH:
        i2cLcdWriteText("Mi, ");
        break;
        case DONNERSTAG:
        i2cLcdWriteText("Do, ");
        break;
        case FREITAG:
        i2cLcdWriteText("Fr, ");
        break;
        case SAMSTAG:
        i2cLcdWriteText("Sa, ");
        break;
        case SONNTAG:
        i2cLcdWriteText("So, ");
        break;
        default:
        i2cLcdWriteText("??, ");
        break;
    }
    //Datum Tag
    i2cLcdWriteChar(((bcdTime.dateBcd>>4)&0x0f)+0x30);
    i2cLcdWriteChar(((bcdTime.dateBcd)&0x0f)+0x30);
    i2cLcdWriteChar('.');
    //Datum Monat
    i2cLcdWriteChar(((bcdTime.monthBcd>>4)&0x0f)+0x30);
    i2cLcdWriteChar(((bcdTime.monthBcd)&0x0f)+0x30);
    i2cLcdWriteChar('.');
    //Datum Jahr
    i2cLcdWriteChar(((bcdTime.yearBcd>>4)&0x0f)+0x30);
    i2cLcdWriteChar(((bcdTime.yearBcd)&0x0f)+0x30);
    i2cLcdWriteText("  ");
}

void farbshow(void)
{
    static uint8_t red = 255;
    static uint8_t redCount=0;//0=stop, 1=up, 2=down
    static uint8_t green = 0;
    static uint8_t greenCount=1;//0=stop, 1=up, 2=down
    static uint8_t blue = 0;
    static uint8_t blueCount=0;//0=stop, 1=up, 2=down
    
    if (greenCount==1)
    {
        green++;
        if (green==255)
        {
            greenCount=0;
            redCount=2;
        }
    }
    else if (redCount==2)
    {
        red--;
        if (red==0)
        {
            redCount=0;
            blueCount=1;
        }
    }
    else if (blueCount==1)
    {
        blue++;
        if (blue==255)
        {
            blueCount=0;
            greenCount=2;
        }
    }
    else if (greenCount==2)
    {
        green--;
        if (green==0)
        {
            greenCount=0;
            redCount=1;
        }
    }else if (redCount==1)
    {
        red++;
        if (red==255)
        {
            redCount=0;
            blueCount=2;
        }
    }else if (blueCount==2)
    {
        blue--;
        if (blue==0)
        {
            blueCount=0;
            greenCount=1;
        }
    }
    i2cLcdSetRGB(red,green,blue);
}

#define FARBSHOW_MS     20

int main(void)
{
    //Variablen
    bcdTime_t rtcTime;
    uint64_t timeNow=0;
    uint64_t nextRtcUpdate=0;
    uint8_t rtcTimeChanged=0;
    uint8_t sekunden=0;
    uint64_t nextFarbshow=0;


    //Board initialisieren
    initBoard();
    lcdLight(255);
    initRtc(DS3231_I2C_ADDR);
    initAt2cC32(AT24C32_I2C_ADDR);
    i2cLcdInit(16,2,LCD_I2C_ADDR,LCD_RGB_I2C_ADDR);

//Set RTC-Time
//     const bcdTime_t RESET_TIME = {0x15/*sec*/,0x44/*min*/,0x14/*h*/,0x06/*day*/,0x31/*date*/,0x10/*month*/,0x20/*year*/};  
//     if (rtcWriteTime(RESET_TIME))
//     {
//         ERROR();
//     }    

//i2c-Scan
//     for(uint8_t i=0;i<127;i++)
//     {
//         if (!i2c_start(i<<1))
//         {
//             lcdWriteZahl(1,0,i<<1,3,0);
//         }
//         i2c_stop();
//         
//     }
   
    
    while(1)
    {
        //Eingabe-------------------------------------------------------------------------
        timeNow = getSystemTimeMs();
        sekunden = ((rtcTime.secondBcd&0x0f)+((rtcTime.secondBcd>>4)&0x0f)*10);
        if (timeNow > nextRtcUpdate)
        {
            if (rtcReadTime(&rtcTime))
            {
                ERROR();
            }
            rtcTimeChanged= 1;
            nextRtcUpdate = timeNow + RTC_POLL_TIME_MS;
        }
        

        //Verarbeitung--------------------------------------------------------------------
        
        //Ausgabe------------------------------------------------------------------
        if (getSystemTimeMs() > nextFarbshow)
        {
            farbshow();
            nextFarbshow = nextFarbshow + FARBSHOW_MS;
        }
        
        
        if (rtcTimeChanged)
        {
            rtcTimeChanged=0;
            
        	lcdWriteZeit(0,rtcTime);
            //ledWrite(0xffff<<(16-((sekunden)*(uint16_t)16/59)));
            i2cLcdWriteZeit(rtcTime);            
            //i2cLcdSetRGB((!((sekunden)%3))*255,(!((sekunden+1)%3))*255,(!((sekunden+2)%3))*255);

        }
    }
}

