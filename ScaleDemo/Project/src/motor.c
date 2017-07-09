#include "stm8s.h"
#include "global.h"
#include "motor.h"

//OLD
//unsigned char code   FANRUN[8]={0xf8,0xfc,0xdc,0xde,0xda,0xdb,0xd9,0xf9}; //��ת
//unsigned char code ZHENGRUN[8]={0xf9,0xd9,0xdb,0xda,0xde,0xdc,0xfc,0xf8}; //��ת
//NEW USE 0 1 2 3 
///0001 0011 0010 0110 0100 1100 1000 1001 ��
static       u8  index=0;
static const u8  FANRUN[8]={0x08,0x0c,0x04,0x06,0x02,0x03,0x01,0x09}; //��ת
static const u8  ZHENGRUN[8]={0x09,0x01,0x03,0x02,0x06,0x04,0x0c,0x08}; //��ת


void CPU_RUN_LED_Init(void)
 {  
   GPIO_Init(GPIOF,GPIO_PIN_4,GPIO_MODE_OUT_PP_LOW_FAST);
   GPIO_Init(GPIOD,GPIO_PIN_4,GPIO_MODE_OUT_PP_LOW_FAST);
 }


void Motor_Pin_Init(void)
 {  //6������
   GPIO_Init(GPIOB,GPIO_PIN_0,GPIO_MODE_OUT_PP_LOW_FAST);
   GPIO_Init(GPIOB,GPIO_PIN_1,GPIO_MODE_OUT_PP_LOW_FAST);
   GPIO_Init(GPIOB,GPIO_PIN_2,GPIO_MODE_OUT_PP_LOW_FAST);
   GPIO_Init(GPIOB,GPIO_PIN_3,GPIO_MODE_OUT_PP_LOW_FAST);
   
   GPIO_Init(GPIOB,GPIO_PIN_4,GPIO_MODE_IN_PU_NO_IT);
   GPIO_Init(GPIOE,GPIO_PIN_6,GPIO_MODE_IN_PU_NO_IT);
 }


/******************************************************
*
*	             ��ʱ����
*
********************************************************/
 static void delay(unsigned int t)
{                           
   unsigned int k;
   while(t--)
   {
     for(k=0; k<5000; k++)
     //for(k=0; k<2500; k++)
     { }
   }
}
/////////////////////////////////////////
void   motor_phase_input(u8 phase)
 {
    switch(phase)
    {
     case 0x09:
              PHASE_A_H;
              PHASE_B_L;
              PHASE_C_L;
              PHASE_D_H;
              break;
     case 0x01:
              PHASE_A_L;
              PHASE_B_L;
              PHASE_C_L;
              PHASE_D_H;
              break;
     case 0x03:
              PHASE_A_L;
              PHASE_B_L;
              PHASE_C_H;
              PHASE_D_H;
              break;
     case 0x02:
               PHASE_A_L;
               PHASE_B_L;
               PHASE_C_H;
               PHASE_D_L;
               break;
     case 0x06:
               PHASE_A_L;
               PHASE_B_H;
               PHASE_C_H;
               PHASE_D_L;
               break;
     case 0x04:
               PHASE_A_L;
               PHASE_B_H;
               PHASE_C_L;
               PHASE_D_L;
               break;
     case 0x0c:
               PHASE_A_H;
               PHASE_B_H;
               PHASE_C_L;
               PHASE_D_L;
               break;
     case 0x08:
              PHASE_A_H;
              PHASE_B_L;
              PHASE_C_L;
              PHASE_D_L;
              break;  
     default:break;
     } 
 } 
//////////////////////////////////////////////
void motor_fan_run(u16 steps)
{
 u16 i;
  u8 j;
 for(i=0;i<steps;i++)
   {
     j = FANRUN[index];
     motor_phase_input(j);
     delay(12000);             
     index++;
     if(index==8) index=0;
   }
}
//////////////////////////////////////

void motor_search_run(void)
{
 u8   j,k; //
 u16  i;
 i=0;
 do
  { 
     j = ZHENGRUN[index];
     motor_phase_input(j);
     delay(12000);             
     index++;
     if(index==8) index=0;
     i++;
   }
    while((i<500)&&(0==GPIO_ReadInputPin(GPIOB,GPIO_PIN_4)));
 
   for(k=0;k<3;k++)
   {
     j = ZHENGRUN[index];
     motor_phase_input(j);
     delay(12000);  
     index++;
     if(index==8) index=0;
   }
}
/*
void  main() 
 { 
   unsigned char x,i,temp_v,motor_phase;
   unsigned int y;
   //P2 = 0xff;           //input signal
   //����������λ��
   P1M1 = 0x0;
   P1M0 = 0x27;
   Beep_Run();
   //////////////////////////////////�����Ȼ���N����Ȼ�����������
   x = 0;
   for(y=0;y<32;y++)
   {
    P1 &= FANRUN[x];
	P1 |= FANRUN[x]&0x27;
	delay(5);             
    x++;
	if(x==8) x=0;
   }
   delay(500);
   ///////////////////////////////////////
   x = 0x55;
   y = 0xaa;
   while(x!=y)
   {
    delay(3);
    x = Motor_POS;
    delay(3);
    y = Motor_POS;			
   }
  if(x == MOTOR_NO_ARR)             //��￪������ָ��λ��
    {  y = 0;
	   i = 0;
	   do{
	       //P1 = FANRUN[i]&0x0f;      //ȡ����
		  	 P1 &= ZHENGRUN[i];
			 P1 |= ZHENGRUN[i]&0x27;
		
		     delay(5);                 //����ת��
           i++;	y++;
		   if(i==8) i=0;
		   x = Motor_POS;
		  }	 
		  while((x!=MOTOR_ARRIVE) && (y !=260));  //ֱ����ﵽλ��ֹͣ
		  Beep_Run();
		  motor_phase = i;
		  if(y==260)                      //timeout
		 {	
		    i = motor_phase;
		    for(;y!=0;y--)
		      { 
			 	 P1 &= FANRUN[i];
			     P1 |= FANRUN[i]&0x27;
				delay(5);                 //����ת��
                i++;	
		        if(i==8) i=0;
		       }	 
		    while(1)
			{
			Beep_Run();
			} //ֹͣ����������
		 }
	}
   /////////////////////////////////////������������״̬
	motor_phase = 0;
   while(1)
   {
 ganrao:
     temp_v =  Lamp_SEL;
	 delay(100);
	 if(temp_v != Lamp_SEL)
	   goto ganrao;
     if(temp_v == 1)
	 {
     if(Motor_POS==MOTOR_NO_ARR)   //ѡ�񵶵��ź� ����ﲻ��λ��,���е��л�λ��
       { y = 0;
	     i = 0;
	   do{
	         P1 &= ZHENGRUN[i];
			 P1 |= ZHENGRUN[i]&0x27; 
			    delay(2);                 //����ת��
           i++;	y++;
		   if(i==8) i=0;
		   x = Motor_POS;
		   delay(3);                 //����ת��
		  }	 
		  while((x!=MOTOR_ARRIVE) && (y !=260));  //ֱ����ﵽλ��ֹͣ���߳�ʱ
		  Beep_Run();   //����������ʾ��
		  motor_phase = i;
		  if(y==260)               //timeout       
		  {  i = motor_phase;
		    for(;y!=0;y--)
		      {
			   //P1 = ZHENGRUN[i]&0x0f;    //ȡ����
			  	 P1 &= FANRUN[i];
			     P1 |= FANRUN[i]&0x27;
			    delay(5);                 //����ת��
                i++;	
		        if(i==8) i=0;
		       }	 
		    while(1){Beep_Run();}                   //ֹͣ���������� //���߽���
		  }
       }
	  }
   if(temp_v == 0)
   {
    if(Motor_POS== MOTOR_ARRIVE)   //�����λ��
       { y = 129;
	     i = 0;
		  for(;y!=0;y--)
		      { 
			     P1 &= FANRUN[i];
			     P1 |= FANRUN[i]&0x27;
		        delay(5);                 //����ת��
                i++;	
		        if(i==8) i=0;
		       }
		//motor_phase = i;  //������λ
		Beep_Run();
       }
    }
   /////////////////////////////////////////////////
  }
 }*/