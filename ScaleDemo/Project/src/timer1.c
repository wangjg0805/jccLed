#include "stm8s.h"
#include "stm8s_tim1.h"
#include "stm8s_tim2.h"
#include "global.h"
#include "key.h"

static u8 T0ms02,T0ms10,T0ms100,T0sec0_5,Sec0_5,T0min0_5;
       u8 flag_10ms,flag_100ms,flag_500ms;

////////////////////////////////////
//��ʱ��1��ʼ��
////////////////////////////////////
void TIM1_Init(void)
 {
  TIM1_DeInit();
  TIM1_ARRPreloadConfig(ENABLE);
  TIM1_TimeBaseInit(1,TIM1_COUNTERMODE_UP,900,0);
  TIM1_ITConfig(TIM1_IT_UPDATE, ENABLE);
  TIM1_Cmd(ENABLE);
  }
////////////////////////////////////
//��ʱ��2��ʼ��
////////////////////////////////////
void TIM2_Init(void)
 {
  TIM2_DeInit();
  TIM2_ARRPreloadConfig(ENABLE);
  TIM2_TimeBaseInit(TIM2_PRESCALER_16,2000); //1M  ����2000�������� 2ms 
  TIM2_ITConfig(TIM2_IT_UPDATE, ENABLE);
  TIM2_Cmd(ENABLE);
  }
////////////////////////////////////
//��ȡʱ��
////////////////////////////////////
void Timer_Int_Pro(void)
{
	      
		T0ms02++;	
                Key_Scan();
		//========== 10ms Time ===========
		if(T0ms02<5)
			return;
		T0ms02=0;

                T0ms10++;
                flag_10ms = 1;
                /////////////////////////////////////////��������
		if(0!=Key_Sound_Time)   //10ms ��������
		{
                  Beep_Start();
                  Key_Sound_Time--;
		}
                else
                  Beep_Stop();
                ////////////////////////////////////////�����������
		//========== 100ms Time ============
		if(T0ms10<10)
			return;
		T0ms10=0;
		  
                T0ms100++;
                flag_100ms = 1;
			
		//=========== 0.5s Time ============
		if(T0ms100<5)
			return;
	
    	        T0ms100=0;
                flag_500ms = 1;
		T0sec0_5++;
                Sec0_5++;	
              
    	//========= 30s Time ==========
		if(T0sec0_5<60)
			return;

		T0sec0_5=0;
		Sec0_5=0;
		T0min0_5++;
		//========= 30min  Time ============
		if(T0min0_5<60)
			return;
		T0min0_5=0;
   }