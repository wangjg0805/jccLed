#include "stm8s.h"

#include "cs5550.h"
#include "energy.h"

#define      BUF_LENGTH     16

static   u8  Energy_AMP;
static  u16  ad_data;
static  u16  Energy_Buf[BUF_LENGTH];

////////////////////////////////////////////
/* 1 delay */
////////////////////////////////////////////
static void delay(u16 length)
	{
	  while (length >0)
    	         length--;
	 }
/////////////////////////////////////////
//���طŴ���
/////////////////////////////////////////
void  Energy_GetAMP(u8 *AMPData)
   {
    *AMPData = Energy_AMP;
   }
/////////////////////////////////////////
//���÷Ŵ���
/////////////////////////////////////////
void Energy_SetAMP(u8 AMPData)
   {
    delay(200);
    cs5550_config(CS5550_WRITE+CS5550_AIN1_GAIN,Energy_AMPDATA[AMPData]); //0.5
    delay(200);
    Energy_AMP = AMPData;
   }
/////////////////////////////////////////
//��ʼ��
/////////////////////////////////////////
void Energy_Init(void)	//��ʼ�������÷Ŵ���Ϊ0
{
    u8 i;
    Energy_AMP = 5;
    Energy_SetAMP(Energy_AMP);
    for(i=0;i<BUF_LENGTH;i++)
      Energy_Buf[i] = 0;
    return;
}
/////////////////////////////////////////
//��
/////////////////////////////////////////
void   Energy_Read(void)
{   
   static u8 Buf_index = 0;
          u8 i;
         u16 ad_tmp;
         u32 tmp;
    
     do
     { //һֱû��׼���� 
       tmp = cs5550_read(CS5550_READ+CS5550_STATE_REG);        //read status  
     }
      while((tmp&CRDY) == 0);
     
      ad_tmp = cs5550_read(CS5550_READ+CS5550_AIN1_FILT_OUT)>>8; //ʹ��16λ
      cs5550_config(0x5e,0xffffff);  //clear flag
      Energy_Buf[Buf_index++] = ad_tmp;
      if(BUF_LENGTH == Buf_index)
         Buf_index = 0;
      ////////////////////ȡƽ��ֵ
      tmp = 0;
      for(i=0;i<BUF_LENGTH;i++)
      tmp+=Energy_Buf[i];
      ad_data = tmp/BUF_LENGTH;   
}

/////////////////////////////////////////
//get ad_data
/////////////////////////////////////////
u16 Get_Ad_Data(void)
{
 return(ad_data);
}
 /////////////////////////////////////////��ȡ8��ȡƽ��ֵ
u16 Get_Ad_Data_8(void)
  {
       u8   loop_idx;
      u16   ad_tmp[8];
      u32   ad_data_sum; 
     for(loop_idx=0;loop_idx<8;loop_idx++)
      { 
       Energy_Read();
       ad_tmp[loop_idx] = Get_Ad_Data(); 
      }
     ad_data_sum = 0;
     for(loop_idx=0;loop_idx<8;loop_idx++)
     ad_data_sum += ad_tmp[loop_idx];
     return(ad_data_sum/8);
  }

