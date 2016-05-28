#ifndef __IMG_PROCESS_H
#define __IMG_PROCESS_H

#include "stm32f4xx_hal.h"

typedef struct{
    uint8_t  x;             // [0,255]
    uint8_t  y;           // [0,255]
    double value;            // [0,255]
}POINT;//RGB¸ñÊ½ÑÕÉ«


POINT imgmax(uint16_t img[][80]);
POINT imgmin(uint16_t img[][80]);

void img8bit(uint16_t inputimg[][80],uint8_t outputimg[][80]);
void binimg(uint16_t inputimg[][80],uint16_t threshold);

int rangeX(int x);
int rangeY(int x);

void img2RGB(uint16_t inputimg[][80],uint16_t outputimg[][80]);
void Draw_Rect(uint16_t img[][80],uint8_t x_start,uint8_t y_start,uint8_t width,uint8_t height,uint16_t color);
void Cross_point(uint16_t img[][80],uint8_t x,uint8_t y,uint16_t color);

#endif
