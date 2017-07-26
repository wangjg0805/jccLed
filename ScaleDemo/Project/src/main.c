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
    HSI_CLK_INIT();
  
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

    /////////////////////////////////////////////////////////////////////////			
    //i = System_Init();
    i = 0xaa;
    if(MACHINE_FACTORY_MODE == i) {
        Factory_Pro();
    } else {
        Normal_Pro();
    }
	while(1){;}
}  
 