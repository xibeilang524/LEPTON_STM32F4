#include "font8x16.h"
#include "TL1771_Driver.h"

void nnop(uint8_t n)
{
	while(n--)
		__NOP();
}

void fill(unsigned int color,unsigned int area)	
{
	int i;
		for (i=0;i<area;i++)
		{
			LCD_Write_Data(color); 
   	}
	}
	
//=========================================================================================================

void LCD_Write_Reg(unsigned int Reg)
{	
		_CLEAR_RS_
		nnop(32);		//tAS
		_CLEAR_CS_
		nnop(32);		
		_CLEAR_WR_
		nnop(32);
		_SET_RD_
		nnop(32);		//tDDR
		GPIOC->ODR=((unsigned char)(Reg>>8));
		nnop(32);
		_SET_WR_
		nnop(32);		//PWHW,PWHR
		_CLEAR_WR_
		nnop(32);
		_SET_RD_
		nnop(32);
		GPIOC->ODR=((unsigned char)(Reg&0x00ff));
		nnop(32);
		_SET_WR_	
		nnop(32);
		_SET_CS_	
		nnop(32);
}
void LCD_Write_Data(unsigned int Data)
{   
		
		_SET_RS_
		nnop(32);
		_CLEAR_CS_
		nnop(32);
		_CLEAR_WR_
		nnop(32);
		_SET_RD_
		nnop(32);
		GPIOC->ODR=((unsigned char)(Data>>8));       //高8位
		nnop(32);
		_SET_WR_
		nnop(32);
		_CLEAR_WR_
		nnop(32);
		_SET_RD_
		nnop(32);
		GPIOC->ODR=((unsigned char)(Data&0x00ff));   //低8位	
		nnop(32);
		_SET_WR_
		nnop(32);
		_SET_CS_
		nnop(32);
}
void LCD_Write_Cmd(unsigned int Reg,unsigned int Data)
{	
	LCD_Write_Reg (Reg) ;
  LCD_Write_Data(Data);
}

//=========================================================================================================
void LCD_SetDispAddr(unsigned int x,unsigned int y)
{
   unsigned int temp;
   temp = ((x<<8)&0xff00)+y ;
   LCD_Write_Cmd(0x0021,temp);  
   LCD_Write_Reg(0x0022);      
}
//========================================================================================================= 
void LCD_SetWindos(unsigned int xstart,unsigned int ystart,unsigned int xend,unsigned int yend)
{              
    LCD_Write_Cmd(0x0045,((yend<<8)+ystart));            
    LCD_Write_Cmd(0x0044,((xend<<8)+xstart));   
    LCD_SetDispAddr(ystart,xstart);
    LCD_Write_Reg(0x0022);   
}
//=========================================================================================================
void LCD_Show_Char(unsigned char c,unsigned char x, unsigned char y,unsigned int color,unsigned int bkcolor)
{
	unsigned char k,line,Ctemp;
	LCD_SetWindos(x,y,x+7,y+15); 

   c=c-0x20;						//字库从空格符开始，空格符之前的符号未列入
	for (line=0; line<16; line++)      
	{   
		Ctemp=font8x16[c][line];        
		for(k=0;k<8;k++)                
		{
			if(Ctemp&0x01)              
			{
				LCD_Write_Data(color);   
			}
			else
			{
				LCD_Write_Data(bkcolor);   
			}
			Ctemp=Ctemp>>1;              
		}
	}
}

//=========================================================================================================
void LCD_Show_String (unsigned char *String,unsigned char x,unsigned char y,unsigned int color,unsigned int bkcolor)
{
	while (*String)                        
	{
		LCD_Show_Char(*String++,x,y,color,bkcolor);           
 		x=x+8; 	                                  
	}
}
//=========================================================================================================

void LCD_Show_Num(uint16_t num,unsigned char x, unsigned char y,unsigned int color,unsigned int bkcolor)
{
	uint8_t numstr[5];	
	numstr[4]=48+(unsigned char)(num%10);	//ge
	numstr[3]=48+(unsigned char)((num/10)%10);		//shi
	numstr[2]=48+(unsigned char)((num/100)%10);		//bai
	numstr[1]=48+(unsigned char)((num/1000)%10);		//qian
	numstr[0]=48+(unsigned char)((num/10000)%10);		//wan
	

	
	LCD_Show_String(numstr,x,y,color,bkcolor);
	
}



void LCD_Clear(unsigned int color)
{
	unsigned int i,j;
	LCD_SetDispAddr(0,0);
	//LCD_SetWindos(0,0,127,160);
	for(i=0;i<160;i++)   
	{
		for(j=0;j<128;j++)   
		{
			LCD_Write_Data (color);    
		}							  
	}
}

void LCD_Init(void)              //液晶初始化
{
	_CLEAR_RESET_    
	HAL_Delay(100);
	_SET_RESET_   
  HAL_Delay(100);

	
  LCD_Write_Cmd(0x0000,0x0001);  //;Power Control 2      
	HAL_Delay(20);
 	LCD_Write_Cmd(0x0011,0x000a);  //;Power Control 2                       
	HAL_Delay(20);
	LCD_Write_Cmd(0x0012,0x0001);  //;Power Control 3
	HAL_Delay(20);
	LCD_Write_Cmd(0x0013,0x081e);  //;Power Control 4
	HAL_Delay(20);
	LCD_Write_Cmd(0x0014,0x0c12);  //;Power Control 5
	HAL_Delay(20);
	LCD_Write_Cmd(0x0010,0x220c);  //;Power Control 1
	HAL_Delay(200);

  LCD_Write_Cmd(0x0002,0x0200); 
	HAL_Delay(20);
 	LCD_Write_Cmd(0x0007,0x0005);  //Display Control 1
	HAL_Delay(20);
	LCD_Write_Cmd(0x0008,0x030a);  //Display Control 2
	HAL_Delay(20);
	LCD_Write_Cmd(0x000b,0x0000); 
	HAL_Delay(20);
	LCD_Write_Cmd(0x000c,0x0000); 
	HAL_Delay(20);
	LCD_Write_Cmd(0x00a0,0x0100); 
	HAL_Delay(20);
	LCD_Write_Cmd(0x00a7,0x0011); 
	HAL_Delay(20);
	LCD_Write_Cmd(0x000e,0x0002); 
	HAL_Delay(20);
	
	LCD_Write_Cmd(0x0030,0x0300);    //;Gamma Control 1
	HAL_Delay(20);
	LCD_Write_Cmd(0x0031,0x0307);    //;Gamma Control 2
	HAL_Delay(20);
	LCD_Write_Cmd(0x0032,0x0000);    //;Gamma Control 3                        
	HAL_Delay(20);
	LCD_Write_Cmd(0x0033,0x0200);    //;Gamma Control 4 
	HAL_Delay(20);
	LCD_Write_Cmd(0x0034,0x0707);    //;Gamma Control 5 
	HAL_Delay(20);
	LCD_Write_Cmd(0x0035,0x0003);    //;Gamma Control 6 
	HAL_Delay(20);
	LCD_Write_Cmd(0x0036,0x0703);    //;Gamma Control 7 
	HAL_Delay(20);
	LCD_Write_Cmd(0x0037,0x0002);    //;Gamma Control 8 
	HAL_Delay(20);
	
	LCD_Write_Cmd(0x0038,0x0a00);    //;Power Control 6  
	HAL_Delay(20);
	LCD_Write_Cmd(0x0040,0x0000);     
	HAL_Delay(20);
	LCD_Write_Cmd(0x0042,0x9f00);     
	HAL_Delay(20);
	LCD_Write_Cmd(0x0043,0x9f00); 
	HAL_Delay(20);
	
	LCD_Write_Cmd(0x0044,0x7f00);   //Horizontal window address
	HAL_Delay(20);
	LCD_Write_Cmd(0x0045,0x9f00);   //Vertical window address
	HAL_Delay(20);
	
	LCD_Write_Cmd(0x0007,0x1001);    //;Display Control 1                       
	HAL_Delay(20);
             
	LCD_Write_Cmd(0x0007,0x1021);    //;Display Control 1                
	HAL_Delay(20);
	LCD_Write_Cmd(0x0007,0x1023);    //;Display Control 1
	HAL_Delay(20);

	LCD_Write_Cmd(0x0007,0x1037);
	HAL_Delay(20);
	
	LCD_Write_Cmd(0x0001,0x010c);  
	HAL_Delay(20);
	LCD_Write_Cmd(0x0044,0x7f00);   //Horizontal window address
	HAL_Delay(20);
	LCD_Write_Cmd(0x0045,0x9f00);   //Vertical window address
	HAL_Delay(20);
	LCD_Write_Cmd(0x0003,0x1018);   //Entry Mode  
	HAL_Delay(20);

}
