#include "stm8s_conf.h"

#include "stdio.h" 
#include "global.h"

#include "timer1.h"

#include "ht1621.h" 
#include "CS1231.h"
#include "I2C.h"
#include "key.h"

//
////////////////////////////////////////////
void Init_HSE(void)
{
    CLK_DeInit(); 
    CLK_HSECmd(ENABLE);
    //while(SET != CLK_GetFlagStatus(CLK_FLAG_HSERDY));
    //CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
    //CLK_ClockSwitchCmd(ENABLE); 
    //CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO,CLK_SOURCE_HSE,DISABLE,CLK_CURRENTCLOCKSTATE_ENABLE);
}

void Init_HSI(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_CPUDIV1);
    
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
 
    Init_HSI();
  
    LedCpu_Init();
    
    BEEP_Init(BEEP_FREQUENCY_2KHZ);
    Key_Init();
    CS1231_Init();
    Filter_Init();
    ////////////////////////////ii2c init
    I2c_Init();
    ////////////////////////////ht1621 init 
    HT1621_Init();
    TIM2_Init();
    ////////////////////////////RS232 init 
    UART2_INIT();
    enableInterrupts();
   /*
    while(1){
         if(1 == Flag_500ms) {
            Flag_500ms = 0; 
            LedCpu_Reverse();
         
         }
    }
    */
    /////////////////////////////////////////////////////////////////////////			
    MachData.mode = System_Init();
    SystemBootDisplay_Init(); //optional

    if((MACHINE_NORMAL_MODE+MACHINE_CAL_MODE) == MachData.mode) {
        CalData.calstart = 1;
        CalData.calstep = 1;
    } else if((MACHINE_NORMAL_MODE+MACHINE_CAL_MODE) == MachData.mode) {
        FactoryData.factorystart = 1;
        FactoryData.factorystep = 1;
        FactoryData.factoryindex = 1;
    }
    
    if(1)
        Normal_Pro();
    
	while(1){;}
}  
 