#include "stm8s.h"
#include "stdio.h" 
#include "global.h"

#include "timer1.h"

#include "ht1621.h" 
#include "CS1231.h"
#include "I2C.h"
#include "key.h"

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
//主函数入口
/////////////////////////////

void main(void)       
{
    u32 ad_tmp;
    u32 cycle_times = 0;
    u32 cycle_times1 = 0;
    u32  i;
    //use HSEclk
    //HSE_CLK_INIT();
    
    u8 buf[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
    
    HSI_CLK_INIT();
  
    LedCpu_Init();
    Speaker_Init();
    Key_Init();
    CS1231_Init();
    ////////////////////////////ii2c init
    I2c_Init();
    ////////////////////////////ht1621 init 
    HT1621_Init();
    TIM2_Init();
    ////////////////////////////RS232 init 
    UART2_INIT();
    enableInterrupts();

/*
    Write_EEPROM(0x10,buf,0x08);
    Write_EEPROM(0x20,&buf[8],0x08);
    for(i=0;i<16;i++)
        buf[i] = 0;
*/
    
    while(1){
        
        if(Flag_10ms) {
            Flag_10ms = 0;
            Key_Scan();
            /////////////////////////////////////////声音处理
            if(0!=RunData.key_sound_time) {
                Speaker_On();
                RunData.key_sound_time--;
                if(RunData.key_sound_time == 0)
                    Speaker_Off();
            }
	    }
        
        if(Flag_100ms) {
            Flag_100ms = 0;
            Display_Boot_Info();
            i = Key_GetCode();
            if(0 != i) {
                Display_Area2(i,2);
                RunData.key_sound_time = 200;
            } else
                Display_Area2(22222,2);
            
            if(1 == CS1231_Read())
                Display_Area3(MData.hx711_data,2);
            else
                Display_Area3(444444,2);
            Update_Display();
        }
        if(0) {
            Flag_500ms = 0;
            Read_EEPROM(0x10,buf,0x08);
            Read_EEPROM(0x20,&buf[8],0x08);
        } 
    }
    

 
    ///////////////////////////系统应用初始化
    filter_init();
    ////////////////////////////////////////
    ////////////////////////////cs5550 init
  //////////////////////////////////////进入主循环
  //打印机初始化
  Printer_Init();
  
  while (1)
    {  
   ;
   }   //while end
}
