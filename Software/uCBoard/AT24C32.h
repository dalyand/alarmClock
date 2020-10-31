/*
 * AT24C32.h
 *
 * Created: 31.10.2020 15:01:03
 *  Author: Dario Dündar
 */ 


#ifndef AT24C32_H_
#define AT24C32_H_

#include "ucBoardDriver.h"
#include "i2c_master.h"

void initAt2cC32(uint8_t at24C32Adress);
uint8_t at24c32Write(uint16_t adr, uint8_t* data, uint16_t length);
uint8_t at24c32Read(uint16_t adr, uint8_t* data, uint16_t length);




#endif /* AT24C32_H_ */