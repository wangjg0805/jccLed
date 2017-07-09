#include "stm8s.h"
#include "stdio.h" 
#include "global.h"
#include "energy.h"

#include "timer1.h"

#include "ht1621.h" 
#include "cs5550.h"
#include "rs232.h"
#include "I2C.h"
#include "key.h"

//////////////////////////////////new added
#define AD_BUF_LENGTH   48

static u16 ad_new_data = 0;
static u16 ad_buf[AD_BUF_LENGTH];       //
static u8  buf_index = 0;

////////////////////////////////////////////
/* 1 delay */
////////////////////////////////////////////
static void delay_ms(u32 length)
	{
	  while (length >0)
    	         length--;
	 }

static void filter_init(void)
 {
   for(buf_index=0;buf_index<AD_BUF_LENGTH;buf_index++)
       ad_buf[buf_index] = 0;
   buf_index = 0;   
 }


static void filter(u16 raw_data)
 {
  u8 i,pass;
  u32 sum;
  
  ad_buf[buf_index] = raw_data;
  buf_index++;
  if(buf_index==AD_BUF_LENGTH)
    buf_index = 0;
  
  /*////////////////////////////
  sum = 0;
  for(i=0;i<AD_BUF_LENGTH;i++)
    sum = sum+ad_buf[i];
   ad_new_data = sum/AD_BUF_LENGTH;
  */
  for(pass = 0;pass < AD_BUF_LENGTH - 1 ;pass++)
	   {
              for( i = 0;i < (AD_BUF_LENGTH - 1 - pass);i++)
		 {
		   if(ad_buf[i] > ad_buf[i+1])
	               {
			 sum = ad_buf[i];
		   	 ad_buf[i] = ad_buf[i+1];
			 ad_buf[i+1] = sum ;
		        }
		 }
           }
  
   sum = 0;
   for(i=0;i<AD_BUF_LENGTH-32;i++)
    sum = sum+ad_buf[16+i];
   ad_new_data = sum/(AD_BUF_LENGTH-32);

   /*sum =  ad_buf[AD_BUF_LENGTH/2-4]+ad_buf[AD_BUF_LENGTH/2-3]+ad_buf[AD_BUF_LENGTH/2-2]+ad_buf[AD_BUF_LENGTH/2-1];
       sum +=(ad_buf[AD_BUF_LENGTH/2]  +ad_buf[AD_BUF_LENGTH/2+1]+ad_buf[AD_BUF_LENGTH/2+2]+ad_buf[AD_BUF_LENGTH/2+3]+7);
       ad_new_data = sum/8;*/
 }
////////////////////////////////////////////
//
////////////////////////////////////////////
void HSE_CLK_INIT(void)
{
CLK_DeInit(); 
CLK_HSECmd(ENABLE);
while(SET != CLK_GetFlagStatus(CLK_FLAG_HSERDY));
CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
CLK_ClockSwitchCmd(ENABLE); 
CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO,CLK_SOURCE_HSE,DISABLE,CLK_CURRENTCLOCKSTATE_ENABLE);
}

void HSI_CLK_INIT()
{
 CLK->CKDIVR &= (uint8_t)(~CLK_CKDIVR_HSIDIV);
}
////////////////////////////
//���������
/////////////////////////////

void main(void)       
{
    static u16   ad_tmp,mode_tmp;
    u32   cycle_times = 0;
    u32   cycle_times1 = 0;
    u8   i;
    //use HSEclk
    //HSE_CLK_INIT();
    HSI_CLK_INIT();
  
    
    CPU_RUN_LED_Init();
    /*
    while(1){
    delay_ms(100000);
    GPIO_WriteHigh(GPIOF,  GPIO_PIN_4);
    GPIO_WriteHigh(GPIOD,  GPIO_PIN_4);
    delay_ms(200000);
    GPIO_WriteLow(GPIOF,  GPIO_PIN_4);
    GPIO_WriteLow(GPIOD,  GPIO_PIN_4);
    }
    */
    /*
    //////////////����������ʼ�� pin init  
    //delay(30000);
    Beep_Pin_Init();
    Key_Sound_Time = 20; //��������500ms
    Key_Pin_Init();
    */
    ////////////////////////////ht1621 init 
    HT1621_Pin_Init();
    Init_1621();
    All_ON_Must(); //
    i = 0;
    while(1){
        All_Special_Char(i%10);
        i++;
        if(i&0x01)
            GPIO_WriteHigh(GPIOF,  GPIO_PIN_4);
        else
            GPIO_WriteLow(GPIOF,  GPIO_PIN_4);
        delay_ms(7777777);
    }
    
  ////////////////////////////RS232 init 
  UART2_INIT();
  TIM2_Init();
  ////////////////////////////ii2c init
  I2c_Init();
  
  ///////////////////////////ϵͳӦ�ó�ʼ��
  Energy_Init(); //����ϵ���ͻ���
  APP_Init();
  filter_init();
  ////////////////////////////////////////
  ////////////////////////////cs5550 init
  cs5550_pin_init();
  cs5550_Init();
  /*////////////////////////
  while(1)
  {
    Printer_Init();
    delay_ms(999000);
  } */
  /////////////////////
  Motor_Pin_Init();
  motor_fan_run(32);
  motor_search_run();
  ///////////////////
  enableInterrupts();
  ////////////////driver init
  //����ȫ��3S ��� �а��� ����AD ����״̬
  mode_tmp = 0; //ģʽ��־
  do
  { 
    delay_ms(200); 
    if(flag_10ms)
     {
      flag_10ms = 0;
      ad_tmp++;
       if(KEY_SAVE+KEY_PRESSED_3S == Get_Key_Code())
          mode_tmp = 0x55;
     }   
  }while(ad_tmp!=320);
  ////////////////////////////////////////
  All_off_Must();
  ///////////////////////////////////////������ѭ��
  //��ӡ����ʼ��
  Printer_Init();
  
  while (1)
    {  
      if(flag_10ms)
      {   
       flag_10ms = 0;
       cycle_times1++;
       //cs5550 ���ݶ�ȡ������ѭ���н���
       //ÿ�ζ�ȡ1�����ݣ�Ȼ����ڻ�����
       //ad_tmp = Get_Ad_Data_8();
       Energy_Read();
       filter(Get_Ad_Data());      //�˲�һ��Ȼ������ʾ����
       ////////////////////////////////////////
       ////////////////////////////key
        //����ɨ������жϽ���
       i = Get_Key_Code();
       if(0 != i)
       {                      //��������
        switch(i)
        {
         case KEY_PRESSED+KEY_MODE: 
              Key_Sound_Time = 20; //����200MS
              Key_Mode_Pro();   break;
         case KEY_PRESSED+KEY_UP:  
              Key_Sound_Time = 20; //����200MS
              Key_Up_Pro();     break;
         case KEY_PRESSED+KEY_DOWN:  
              Key_Sound_Time = 20; //����200MS
              Key_Down_Pro();   break; 
         case KEY_PRESSED+KEY_ENTER:  
              Key_Sound_Time = 20; //����200MS
              Key_Enter_Pro();  break;
         case KEY_PRESSED+KEY_SAVE: 
              Key_Sound_Time = 20; //����200MS
              Key_Save_Pro();   break;
         case KEY_PRESSED+KEY_ESC: 
              Key_Sound_Time = 20; //����200MS
              Key_ESC_Pro();    break;
         //////////����Ϊ��������
         case KEY_UP+KEY_PRESSED_3S:
                          if(0 == (cycle_times1%8))
                          Key_Up_Fast();    break;
         case KEY_DOWN+KEY_PRESSED_3S:
                          if(0 == (cycle_times1%8))
                          Key_Down_Fast();  break; 
         case KEY_SAVE+KEY_PRESSED_3S:    //����SAVE ����鿴ģʽ
                          Key_Recall_Pro(); break;
         case KEY_ESC+KEY_PRESSED_3S:     //����ESC  ��������Ѵ��¼
                          Key_Rec_Clear_Pro(); break;                  
         default:break;
        }
       }  //key over
      }   //10ms end  
      ///////////////////////display proc
     if(flag_100ms)
     {
       
      flag_100ms = 0;
      cycle_times++;
      if(0x55 == mode_tmp)   //�鿴����״̬
       {
        //display_ad_data(ad_tmp); 
        display_ad_data(ad_new_data);
       }
      else                   //��������״̬
       {
        //Data_Cal(ad_tmp);     //����  ���ݲ������Դ�
        Data_Cal(ad_new_data);
        //if(0 == (cycle_times%3))
        Refresh_DisplayBuf(); //�Դ����
       }
      }
      //////////////////////////rs232
      if(flag_500ms)
      {
       flag_500ms = 0;
        ////////�����λ���ź�
       if(GPIO_ReadInputPin(GPIOE,GPIO_PIN_6))
       {
        if(GPIO_ReadInputPin(GPIOB,GPIO_PIN_4))
        {
        }
        else
        {
         motor_search_run();
        }  
       }
       else
       {
         if(GPIO_ReadInputPin(GPIOB,GPIO_PIN_4))
        {
         motor_fan_run(125); 
        }
        else
        {
         
        }  
       }
      
       //if(GPIO_ReadInputPin(GPIOE,GPIO_PIN_6))
       // Key_Sound_Time = 20;
      
         
       //Send_RS232(ad_tmp);  
      }
     //ˢ����ʾ�洢�� 
     Refresh_Ht1621();     
   }   //while end
}
