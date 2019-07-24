#ifndef __io_h__
#define __io_h__

void LCD_init();
void LCD_ClearScreen(void);
void LCD_WriteCommand (unsigned char Command);
void LCD_Cursor (unsigned char column);
void LCD_DisplayString(unsigned char column ,const unsigned char *string);
void delay_ms(int miliSec);
void LCD_CreateCharacter(unsigned char pixel[8], unsigned char reg);
void LCD_4bitWrite (unsigned char);
//void LCD_DisplayString_NO_CLEAR( unsigned char column, const unsigned char* string)
#endif
