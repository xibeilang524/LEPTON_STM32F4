#ifndef __DETECT_H
#define __DETECT_H

#include "TL1771_Driver.h"
#include "stm32f4xx_hal.h"

#define TEMPLATE_WIDTH 12		// 8 or 12
#define TEMPLATE_HEIGHT 12

void show_template(uint16_t img[][80],uint8_t x,uint8_t y);
void detect(uint16_t binimg[][80],float correlation);																														
																														
#endif
