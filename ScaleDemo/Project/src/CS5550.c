/********************************************************************************
 *    CS5550.c: C file for CS5550
 *    Describtion: The C file is the definition of CS5550 for the LPC2378
 *                 family microprocessor.
********************************************************************************/
#include "stm8s.h"

#include "CS5550.h"

//8个放大倍数
const  u32 Energy_AMPDATA[8]={GAIN_0_03125,GAIN_0_0625,GAIN_0_125,GAIN_0_25,GAIN_0_5,GAIN_1,GAIN_2,GAIN_4};

////////////////////////////////////////////
/* 1 delay */
////////////////////////////////////////////
static void delay(u16 length)
	{
	  while (length >0)
    	         length--;
	 }
////////////////////////////////////////////
/* 2 pin init */
///////////////////////////////////////////
void cs5550_pin_init(void)
{
   GPIO_Init(GPIOB,CS5550_RESET, GPIO_MODE_OUT_PP_LOW_FAST);
   GPIO_Init(GPIOA,CS5550_CS,    GPIO_MODE_OUT_PP_LOW_FAST);
   GPIO_Init(GPIOA,CS5550_SCLK,  GPIO_MODE_OUT_PP_LOW_FAST);
   GPIO_Init(GPIOB,CS5550_SDI,   GPIO_MODE_OUT_PP_LOW_FAST);
     
   GPIO_Init(GPIOA,CS5550_SDO,    GPIO_MODE_IN_PU_NO_IT);
   GPIO_Init(GPIOB,CS5550_INT,    GPIO_MODE_IN_PU_NO_IT);//使用查询方式
  // GPIO_Init(GPIOB,CS5550_INT,    GPIO_MODE_IN_PU_IT); //设置为有中断
}

////////////////////////////////////////////
/* 3 ad_write_byte*/
////////////////////////////////////////////
static void ad_write_byte(u8 dat)
{
	u8 i,j;
        j = dat;
       for(i=0;i<8;i++)
	 {      
                if(j & 0x80)
                  CS5550_SDI_HIGH;
		else
                  CS5550_SDI_LOW;
                delay(35);
	        CS5550_CLK_LOW;
                delay(35);
                CS5550_CLK_HIGH;	
                delay(35);
                j<<=1; 
	  }
         CS5550_SDI_HIGH;	
         //delay(100);
}

////////////////////////////////////////////
/* 4 ad_read_byte*/
////////////////////////////////////////////
static u32 ad_read_24bit()
{
	u8 i;
	u32 dat;
        dat = 0;   
        for(i=0;i<24;i++)
          { 
               CS5550_CLK_LOW;    //Clear SCLK for low level.
               delay(35);
	        if((i==7)||(i==15)||(i==23))
                     CS5550_SDI_LOW;  
		else
		    CS5550_SDI_HIGH;
                delay(35);
	        CS5550_CLK_HIGH;
                dat<<=1;
                if(READ_CS5550_SDO)
                dat |= 0x01;
                delay(35);
          }
        CS5550_SDI_HIGH; 
        delay(35);
      	return dat;
}
////////////////////////////////////////////
/* 5 cs5550_read_ad*/
////////////////////////////////////////////
u32 cs5550_read(u8 cmd)
{
    ad_write_byte(cmd);
    return (ad_read_24bit());
}
////////////////////////////////////////////
/* 6 write cmd*/
////////////////////////////////////////////
void cs5550_command(u8 cmd)
{
    ad_write_byte(cmd);  
}
////////////////////////////////////////////
/* 7 CS5550 configuration*/
////////////////////////////////////////////
void cs5550_config(u8 cmd, u32 value)
{
    ad_write_byte(cmd);
    ad_write_byte((value>>16)&0xff);
    ad_write_byte((value>>8)&0xff);
    ad_write_byte(value&0xff);
}
////////////////////////////////////////////
/* 8 CS5550 Serial Port initial*/
////////////////////////////////////////////
void cs5550_Init(void)
  {
      //delay(200);
      u32 tmp;
      GPIO_WriteLow(GPIOB,CS5550_RESET);
      delay(200);
      GPIO_WriteHigh(GPIOB,CS5550_RESET);
      delay(200);
      GPIO_WriteLow(GPIOA,CS5550_CS);   //始终拉低
      delay(200);
      cs5550_config(0xff,0xffffff);
      cs5550_config(0xff,0xfffffe);
      delay(200);
      cs5550_config(0x40,0x000001);
      delay(200);
      cs5550_config(0x4a,0x0000fa);
      delay(200);
      cs5550_config(0x74,0x800001);
      delay(200);
      cs5550_config(0x5e,0xffffff);
      delay(200);
      //开机校准0
      /*
      delay(200);
      cs5550_command(CS5550_CAL_CH1+CS5550_CAL_OFF);
      delay(10000);
      do
      {  
       tmp = cs5550_read(CS5550_READ+CS5550_STATE_REG); //read status  
      }while((tmp&DRDY)==0);
      cs5550_config(0x5e,0xffffff);
      delay(200);*/
      //设置默认系数
      cs5550_command(0xe8);
      delay(200);
      
     /* cs5550_config(CS5550_WRITE+CS5550_CONFIG_REG,GAIN_10X+INT_LOW_LEVEL+AIN1_HPF_DISABLE+CLK_DIV_1);
     //delay(200);
     cs5550_config(CS5550_WRITE+CS5550_CYCLE_REG,CS5550_CYCLE_COUNT);
     //delay(200);
     cs5550_config(CS5550_WRITE+CS5550_MASK_REG,DRDY);      //当DRDY有效时候激活INT引脚 
     //delay(200);
     cs5550_config(CS5550_WRITE+CS5550_STATE_REG,0xffffff); //clear all
     //delay(200);  
     cs5550_command(CS5550_CONTINUE_CONVERT_CMD);
     //delay(20000);*/
  }

