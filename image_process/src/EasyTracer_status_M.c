#include "EasyTracer_status_M.h"
#include "stm32f4xx_hal.h"

#if NEED_DEBUG
#include "stdio.h"
#endif

#define min3v(v1, v2, v3)   ((v1)>(v2)? ((v2)>(v3)?(v3):(v2)):((v1)>(v3)?(v3):(v1)))
#define max3v(v1, v2, v3)   ((v1)<(v2)? ((v2)<(v3)?(v3):(v2)):((v1)<(v3)?(v3):(v1)))

extern uint16_t rawIMG[60][80],imgRGB[60][80];

//读取RBG格式颜色，唯一需要移植的函数
//extern unsigned short GUI_ReadBit16Point(unsigned short x,unsigned short y);

void ReadColor(unsigned int x,unsigned int y,COLOR_RGB *Rgb)
{
	unsigned short C16;

	C16 = imgRGB[y][x];

	Rgb->red   =	 (unsigned char)((C16&0xf800)>>8);
	Rgb->green =	 (unsigned char)((C16&0x07e0)>>3);
	Rgb->blue  =     (unsigned char)((C16&0x001f)<<3);
	
}



//RGB转HSL
void RGBtoHSL(const COLOR_RGB *Rgb, COLOR_HSL *Hsl)
{
    int h,s,l,maxVal,minVal,difVal;
	int r  = Rgb->red;
	int g  = Rgb->green;
    int b  = Rgb->blue;
	
	maxVal = max3v(r, g, b);
	minVal = min3v(r, g, b);
	
	difVal = maxVal-minVal;
	
	//计算亮度
    l = (maxVal+minVal)*240/255/2;
	
	if(maxVal == minVal)//若r=g=b
	{
		h = 0; 
		s = 0;
	}
	else
	{
		//计算色调
		if(maxVal==r)
		{
			if(g>=b)
				h = 40*(g-b)/(difVal);
			else
				h = 40*(g-b)/(difVal) + 240;
		}
		else if(maxVal==g)
			h = 40*(b-r)/(difVal) + 80;
		else if(maxVal==b)
			h = 40*(r-g)/(difVal) + 160;
		//计算饱和度
		if(l == 0)
			s = 0;
		else if(l<=120)
			s = (difVal)*240/(maxVal+minVal);
		else
			s = (difVal)*240/(511 - (maxVal+minVal));
	}
    Hsl->hue =        (unsigned char)(((h>240)? 240 : ((h<0)?0:h)));
    Hsl->saturation = (unsigned char)(((s>240)? 240 : ((s<0)?0:s)));
    Hsl->luminance =  (unsigned char)(((l>240)? 240 : ((l<0)?0:l)));
}

//匹配颜色
static int ColorMatch(const COLOR_HSL *Hsl,const TARGET_CONDI *Condition)
{
	if( 
		Hsl->hue		>	Condition->H_MIN &&
		Hsl->hue		<	Condition->H_MAX &&
		Hsl->saturation	>	Condition->S_MIN &&
		Hsl->saturation	<   Condition->S_MAX &&
		Hsl->luminance	>	Condition->L_MIN &&
		Hsl->luminance	<   Condition->L_MAX 
    )
		return 1;
	else
		return 0;
}

//搜索腐蚀中心
static int SearchCentre(unsigned int *x,unsigned int *y,const TARGET_CONDI *Condition,const SEARCH_AREA *Area)
{
	unsigned int SpaceX,SpaceY,i,j,k,FailCount=0;
	COLOR_RGB Rgb;
	COLOR_HSL Hsl;
	
	SpaceX = Condition->WIDTH_MIN/2;
	SpaceY = Condition->HIGHT_MIN/2;

	for(i=Area->Y_Start;i<Area->Y_End;i+=SpaceY)
	{
		for(j=Area->X_Start;j<Area->X_End;j+=SpaceX)
		{
			FailCount=0;
			for(k=0;k<SpaceX+SpaceY;k++)
			{
				if(k<SpaceX)
					ReadColor(j+k,i+SpaceY/2,&Rgb);
				else
					ReadColor(j+SpaceX/2,i+(k-SpaceX),&Rgb);
				RGBtoHSL(&Rgb,&Hsl);
				
				if(!ColorMatch(&Hsl,Condition))
					FailCount++;
				else
				{
						if(FailCount)
							FailCount--;
				}
				if(FailCount>2)
					break;
			}
			if(k==SpaceX+SpaceY)
			{
				*x = j+SpaceX/2;
				*y = i+SpaceY/2;
				return 1;
			}
		}
	}
	return 0;
}


//#include "st7781.h"

//从腐蚀中心向外腐蚀，得到新的腐蚀中心
static int Corrode(unsigned int oldx,unsigned int oldy,const TARGET_CONDI *Condition,RESULT *Resu)
{
	unsigned int Xmin,Xmax,Ymin,Ymax,i,FailCount;
	COLOR_RGB Rgb;
	COLOR_HSL Hsl;
	
	//printf("Corrode at(%d,%d)\r\n\r\n",oldx,oldy);
	
	//printf("x--\r\n");
	FailCount=0;
	for(i=oldx;i>IMG_X;i--)
	{
		ReadColor(i,oldy,&Rgb);
		RGBtoHSL(&Rgb,&Hsl);
		//printf("(%d,%d)= %d %d %d",i,oldy,Hsl.hue, Hsl.saturation, Hsl.luminance);
		if(!ColorMatch(&Hsl,Condition))
		{
			FailCount++;
			//printf("  %dth no",FailCount);
		}
		else
		{
				if(FailCount)
					FailCount--;
		}
		//printf("\r\n");
		if(FailCount>8)
			break;	
	}
	
	
	//printf("x++\r\n");
	Xmin=i+8;
	FailCount=0;
	
	for(i=oldx;i<IMG_X+IMG_W;i++)
	{
		ReadColor(i,oldy,&Rgb);
		RGBtoHSL(&Rgb,&Hsl);
		//printf("(%d,%d)= %d %d %d",i,oldy,Hsl.hue, Hsl.saturation, Hsl.luminance);
		if(!ColorMatch(&Hsl,Condition))
		{
			FailCount++;
			//printf("  %dth no",FailCount);
		}
		else
		{
				if(FailCount)
					FailCount--;
		}
		//printf("\r\n");
		if(FailCount>8)
			break;	
	}
	
	//printf("y--\r\n");
	Xmax=i-8;
	FailCount=0;
	
	for(i=oldy;i>IMG_Y;i--)
	{
		ReadColor(oldx,i,&Rgb);
		RGBtoHSL(&Rgb,&Hsl);
		//printf("(%d,%d)= %d %d %d",oldx,i,Hsl.hue, Hsl.saturation, Hsl.luminance);
		if(!ColorMatch(&Hsl,Condition))
		{
			FailCount++;
			//printf("  %dth no",FailCount);
		}
		else
		{
				if(FailCount)
					FailCount--;
		}
		//printf("\r\n");
		if(FailCount>8)
			break;	
	}
	
	//printf("y++\r\n");
	Ymin=i+8;
	FailCount=0;
	
	for(i=oldy;i<IMG_Y+IMG_H;i++)
	{
		ReadColor(oldx,i,&Rgb);
		RGBtoHSL(&Rgb,&Hsl);
		//printf("(%d,%d)= %d %d %d",oldx,i,Hsl.hue, Hsl.saturation, Hsl.luminance);
		if(!ColorMatch(&Hsl,Condition))
		{
			FailCount++;
			//printf("  %dth no",FailCount);
		}
		else
		{
				if(FailCount)
					FailCount--;
		}
		//printf("\r\n");
		if(FailCount>8)
			break;	
	}
	Ymax=i-8;
	FailCount=0;
	
	Resu->x	= (Xmin+Xmax)/2;
	Resu->y	= (Ymin+Ymax)/2;
	Resu->w	= Xmax-Xmin;
	Resu->h	= Ymax-Ymin;

	if(((Xmax-Xmin)>(Condition->WIDTH_MIN)) && ((Ymax-Ymin)>(Condition->HIGHT_MIN)) &&\
	   ((Xmax-Xmin)<(Condition->WIDTH_MAX)+1) && ((Ymax-Ymin)<(Condition->HIGHT_MAX)+1) )  //bug
		return 1;	
	else
		return 0;	
}

//唯一的API，用户将识别条件写入Condition指向的结构体中，该函数将返回目标的x，y坐标和长宽
//返回1识别成功，返回1识别失败
int Trace(const TARGET_CONDI *Condition,RESULT *Resu)
{
	static unsigned int x0,y0;
	static SEARCH_AREA FullFrameArea={IMG_X,IMG_X+IMG_W-1,IMG_Y,IMG_Y+IMG_H-1};
	static SEARCH_AREA LastFrameArea;
	static TRACE_STATE TraceState = TS_FIND_IN_FULL_FRAME;
	
	RESULT TempResult;	
	
	switch(TraceState)
	{
		case TS_FIND_IN_FULL_FRAME:
			if(SearchCentre(&x0,&y0,Condition,&FullFrameArea))
			{
				printf("SearchCentre at full frame succeed\r\n");
				TraceState = TS_TRACING;
			}
			else
			{
				#if NEED_DEBUG
				printf("SearchCentre at full frame failed\r\n");
				#endif
				return 0;
			}
		break;
		
		case TS_FIND_IN_LAST_FRAME:
			if(SearchCentre(&x0,&y0,Condition,&LastFrameArea))
			{
				printf("SearchCentre at last frame succeed\r\n");
				TraceState = TS_TRACING;
			}
			else
			{
				#if NEED_DEBUG
				printf("SearchCentre at last frame failed\r\n");
				#endif
				TraceState = TS_FIND_IN_FULL_FRAME;
				return 0;
			}
		break;
		
		case TS_TRACING:
			break;
	}
	
	TempResult.x = x0;
	TempResult.y = y0;

	
	
	if(Corrode(TempResult.x,TempResult.y,Condition,&TempResult)==0)
	{
			#if NEED_DEBUG
			printf("Corrode at (%d,%d) failed\r\n",TempResult.x,TempResult.y);
			#endif
			TraceState = TS_FIND_IN_LAST_FRAME;
			return 0;
	}
			
	Resu->x=TempResult.x;
	Resu->y=TempResult.y;
	Resu->w=TempResult.w;
	Resu->h=TempResult.h;
	
	LastFrameArea.X_Start= TempResult.x - ((TempResult.w)>>1);
	LastFrameArea.X_End  = TempResult.x + ((TempResult.w)>>1);
	LastFrameArea.Y_Start= TempResult.y - ((TempResult.h)>>1);
	LastFrameArea.Y_End  = TempResult.y + ((TempResult.h)>>1);
	
	x0=TempResult.x;
	y0=TempResult.y;
	
	#if NEED_DEBUG
	printf("OK (%d,%d,%d,%d)\r\n",Resu->x,Resu->y,Resu->w,Resu->h);
	#endif
	
	return 1;
}
