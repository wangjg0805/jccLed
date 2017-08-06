#include "stm8s.h"
#include "global.h"

void LedCpu_Init(void)
 {  
   GPIO_Init(GPIOF,GPIO_PIN_4,GPIO_MODE_OUT_PP_LOW_FAST);
   GPIO_Init(GPIOE,GPIO_PIN_5,GPIO_MODE_OUT_PP_LOW_FAST);
 }

void LedCpu_Reverse(void)
 {  
    GPIO_WriteReverse(GPIOF,  GPIO_PIN_4);
 }

void LedCpu_On(void)
{      
    GPIO_WriteHigh(GPIOF,  GPIO_PIN_4);
}

void LedCpu_Off(void)
 {  
    GPIO_WriteLow(GPIOF,  GPIO_PIN_4);
 }

////////////////////////////
//·äÃùÆ÷¹Ü½Å³õÊ¼»¯
///////////////////////////

void BkLight_Reverse(void)
 {  
    GPIO_WriteReverse(GPIOE,  GPIO_PIN_5);
 }

void BkLight_On(void)
 {
    GPIO_WriteHigh(GPIOE, GPIO_PIN_5); 
   
 }
void BkLight_Off(void)
 {
    GPIO_WriteLow(GPIOE, GPIO_PIN_5); 
 }

