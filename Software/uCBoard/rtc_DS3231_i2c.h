/*
 * rtc_DS3231_i2c.h
 *
 * Created: 31.10.2020 13:55:29
 *  Author: Dario Dündar
 */ 


#ifndef RTC_DS3231_I2C_H_
#define RTC_DS3231_I2C_H_

#include "ucBoardDriver.h"
#include "i2c_master.h"


typedef enum day_t{
    MONTAG=1,DIENSTAG,MITTWOCH,DONNERSTAG,FREITAG,SAMSTAG,SONNTAG
}day_t;
#define NR_OF_DAYS                  7
typedef struct bcdTime_t{
    uint8_t secondBcd;/*00-59*/
    uint8_t minuteBcd;/*00-59*/
    uint8_t hourBcd;/*00-24*/
    uint8_t dayBcd;/*1-7*/
    uint8_t dateBcd;/*1-31*/
    uint8_t monthBcd;/*1-12*/
    uint8_t yearBcd;/*00-99*/
} bcdTime_t;

void initRtc(uint8_t rtcAdress);
uint8_t rtcWriteTime(bcdTime_t timeBuffer);
uint8_t rtcReadTime(bcdTime_t* timeBuffer);
void lcdWriteZeit(uint8_t zeile, bcdTime_t bcdTime);


#endif /* RTC_DS3231_I2C_H_ */