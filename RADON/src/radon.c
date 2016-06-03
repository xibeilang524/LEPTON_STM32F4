#include <math.h>   
#include <stdio.h>
#include "stdlib.h"
#include "Radon.h"



void incrementRadon(float *pr, float pixel, float r)
{
	int r1;
	float delta;

	r1 = (int)r;
	delta = r - r1;
	pr[r1] += pixel * (1.0 - delta);
	pr[r1 + 1] += pixel * delta;
}

void radon(float *pPtr, float *iPtr, const uint8_t *thetaPtr, int M, int N, int xOrigin, int yOrigin, int numAngles, int rFirst, int rSize)
{
	int k, m, n;              /* loop counters */
	float angle;             /* radian angle value */
	float cosine, sine;      /* cosine and sine of current angle */
	float *pr;               /* points inside output array */
	float *pixelPtr;         /* points inside input array */
	float pixel;             /* current pixel value */
	float *ySinTable, *xCosTable;
	/* tables for x*cos(angle) and y*sin(angle) */
	float x, y;
	float r;
	float deg2rad = 3.1415 / 180.0;

	/* Allocate space for the lookup tables */
	xCosTable = (float *)calloc(2 * N, sizeof(float));  
	ySinTable = (float *)calloc(2 * M, sizeof(float)); 

	for (k = 0; k < numAngles; k++)
	{
		angle = (float)(thetaPtr[k]*deg2rad);
		pr = pPtr + k*rSize;  /* pointer to the top of the output column */
		cosine = cos(angle);
		sine = sin(angle);

		/* Radon impulse response locus:  R = X*cos(angle) + Y*sin(angle) */
		/* Fill the X*cos table and the Y*sin table.                      */
		/* x- and y-coordinates are offset from pixel locations by 0.25 */
		/* spaced by intervals of 0.5. */
		for (n = 0; n < N; n++)
		{
			x = n - xOrigin;
			xCosTable[2 * n] = (x - 0.25)*cosine;  
			xCosTable[2 * n + 1] = (x + 0.25)*cosine;
		}
		for (m = 0; m < M; m++)
		{
			y = yOrigin - m;
			ySinTable[2 * m] = (y - 0.25)*sine;
			ySinTable[2 * m + 1] = (y + 0.25)*sine;
		}

		pixelPtr = iPtr;
		for (n = 0; n < N; n++)
		{
			for (m = 0; m < M; m++)   
			{
				pixel = *pixelPtr++;
				//pixel = *(pixelPtr + n*M + m);
				if (pixel != 0.0)  
				{
					pixel *= 0.25;
					r = xCosTable[2 * n] + ySinTable[2 * m] - rFirst;
					incrementRadon(pr, pixel, r);

					r = xCosTable[2 * n + 1] + ySinTable[2 * m] - rFirst;
					incrementRadon(pr, pixel, r);

					r = xCosTable[2 * n] + ySinTable[2 * m + 1] - rFirst;
					incrementRadon(pr, pixel, r);

					r = xCosTable[2 * n + 1] + ySinTable[2 * m + 1] - rFirst;
					incrementRadon(pr, pixel, r);
				}
			}
		}
	}

	free((void *)xCosTable); 
	free((void *)ySinTable);
}

//POINT* max(unsigned char str[][46])
//{
//	POINT max[2] = {0,0,-100000000L };
//	unsigned int i, j;
//	for (i = 0; i < 103; i++)
//	{
//		for (j = 0; j < 22; j++)
//		{
//			if (str[i][j] > max[0].value)
//			{
//				max[0].value = str[i][j];
//				max[0].x = j;
//				max[0].y = i;
//			}
//		}
//		for (j = 22; j < 46; j++)
//		{
//			if (str[i][j] > max[1].value)
//			{
//				max[1].value = str[i][j];
//				max[1].x = j - 90;
//				max[1].y = i;
//			}
//		}
//	}
//	return max;
//}

LINE* radonLine(float str[103*46])
{

	float pi = 3.14;
	LINE line[2];
	uint8_t i, j;
	float max = 0;
	for (i = 0; i < 22; i++)
	{
		for (j = 0; j < 103; j++)
		{
			if (str[i * 103 + j] > max)
			{
				max = str[i * 103 + j];
				line[0].maxValue = max;
				line[0].max_point_x = 4*i;
				line[0].max_point_y = j;
			}
		}
	}
	max = 0;
	for (i = 22; i < 46; i++)
	{
		for (j = 0; j < 103; j++)
		{
			if (str[i * 103 + j] > max)
			{
				max = str[i * 103 + j];
				line[1].maxValue = max;
				line[1].max_point_x = 4*i - 90;
				line[1].max_point_y = j;
			}
		}
	}

	line[0].theta = 90 - (float)line[0].max_point_x;
	line[0].rho = 102 - (float)line[0].max_point_y - 51;
	line[0].k = (float)tan((line[0].theta - 90) / 180 * pi);
	line[0].b = line[0].rho / sin(line[0].theta / 180 * pi);

	line[1].theta = 180 - (float)line[1].max_point_x;
	line[1].rho = (float)line[1].max_point_y - 51;
	line[1].k = (float)tan((line[1].theta - 90) / 180 * pi);
	line[1].b = line[1].rho / sin(line[1].theta / 180 * pi);

	return line;
}

