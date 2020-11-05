/*!
 * @file DFRobot_LCD.cpp
 * @brief DFRobot's LCD
 * @n High Accuracy Ambient Light Sensor
 *
 * @copyright	[DFRobot](http://www.dfrobot.com), 2016
 * @copyright	GNU Lesser General Public License
 *
 * @author [yangyang](971326313@qq.com)
 * @version  V1.0
 * @date  2017-2-10
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "DFRobot_LCD.h"

  void i2cLcdBegin(uint8_t cols, uint8_t rows, uint8_t charsize);
  uint8_t i2cLcdSend(uint8_t *data, uint8_t len);
  void i2cLcdSetReg(uint8_t addr, uint8_t data);

  uint8_t _showfunction;
  uint8_t _showcontrol;
  uint8_t _showmode;
  uint8_t _initialized;
  uint8_t _numlines,_currline;
  uint8_t _lcdAddr;
  uint8_t _RGBAddr;
  uint8_t _cols;
  uint8_t _rows;
  uint8_t _backlightval;

const uint8_t color_define[4][3] = 
{
    {255, 255, 255},            // white
    {255, 0, 0},                // red
    {0, 255, 0},                // green
    {0, 0, 255},                // blue
};

/*******************************public*******************************/


void i2cLcdInit(uint8_t lcd_cols,uint8_t lcd_rows,uint8_t lcd_Addr,uint8_t RGB_Addr)
{
     _lcdAddr = lcd_Addr;
     _RGBAddr = RGB_Addr;
     _cols = lcd_cols;
     _rows = lcd_rows;
    i2c_init();
	_showfunction = LCD_8BITMODE | LCD_2LINE | LCD_5x8DOTS;
	i2cLcdBegin(_cols, _rows, LCD_5x8DOTS);
}

void i2cLcdClear()
{
    i2cLcdCommand(LCD_CLEARDISPLAY);        // clear display, set cursor position to zero
    _delay_us(2000);          // this command takes a long time!
}

void i2cLcdHome()
{
    i2cLcdCommand(LCD_RETURNHOME);        // set cursor position to zero
    _delay_us(2000);        // this command takes a long time!
}

void i2cLcdNoDisplay()
{
    _showcontrol &= ~LCD_DISPLAYON;
    i2cLcdCommand(LCD_DISPLAYCONTROL | _showcontrol);
}

void i2cLcdDisplay() {
    _showcontrol |= LCD_DISPLAYON;
    i2cLcdCommand(LCD_DISPLAYCONTROL | _showcontrol);
}

void i2cLcdStopBlink()
{
    _showcontrol &= ~LCD_BLINKON;
    i2cLcdCommand(LCD_DISPLAYCONTROL | _showcontrol);
}
void i2cLcdBlink()
{
    _showcontrol |= LCD_BLINKON;
    i2cLcdCommand(LCD_DISPLAYCONTROL | _showcontrol);
}

void i2cLcdNoCursor()
{
    _showcontrol &= ~LCD_CURSORON;
    i2cLcdCommand(LCD_DISPLAYCONTROL | _showcontrol);
}

void i2cLcdCursor() {
    _showcontrol |= LCD_CURSORON;
    i2cLcdCommand(LCD_DISPLAYCONTROL | _showcontrol);
}

void i2cLcdScrollDisplayLeft(void)
{
    i2cLcdCommand(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void i2cLcdScrollDisplayRight(void)
{
    i2cLcdCommand(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

void i2cLcdLeftToRight(void)
{
    _showmode |= LCD_ENTRYLEFT;
    i2cLcdCommand(LCD_ENTRYMODESET | _showmode);
}

void i2cLcdRightToLeft(void)
{
    _showmode &= ~LCD_ENTRYLEFT;
    i2cLcdCommand(LCD_ENTRYMODESET | _showmode);
}

void i2cLcdNoAutoscroll(void)
{
    _showmode &= ~LCD_ENTRYSHIFTINCREMENT;
    i2cLcdCommand(LCD_ENTRYMODESET | _showmode);
}

void i2cLcdAutoscroll(void)
{
    _showmode |= LCD_ENTRYSHIFTINCREMENT;
    i2cLcdCommand(LCD_ENTRYMODESET | _showmode);
}

void i2cLcdCustomSymbol(uint8_t location, uint8_t charmap[])
{

    location &= 0x7; // we only have 8 locations 0-7
    i2cLcdCommand(LCD_SETCGRAMADDR | (location << 3));
    
    
    uint8_t data[9];
    data[0] = 0x40;
    for(int i=0; i<8; i++)
    {
        data[i+1] = charmap[i];
    }
    i2cLcdSend(data, 9);
}

void i2cLcdSetCursor(uint8_t col, uint8_t row)
{

    col = (row == 0 ? col|0x80 : col|0xc0);
    uint8_t data[3] = {0x80, col};

    i2cLcdSend(data, 2);

}

void i2cLcdSetRGB(uint8_t r, uint8_t g, uint8_t b)
{
    i2cLcdSetReg(REG_RED, r);
    i2cLcdSetReg(REG_GREEN, g);
    i2cLcdSetReg(REG_BLUE, b);
}


void i2cLcdSetColor(uint8_t color)
{
    if(color > 3)return ;
    i2cLcdSetRGB(color_define[color][0], color_define[color][1], color_define[color][2]);
}

void i2cLcdBlinkLED(void)
{
    ///< blink period in seconds = (<reg 7> + 1) / 24
    ///< on/off ratio = <reg 6> / 256
    i2cLcdSetReg(0x07, 0x17);  // blink every second
    i2cLcdSetReg(0x06, 0x7f);  // half on, half off
}

void i2cLcdNoBlinkLED(void)
{
    i2cLcdSetReg(0x07, 0x00);
    i2cLcdSetReg(0x06, 0xff);
}

uint8_t i2cLcdWriteText(char* text)
{
    uint8_t i=0;
    while(text[i])
    {
        if (i2cLcdWriteChar(text[i]))
        {
            return 1;
        }
        
        i++;
    }
    return 0;
}

uint8_t i2cLcdWriteChar(uint8_t value)
{

    uint8_t data[3] = {0x40, value};
    return i2cLcdSend(data, 2);
}

void i2cLcdCommand(uint8_t value)
{
    uint8_t data[3] = {0x80, value};
    i2cLcdSend(data, 2);
}

void i2cLcdBlink_on(){
	i2cLcdBlink();
}

void i2cLcdBlink_off(){
	i2cLcdStopBlink();
}

void i2cLcdCursor_on(){
	i2cLcdCursor();
}

void i2cLcdCursor_off(){
	i2cLcdNoCursor();
}

void i2cLcdSetBacklight(uint8_t new_val){
	if(new_val){
		i2cLcdBlinkLED();		// turn backlight on
	}else{
		i2cLcdNoBlinkLED();		// turn backlight off
	}
}

void i2cLcdLoad_custom_character(uint8_t char_num, uint8_t *rows){
		i2cLcdCustomSymbol(char_num, rows);
}



/*******************************private*******************************/
void i2cLcdBegin(uint8_t cols, uint8_t lines, uint8_t dotsize) 
{
    if (lines > 1) {
        _showfunction |= LCD_2LINE;
    }
    _numlines = lines;
    _currline = 0;

    ///< for some 1 line displays you can select a 10 pixel high font
    if ((dotsize != 0) && (lines == 1)) {
        _showfunction |= LCD_5x10DOTS;
    }

    ///< SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
    ///< according to datasheet, we need at least 40ms after power rises above 2.7V
    ///< before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
    _delay_ms(50);


    ///< this is according to the hitachi HD44780 datasheet
    ///< page 45 figure 23

    ///< Send function set command sequence
    i2cLcdCommand(LCD_FUNCTIONSET | _showfunction);
    _delay_ms(5);  // wait more than 4.1ms
	
	///< second try
    i2cLcdCommand(LCD_FUNCTIONSET | _showfunction);
    _delay_ms(5);

    ///< third go
    i2cLcdCommand(LCD_FUNCTIONSET | _showfunction);




    ///< turn the display on with no cursor or blinking default
    _showcontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    i2cLcdDisplay();

    ///< clear it off
    i2cLcdClear();

    ///< Initialize to default text direction (for romance languages)
    _showmode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    ///< set the entry mode
    i2cLcdCommand(LCD_ENTRYMODESET | _showmode);
    
    
    ///< backlight init
    i2cLcdSetReg(REG_MODE1, 0);
    ///< set LEDs controllable by both PWM and GRPPWM registers
    i2cLcdSetReg(REG_OUTPUT, 0xFF);
    ///< set MODE2 values
    ///< 0010 0000 -> 0x20  (DMBLNK to 1, ie blinky mode)
    i2cLcdSetReg(REG_MODE2, 0x20);
    
    setColorWhite();

}

uint8_t i2cLcdSend(uint8_t *data, uint8_t len)
{
    
    
    if (i2c_start(_lcdAddr))
    {
        return 1;
    }
    //_delay_ms(5);
    for(int i=0; i<len; i++) {
        if (i2c_write(data[i]))
        {
            return 1;
        }
        
		//_delay_ms(5);
    }
    i2c_stop();
    return 0;
}

  void setPWM(uint8_t color, uint8_t pwm){i2cLcdSetReg(color, pwm);}      // set pwm


void i2cLcdSetReg(uint8_t addr, uint8_t data)
{
    i2c_start(_RGBAddr); // transmit to device #4
    i2c_write(addr);
    i2c_write(data);
    i2c_stop();    // stop transmitting
}

void setColorAll(){i2cLcdSetRGB(0, 0, 0);}
void setColorWhite(){i2cLcdSetRGB(255, 255, 255);}

/************************unsupported API functions***************************/
/*
void DFRobot_LCD::off(){}
void DFRobot_LCD::on(){}
void DFRobot_LCD::setDelay (int cmdDelay,int charDelay) {}
uint8_t DFRobot_LCD::status(){return 0;}
uint8_t DFRobot_LCD::keypad (){return 0;}
uint8_t DFRobot_LCD::init_bargraph(uint8_t graphtype){return 0;}
void DFRobot_LCD::draw_horizontal_graph(uint8_t row, uint8_t column, uint8_t len,  uint8_t pixel_col_end){}
void DFRobot_LCD::draw_vertical_graph(uint8_t row, uint8_t column, uint8_t len,  uint8_t pixel_row_end){}
void DFRobot_LCD::setContrast(uint8_t new_val){}
*/
