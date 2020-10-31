/*
 * AT24C32.c
 *
 * Created: 31.10.2020 15:00:54
 *  Author: Dario Dündar
 */ 


#include "AT24C32.h"

uint8_t at24c32Adr = 0;
uint64_t at24c32WriteFinishes = 0;


void initAt2cC32(uint8_t at24C32Adress)
{
    i2c_init();
    at24c32Adr = at24C32Adress;
}

uint8_t at24c32Write(uint16_t adr, uint8_t* data, uint16_t length)
{
    //wait until the last write has finished
    while(getSystemTimeMs()<at24c32WriteFinishes);
    if (i2c_start(at24c32Adr | 0x00)) return 1;

    i2c_write((adr>>8) & 0b00001111);
    i2c_write(adr    & 0b11111111);

    for (uint16_t i = 0; i < length; i++)
    {
        if (i2c_write(data[i])) return 1;
    }

    i2c_stop();
    //write takes up to 20ms to finish
    at24c32WriteFinishes=getSystemTimeMs()+20;
    return 0;
}

uint8_t at24c32Read(uint16_t adr, uint8_t* data, uint16_t length)
{
    //wait until the last write has finished
    while(getSystemTimeMs()<at24c32WriteFinishes);
    if (i2c_start(at24c32Adr)) return 1;

    i2c_write((adr>>8) & 0b00001111);
    i2c_write(adr    & 0b11111111);

    if (i2c_start(at24c32Adr | 0x01)) return 1;

    for (uint16_t i = 0; i < (length-1); i++)
    {
        data[i] = i2c_read_ack();
    }
    data[(length-1)] = i2c_read_nack();

    i2c_stop();

    return 0;
}