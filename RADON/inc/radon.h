#ifndef __Radon_H
#define __Radon_H

#include "stm32f4xx_hal.h"
#include "img_process.h"

typedef struct {
	uint8_t max_point_x;
	uint8_t max_point_y; 
	float maxValue;
	float theta;
	float rho;
	float k;
	float b;
}LINE;

void radon(float *pPtr, float *iPtr, const uint8_t *thetaPtr, int M, int N, int xOrigin, int yOrigin, int numAngles, int rFirst, int rSize);
//POINT* max(unsigned char str[][91]);
LINE* radonLine(float str[103*46]);
//POINT min(unsigned char str[][91]);

#endif // !__Radon_H

