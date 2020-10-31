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

// i2c slave address
#define DS3231_I2C_ADDR             0b11010000  // DS3231 (RTC) 208
#define AT24C32_I2C_ADDR            0b10101110  // AT24C32 (32k Memory) 174

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



int main(void)
{
    //Variablen
    bcdTime_t rtcTime;
    uint64_t timeNow=0;
    uint64_t nextRtcUpdate=0;
    uint8_t rtcTimeChanged=0;
    


    //Board initialisieren
    initBoard();
    lcdLight(255);
    initRtc(DS3231_I2C_ADDR);
    initAt2cC32(AT24C32_I2C_ADDR);

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
        
        if (rtcTimeChanged)
        {
            rtcTimeChanged=0;
            
        	lcdWriteZeit(0,rtcTime);
            ledWrite(0xffff<<(16-((((rtcTime.secondBcd&0x0f)+((rtcTime.secondBcd>>4)&0x0f)*10))*(uint16_t)16/59)));
        }
    }
}

