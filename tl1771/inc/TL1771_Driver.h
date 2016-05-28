#ifndef __TL1771_DRIVER_H
#define __TL1771_DRIVER_H
//===========================================================================================
#define RED	  0XF800
#define GREEN 0X07E0
#define BLUE  0X001F  
#define BRED  0XF81F
#define GRED  0XFFE0
#define GBLUE 0X07FF
#define BLACK 0X0000
#define WHITE 0XFFFF
#define SKY_BLUE 0x02FF
#define GRAY0    0xEF7D
#include "stm32f4xx_hal.h"
#include "config.h"

//===========================================================================================
//void Delay(unsigned int time);
void LCD_Init(void);
void LCD_Clear(unsigned int color);
void LCD_SetWindos(unsigned int xstart, unsigned int ystart, unsigned int xend, unsigned int yend);
void LCD_Write_Reg(unsigned int Reg);
void LCD_Write_Data(unsigned int Data);
void LCD_Write_Cmd(unsigned int Reg,unsigned int Data);
void LCD_Show_Char(unsigned char c,unsigned char x, unsigned char y,unsigned int color,unsigned int bkcolor);
void LCD_Show_String (unsigned char *String,unsigned char x,unsigned char y,unsigned int color,unsigned int bkcolor);
void fill(unsigned int color,unsigned int area);
void nnop(uint8_t);
void LCD_SetDispAddr(unsigned int x,unsigned int y);
void LCD_Show_Num(uint16_t num,unsigned char x, unsigned char y,unsigned int color,unsigned int bkcolor);

#endif
