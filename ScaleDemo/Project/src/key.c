#include "stm8s.h"
#include "global.h"
#include "key.h"

static u16 key_press_time[4][4];    //记录每个按键按下的时间

void ScanDriveLine(u8 index)
{
    switch(index) 
    {
    case 0:
        GPIO_WriteLow(GPIOB,   GPIO_PIN_0);
        GPIO_WriteHigh(GPIOB,  GPIO_PIN_1);
        GPIO_WriteHigh(GPIOB,  GPIO_PIN_2);
        GPIO_WriteHigh(GPIOB,  GPIO_PIN_3);
        break;            
    case 1:
        GPIO_WriteHigh(GPIOB,  GPIO_PIN_0);
        GPIO_WriteLow(GPIOB,   GPIO_PIN_1);
        GPIO_WriteHigh(GPIOB,  GPIO_PIN_2);
        GPIO_WriteHigh(GPIOB,  GPIO_PIN_3);
        break;            

    case 2:
        GPIO_WriteHigh(GPIOB,  GPIO_PIN_0);
        GPIO_WriteHigh(GPIOB,  GPIO_PIN_1);
        GPIO_WriteLow(GPIOB,   GPIO_PIN_2);
        GPIO_WriteHigh(GPIOB,  GPIO_PIN_3);
        break;            

    case 3:
        GPIO_WriteHigh(GPIOB,  GPIO_PIN_0);
        GPIO_WriteHigh(GPIOB,  GPIO_PIN_1);
        GPIO_WriteHigh(GPIOB,  GPIO_PIN_2);
        GPIO_WriteLow(GPIOB,   GPIO_PIN_3);
        break;            

    default:break;
    }
}
////////////////////////////
//键盘管脚初始化
///////////////////////////
void Key_Init(void)
 {  //6个按键
    u8 i,j;
    GPIO_Init(GPIOB,GPIO_PIN_0,GPIO_MODE_OUT_PP_LOW_FAST);
    GPIO_Init(GPIOB,GPIO_PIN_1,GPIO_MODE_OUT_PP_LOW_FAST);
    GPIO_Init(GPIOB,GPIO_PIN_2,GPIO_MODE_OUT_PP_LOW_FAST);
    GPIO_Init(GPIOB,GPIO_PIN_3,GPIO_MODE_OUT_PP_LOW_FAST);
 
    GPIO_Init(GPIOC,GPIO_PIN_1,GPIO_MODE_IN_PU_NO_IT);
    GPIO_Init(GPIOC,GPIO_PIN_2,GPIO_MODE_IN_PU_NO_IT);
    GPIO_Init(GPIOC,GPIO_PIN_3,GPIO_MODE_IN_PU_NO_IT);
    GPIO_Init(GPIOC,GPIO_PIN_4,GPIO_MODE_IN_PU_NO_IT);
    
    for(i=0;i<4;i++) {
        for(j=0;j<4;j++)
            key_press_time[i][j] = 0;
    }
    
 }

//**********************************************************************
//函数  名:Key_Scan
//函数功能:
//入口参数:无
//出口参数:无
//说    明:每2ms扫描一次
//**********************************************************************

void  Key_Scan(void)
{
    u8 i,j,key_tmp;
    
    for(i=0;i<4;i++) {
        ScanDriveLine(i);
        key_tmp = (GPIO_ReadInputData(GPIOC) >> 1) & 0x0f; //某位为0表示有按键
        if(0x0f == key_tmp) {
            for(j=0; j<4; j++)
                key_press_time[i][j] = 0;  //清零
        } else {
            //有键按下
            switch((~key_tmp)&0x0f) {
            case 0x01:  key_press_time[i][0]++; break;
            case 0x02:  key_press_time[i][1]++; break;
            case 0x04:  key_press_time[i][2]++; break;
            case 0x08:  key_press_time[i][3]++; break;
            }
        }
    }
}
/****************************************************************************
* 名称：Get_KeyCode()
* 功能：读键码
* 入口参数：无
* 出口参数：低8位键码(0~15), 高8位动作(0 未按,1 按下,2 按下2s, 3 按下5s)
****************************************************************************/
u16 Key_GetCode(void)
{
    u8 i,j;
    u16 Key_CodeTmp;
    Key_CodeTmp = 0;
    
    for(i=0;i<4;i++) {
        for(j=0;j<4;j++){
            if(key_press_time[i][j] > KEY_PRESS_TIME_3S) {
                Key_CodeTmp = KEY_PRESSED_3S+((i+1)*4 + (j+1));
            }
        }
    }
    if(0 == Key_CodeTmp) {
        for(i=0;i<4;i++) {
            for(j=0;j<4;j++){
                if(key_press_time[i][j] > KEY_PRESS_TIME) {
                    Key_CodeTmp = KEY_PRESSED+((i+1)*4 + (j+1));
                }
            }
        }
    }
    
    return(Key_CodeTmp);
}

