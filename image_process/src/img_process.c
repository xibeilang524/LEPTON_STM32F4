#include "img_process.h"
#include "stdlib.h"
#include "stm32f4xx_hal.h"
#include "string.h"
#include "TL1771_Driver.h"
#include "LEPTON_Driver.h"

int rangeX(int x)
{
	int temp;
	temp = (x) < 80 ? (x) : 80;
	return (temp > 0 ? (temp) : 0);
}

int rangeY(int x)
{
	int temp;
	temp = (x) < 60 ? (x) : 60;
	return (temp > 0 ? (temp) : 0);
}


POINT imgmax(uint16_t img[][80])
{
	POINT point;
	uint8_t i,j;
	for(i=0;i<60;i++)
		for(j=0;j<80;j++)
		{
			if(img[i][j]>point.value)
			{
				point.x=j;
				point.y=i;
				point.value=img[i][j];
			}
		}
		return point;
}

POINT imgmin(uint16_t img[][80])
{
	POINT point;
	uint8_t i,j;
	for(i=0;i<60;i++)
		for(j=0;j<80;j++)
		{
			if(img[i][j]<point.value)
			{
				point.x=j;
				point.y=i;
				point.value=img[i][j];
			}
		}
		return point;
}


void img8bit(uint16_t inputimg[][80],uint8_t outputimg[][80])
{
	uint8_t X,Y;
	
	for(Y=0;Y<60;Y++)
			for(X=0;X<80;X++)
			{
				outputimg[Y][X] = (float)inputimg[Y][X]/65535*0xFF;
			}			
}

void img2RGB(uint16_t inputimg[][80],uint16_t outputimg[][80])
{
		uint8_t X,Y;
	
	for(Y=0;Y<60;Y++)
			for(X=0;X<80;X++)
			{
				outputimg[Y][X] = TermoClr(inputimg[Y][X],0,65535);
				//outputimg[Y][X] = BLACK;
			}			
}

void binimg(uint16_t inputimg[][80],uint16_t threshold)
{
	uint8_t X,Y;
	for(Y=0;Y<60;Y++)
			for(X=0;X<80;X++)
			{
				if(inputimg[Y][X]>threshold)
					inputimg[Y][X]=1;
				else if(inputimg[Y][X]<=threshold)
					inputimg[Y][X]=0;
			}		
}

void Draw_Rect(uint16_t img[][80],uint8_t x_start,uint8_t y_start,uint8_t width,uint8_t height,uint16_t color)
{
	uint8_t i;
	for(i=0;i<=width;i++)
	{
		img[y_start+height][x_start+i] = color;
		img[y_start][x_start+i] = color;
	}
	for(i=0;i<=height;i++)
	{
		img[y_start+i][x_start] = color;
		img[y_start+i][x_start+width] = color;
	}
}

void Cross_point(uint16_t img[][80],uint8_t x,uint8_t y,uint16_t color)
{
	uint8_t i;
	for(i=0;i<80;i++)
		img[y][i] = color;
	for(i=0;i<60;i++)
		img[i][x] = color;
}
