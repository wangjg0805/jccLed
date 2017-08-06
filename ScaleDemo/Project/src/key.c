#include "stm8s.h"
#include "global.h"
#include "key.h"

static u16 key_press_time[4][4];    //记录每个按键按下的时间
static u8 key_released_flag = 0;
static u8 key_read_flag = 1;

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
    u8 i,j,k,key_tmp;
   
    k = 0;
    for(i=0;i<4;i++) {
        ScanDriveLine(i);
        key_tmp = (GPIO_ReadInputData(GPIOC) >> 1) & 0x0f; //某位为0表示有按键
        if(0x0f == key_tmp) {
            if(1 == key_read_flag) {
                for(j=0; j<4; j++)
                    key_press_time[i][j] = 0;  //清零
            }
            k++;
        } else {
            //有键按下
            switch((~key_tmp)&0x0f) {
            case 0x01:  key_press_time[i][0]++;  break;
            case 0x02:  key_press_time[i][1]++; break;
            case 0x04:  key_press_time[i][2]++; break;
            case 0x08:  key_press_time[i][3]++; break;
            }
            key_read_flag = 0;
        }
    }
    
    if(k == 4)
        key_released_flag = 1;
    else
        key_released_flag = 0;
    
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
    
    if((key_read_flag == 0) && (key_released_flag == 1)) 
    {        
    for(i=0;i<4;i++) {
        for(j=0;j<4;j++){
            if(key_press_time[i][j] > KEY_PRESS_TIME_3S) {
                key_press_time[i][j] = 0;
                Key_CodeTmp = KEY_PRESSED_3S+((i+1)*4 + (j+1));
            }
        }
    }
    if(0 == Key_CodeTmp) {
        for(i=0;i<4;i++) {
            for(j=0;j<4;j++){
                if(key_press_time[i][j] > KEY_PRESS_TIME) {
                    key_press_time[i][j] = 0;
                    Key_CodeTmp = KEY_PRESSED+((i+1)*4 + (j+1));
                }
            }
        }
    }
    
    key_read_flag = 1;
   }
   
    return(Key_CodeTmp);
}

u8 KeyCodeToNum(u8 keycode)
{
    u8 i;
    switch(keycode) {
    case KEY_0: i = 0; break;
    case KEY_1: i = 1; break;
    case KEY_2: i = 2; break;
    case KEY_3: i = 3; break;
    case KEY_4: i = 4; break;
    case KEY_5: i = 5; break;
    case KEY_6: i = 6; break;

    case KEY_7: i = 7; break;
    case KEY_8: i = 8; break;
    case KEY_9: i = 9; break;
    default:    i = 0; break;
    }
    return(i);
}

void Key_Proc(u16 key)
{

    switch(key)
    {
    case KEY_PRESSED+KEY_0:
    case KEY_PRESSED+KEY_1:
    case KEY_PRESSED+KEY_2:
    case KEY_PRESSED+KEY_3:
    case KEY_PRESSED+KEY_4:
    case KEY_PRESSED+KEY_5:
    case KEY_PRESSED+KEY_6:
    case KEY_PRESSED+KEY_7:
    case KEY_PRESSED+KEY_8:
    case KEY_PRESSED+KEY_9:
        Key_NumberProc(KeyCodeToNum(key - KEY_PRESSED));
        break;        
    case KEY_PRESSED+KEY_DOT:
        Key_DotProc();
        break;
    case KEY_PRESSED+KEY_TARE:
        Key_TareProc();
        break;
    case KEY_PRESSED+KEY_ZERO:
        Key_ZeroProc();
        break;        
    case KEY_PRESSED+KEY_TOTAL:
        break;
    case KEY_PRESSED+KEY_CLEAR:
        break; 
    default:
        break;
    }
}