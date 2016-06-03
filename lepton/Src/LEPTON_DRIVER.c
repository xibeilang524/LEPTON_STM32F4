#include "stm32f4xx_hal.h"
#include "lepton_driver.h"
#include "TL1771_Driver.h"
#include "img_process.h"
#include <stdio.h>
#include <stdlib.h>

extern uint16_t rawIMG[60][80];
extern SPI_HandleTypeDef hspi1;
static uint16_t IMG[60][VOSPI_FRAME_SIZE/2];

uint16_t RGB565(uint8_t R,uint8_t G,uint8_t B) {
	return ((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3);
}

uint16_t TermoClr(uint16_t Val,uint16_t MinVal, uint16_t MaxVal){
	//0 to 600
	float Y;
	uint16_t X;
	Y = (Val - MinVal)*( 550.0 / (MaxVal - MinVal));
	X = (uint16_t)(Y+0.5);
	return RGB565((X < 255 ? X : 255 ), (X < 255 ? 0 : (X<510 ? X-255 : 255) ) , (X < 255 ? 255-X : (X<510 ? 0 : (X-510)*3) ));

}

void Frametransfer(void){
	uint8_t i,X,Y;
	uint8_t frame_number;
	uint8_t lost_frame_counter = 0;
	uint8_t frame_complete = 0;
	uint8_t *FrmPackt;
	
	FrmPackt = (uint8_t *)calloc(VOSPI_FRAME_SIZE,sizeof(uint8_t));
	
	while(!frame_complete){
		
		HAL_SPI_Receive(&hspi1,FrmPackt,VOSPI_FRAME_SIZE,20);
	
		if((FrmPackt[1]&0xf) != 0x0f){			//校验是否为同步帧
			frame_number = FrmPackt[0];

			if(frame_number < 60 ){
				lost_frame_counter = 0;
					for(i=0;i<82;i++){
						if(!i){
							IMG[frame_number][i] = frame_number;
						}else{
							IMG[frame_number][i] = (FrmPackt[2*i] << 8 | FrmPackt[2*i+1]);
						}
					}
			} else {
				lost_frame_counter++;
			}
			if( frame_number >= 59){
				frame_complete = 1;
				lost_frame_counter = 0;
			}
		}else{
			lost_frame_counter++;
			if(lost_frame_counter>100){
				lost_frame_counter = 0;
			}
		}
	}
		for(Y=0;Y<60;Y++)
			for(X=2;X<82;X++)
			{
				rawIMG[Y][X-2] = IMG[Y][X];
			}
			
			free((void *)FrmPackt);
}


//void Frametransfer(void){
//	uint8_t i,X,Y;
//	uint8_t frame_number;
//	uint8_t lost_frame_counter = 0;
//	uint8_t frame_complete = 0;
//	uint16_t Mx,Mn;
//	uint16_t IMG[60][VOSPI_FRAME_SIZE/2];
//	uint8_t FrmPackt[VOSPI_FRAME_SIZE];

//	
//	int last_frame_number;
//	int start_image = 0;
//	int need_resync = 0;
//	int last_crc;
//	int new_frame = 0;
//	int frame_counter = 0;

//	while(!frame_complete){
//		
//		HAL_SPI_Receive(&hspi1,FrmPackt,VOSPI_FRAME_SIZE,20);
//	
//		if((FrmPackt[0]&0x0f) != 0x0f){			//校验是否为同步帧
//			
//			if(FrmPackt[1] == 0  )
//			{
//				if(last_crc != (FrmPackt[3]<<8 | FrmPackt[4]))
//				{
//					new_frame = 1;
//				}
//				last_crc = FrmPackt[3]<<8 | FrmPackt[4];
//			}
//			frame_number = FrmPackt[1];
//			

//			//printf("frame_number:%d\r\n",frame_number);
//			
//			if(frame_number < 60 ){
//				lost_frame_counter = 0;
//					for(i=0;i<80;i++)
//					{
//						rawIMG[frame_number][i] = (FrmPackt[2*i+4] << 8 | FrmPackt[2*i+5]);
//					}
//			} 
//			if( frame_number == 59){
//				frame_complete = 1;
//				lost_frame_counter = 0;
//			}
//		}
//		
//		lost_frame_counter++;
//		if(lost_frame_counter > 100)
//		{
//			need_resync = 1;
//			lost_frame_counter = 0;

//		}

//		if(need_resync)
//		{
//			printf("resync\n\r");
//			HAL_Delay(185);
//			need_resync = 0;
//		}
//	}


////		if(frame_complete)
////		{
////			if(new_frame)
////			{
////				frame_counter++;

////				{
////					print_image_binary();
////				}
////				new_frame = 0;
////			}
////			frame_complete = 0;
////		}
//		
////		for(Y=0;Y<60;Y++)
////			for(X=2;X<82;X++)
////			{
////				rawIMG[Y][X-2] = IMG[Y][X];
////			}
//	//img2RGB(rawIMG,imgRGB);
//}




void Lepton_Init(void)
{
	LEPTON_RESET_L_LOW;
  LEPTON_PW_DWN_LOW;

  HAL_Delay(200);
  LEPTON_PW_DWN_HIGH;

	HAL_Delay(200);
  LEPTON_RESET_L_HIGH;
}
