#include "template_match.h"

#if TEMPLATE_HEIGHT==8
const uint8_t template_matrix[TEMPLATE_HEIGHT][TEMPLATE_WIDTH] = {
																															{1,1,1,1,1,1,1,1},
																															{1,1,1,1,1,1,1,1},
																															{0,0,0,1,1,0,0,0},
																															{0,0,0,1,1,0,0,0},
																															{0,0,0,1,1,0,0,0},
																															{0,0,0,1,1,0,0,0},
																															{0,0,0,1,1,0,0,0},
																															{0,0,0,1,1,0,0,0}
																														};		//8X8
#elif TEMPLATE_HEIGHT==12																														
const uint8_t template_matrix[TEMPLATE_HEIGHT][TEMPLATE_WIDTH] = {
																															{1,1,1,1,1,1,1,1,1,1,1,1},
																															{1,1,1,1,1,1,1,1,1,1,1,1},
																															{1,1,1,1,1,1,1,1,1,1,1,1},
																															{1,1,1,1,1,1,1,1,1,1,1,1},
																															{0,0,0,0,1,1,1,1,0,0,0,0},
																															{0,0,0,0,1,1,1,1,0,0,0,0},
																															{0,0,0,0,1,1,1,1,0,0,0,0},
																															{0,0,0,0,1,1,1,1,0,0,0,0},
																															{0,0,0,0,1,1,1,1,0,0,0,0},
																															{0,0,0,0,1,1,1,1,0,0,0,0},
																															{0,0,0,0,1,1,1,1,0,0,0,0},
																															{0,0,0,0,1,1,1,1,0,0,0,0}
																														};	//12X12
#endif


void show_template(uint16_t img[][80],uint8_t x,uint8_t y)
{
	uint8_t template_x,template_y;
	for(template_y=0;template_y<TEMPLATE_HEIGHT;template_y++)
		for(template_x=0;template_x<TEMPLATE_WIDTH;template_x++)
		{
			if(template_matrix[template_y][template_x])
				img[y+template_y][x+template_x]=GREEN;
		}
}
																														

void detect(uint16_t binimg[][80],float correlation)
{
	uint8_t x,y;
	uint8_t template_x,template_y;
	uint8_t count_correlation=0;
	uint8_t break_flag = 0;
	for(y=0;y<60;y++)
	{
		if(break_flag)
		{
			break_flag=0;
			break;
		}
		for(x=0;x<80;x++)
		{
			count_correlation=0;
			if(binimg[y][x])		//找到白点
			{
				for(template_y=0;template_y<TEMPLATE_HEIGHT;template_y++)
					for(template_x=0;template_x<TEMPLATE_WIDTH;template_x++)
					{
						if(binimg[y+template_y][x+template_x]==template_matrix[template_y][template_x])	//逐像素检查是否符合模板
						{
							count_correlation++;
						}
					}
				if((float)count_correlation/(TEMPLATE_WIDTH*TEMPLATE_HEIGHT) >= correlation)
				{
					show_template(binimg,x,y);
					break_flag=1;
					break;
				}
			}
		}
	}
}
