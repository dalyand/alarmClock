/*
 * rtc_DS3231_i2c.c
 *
 * Created: 31.10.2020 13:55:16
 *  Author: Dario Dündar
 */ 

#include "rtc_DS3231_i2c.h"

// timekeeping registers
#define DS3231_TIME_CAL_ADDR        0x00
#define DS3231_ALARM1_ADDR          0x07
#define DS3231_ALARM2_ADDR          0x0B
#define DS3231_CONTROL_ADDR         0x0E
#define DS3231_STATUS_ADDR          0x0F
#define DS3231_AGING_OFFSET_ADDR    0x10
#define DS3231_TEMPERATURE_ADDR     0x11
#define DS3231_LENGTH_BCD_TIME      7


uint8_t rtcAdr = 0;

void lcdWriteZeit(uint8_t zeile, bcdTime_t bcdTime)
{
    uint8_t spalte=0;
    //Tag
    switch (bcdTime.dayBcd)
    {
        case MONTAG:
        lcdWriteText(zeile,spalte,"Mo ");
        break;
        case DIENSTAG:
        lcdWriteText(zeile,spalte,"Di ");
        break;
        case MITTWOCH:
        lcdWriteText(zeile,spalte,"Mi ");
        break;
        case DONNERSTAG:
        lcdWriteText(zeile,spalte,"Do ");
        break;
        case FREITAG:
        lcdWriteText(zeile,spalte,"Fr ");
        break;
        case SAMSTAG:
        lcdWriteText(zeile,spalte,"Sa ");
        break;
        case SONNTAG:
        lcdWriteText(zeile,spalte,"So ");
        break;
        default:
        lcdWriteText(zeile,spalte,"?? ");
        break;
    }
    spalte = spalte + 3;
    //Datum Tag
    lcdWriteZahl(zeile,spalte,(bcdTime.dateBcd>>4)&0x0f,1,0);
    spalte = spalte + 1;
    lcdWriteZahl(zeile,spalte,(bcdTime.dateBcd)&0x0f,1,0);
    spalte = spalte + 1;
    lcdWriteText(zeile,spalte,".");
    spalte = spalte + 1;
    //Datum Monat
    lcdWriteZahl(zeile,spalte,(bcdTime.monthBcd>>4)&0x0f,1,0);
    spalte = spalte + 1;
    lcdWriteZahl(zeile,spalte,(bcdTime.monthBcd)&0x0f,1,0);
    spalte = spalte + 1;
    lcdWriteText(zeile,spalte,".");
    spalte = spalte + 1;
    //Datum Jahr
    lcdWriteZahl(zeile,spalte,(bcdTime.yearBcd>>4)&0x0f,1,0);
    spalte = spalte + 1;
    lcdWriteZahl(zeile,spalte,(bcdTime.yearBcd)&0x0f,1,0);
    spalte = spalte + 1;
    lcdWriteText(zeile,spalte," ");
    spalte = spalte + 1;
    //Zeit Stunden
    lcdWriteZahl(zeile,spalte,(bcdTime.hourBcd>>4)&0x0f,1,0);
    spalte = spalte + 1;
    lcdWriteZahl(zeile,spalte,(bcdTime.hourBcd)&0x0f,1,0);
    spalte = spalte + 1;
    lcdWriteText(zeile,spalte,":");
    spalte = spalte + 1;
    //Zeit Minuten
    lcdWriteZahl(zeile,spalte,(bcdTime.minuteBcd>>4)&0x0f,1,0);
    spalte = spalte + 1;
    lcdWriteZahl(zeile,spalte,(bcdTime.minuteBcd)&0x0f,1,0);
    spalte = spalte + 1;
    lcdWriteText(zeile,spalte,":");
    spalte = spalte + 1;
    //Zeit Sekunden
    lcdWriteZahl(zeile,spalte,(bcdTime.secondBcd>>4)&0x0f,1,0);
    spalte = spalte + 1;
    lcdWriteZahl(zeile,spalte,(bcdTime.secondBcd)&0x0f,1,0);
}

void initRtc(uint8_t rtcAdress)
{
    i2c_init();
    rtcAdr = rtcAdress;
}

uint8_t rtcWriteTime(bcdTime_t timeBuffer)
{
    return i2c_writeReg(rtcAdr, DS3231_TIME_CAL_ADDR,(uint8_t*)&timeBuffer,sizeof(bcdTime_t));

}

uint8_t rtcReadTime(bcdTime_t* timeBuffer)
{
    return i2c_readReg(rtcAdr,DS3231_TIME_CAL_ADDR,(uint8_t*)timeBuffer,sizeof(bcdTime_t));
}