#include "stm8s.h"
#include "math.h"
#include "stdio.h"

#include "global.h"
#include "key.h"
#include "I2C.h"


static u32 key_numberbuf[5] = {0,0,0,0,0};
static u8 key_numbercnt = 0;

//*************************************************************
void Key_TareProc(void)
{
    RunData.do_tare_flag = 1;

}        
        
void Key_ZeroProc(void)
{
    RunData.do_zero_flag = 1; 
    
}
void Key_TotalProc(void)
{

}
void Key_ClearProc(void)
{

}

void Key_NumberProc(u8 num)
{
    u8 i;
    if(RunData.no_key_time > 50) {
        for(i=0;i<5;i++)
            key_numberbuf[i] = 0;
    } else {
        for(i=0;i<4;i++)
            key_numberbuf[4-i] = key_numberbuf[3-i];
    }

    key_numberbuf[0] = num;
    key_numbercnt++;
    
    RunData.price_data = key_numberbuf[4]*10000+
                         key_numberbuf[3]*1000+
                         key_numberbuf[2]*100+
                         key_numberbuf[1]*10+
                         key_numberbuf[0];
    
}


void Key_DotProc(void)
{
    switch(key_numbercnt) {
    case 0:
        key_numbercnt++;
        Key_NumberProc(0);
        RunData.price_dot = 4;
        break;
    case 1:
        RunData.price_dot = 4;
        break;
    case 2:
        RunData.price_dot = 3;
        break;
    case 3:
        RunData.price_dot = 2;
        break;
    case 4:
        RunData.price_dot = 1;
        break;
    case 5:
        RunData.price_dot = 0;
        break;
    default:break;
    }
}
