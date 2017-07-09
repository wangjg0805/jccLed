#include "stm8s.h"
#include "global.h"
#include "key.h"

static volatile u8 key_status,key_press,key_press_3s;    //ÿһλ��Ӧһ������(Ϊ1��ʾ��Ӧ�¼�����)

////////////////////////////
//�������ܽų�ʼ��
///////////////////////////
void Beep_Start(void)
 {
   GPIO_WriteLow(GPIOD, BEEP_PIN); 
 }
void Beep_Stop(void)
 {
   GPIO_WriteHigh(GPIOD, BEEP_PIN); 
 }

void Beep_Pin_Init(void)
 {
   GPIO_Init(GPIOD,BEEP_PIN,GPIO_MODE_OUT_PP_LOW_FAST);
   Beep_Stop(); 
 }

////////////////////////////
//���̹ܽų�ʼ��
///////////////////////////
void Key_Pin_Init(void)
 {  //6������
   GPIO_Init(GPIOC,GPIO_PIN_1,GPIO_MODE_IN_PU_NO_IT);
   GPIO_Init(GPIOC,GPIO_PIN_2,GPIO_MODE_IN_PU_NO_IT);
   GPIO_Init(GPIOC,GPIO_PIN_3,GPIO_MODE_IN_PU_NO_IT);
   GPIO_Init(GPIOC,GPIO_PIN_5,GPIO_MODE_IN_PU_NO_IT);
   GPIO_Init(GPIOC,GPIO_PIN_6,GPIO_MODE_IN_PU_NO_IT);
   GPIO_Init(GPIOC,GPIO_PIN_7,GPIO_MODE_IN_PU_NO_IT);
 }
//**********************************************************************
//����  ��:Key_Scan
//��������:
//��ڲ���:��
//���ڲ���:��
//˵    ��:ÿ2msɨ��һ��
//**********************************************************************
#define KEY_MASK    0x11
void  Key_Scan(void)
    {
      static u16 key_press_time[KEY_NUM];    //��¼ÿ���������µ�ʱ��
              u8 i,j;
  
      key_status = GPIO_ReadInputData(GPIOC)|KEY_MASK; //ĳλΪ0��ʾ�а���
      if(0xff == key_status)
      {
         for (i=0; i<KEY_NUM; i++)
           key_press_time[i] = 0;  //����
         
         key_press = 0;
      }  
      else
      { //�м�����
         for (i=0; i<KEY_NUM; i++) //ɨ��ÿ�������ź�
         {  
            j = 1;        //���εõ�����λ
            switch(i)
            {
             case 0: j=1;break;
             case 1: j=2;break;
             case 2: j=3;break;
             case 3: j=5;break;
             case 4: j=6;break;
             case 5: j=7;break;
             default:break;
            }
             if(key_status & (1 << j))
             {  //ĳ��δ����,�ۼ�ʱ������
                key_press_time[i] = 0;
             }
             if(key_press_time[i] == KEY_PRESS_TIME)
             {  //ĳ��������ȥ(����50ms)
                key_press |= 1 << i;
             }      
             if(key_press_time[i]== KEY_3S_TIME)
             {  //ĳ��һֱ����3s
                key_press_3s |= 1 << i;
             }
             key_press_time[i]++;   //��ǰ����ʱ������           
          }
      }
   }
//////////////////////////////////
//**********************************************************************
//����  ��:Get_Key_Status
//��������:����״̬
//��ڲ���:��
//���ڲ���:��״ֵ̬(��Ӧλ1��ʾ�˼�������)
//**********************************************************************
u8 Get_Key_Status(void)
  {
    return(key_status);
  }

/****************************************************************************
* ���ƣ�Get_KeyCode()
* ���ܣ�������
* ��ڲ�������
* ���ڲ�������4λ����(0~15), ��4λ����(0 δ��,1 ����,2 ����2s, 3 ����5s)
****************************************************************************/
u8 Get_Key_Code(void)
{
    u8 i;
    for(i=0; i<KEY_NUM; i++)
    {    
       if(key_press & (1 << i))
        {
            key_press = 0;        //���ֵ
            return(KEY_PRESSED + i);
        }     

       if(key_press_3s & (1 << i))
        {
          if(0xff==key_status)//�ͷŰ��������ֵ
            key_press_3s = 0;    //���ֵ 
            return(KEY_PRESSED_3S + i);
        }
    }
    return(0);
}

