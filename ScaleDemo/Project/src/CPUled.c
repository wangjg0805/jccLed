#include "stm8s.h"
#include "global.h"

void LedCpu_Init(void)
 {  
   GPIO_Init(GPIOF,GPIO_PIN_4,GPIO_MODE_OUT_PP_LOW_FAST);
   GPIO_Init(GPIOD,GPIO_PIN_4,GPIO_MODE_OUT_PP_LOW_FAST);
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
void Speaker_On(void)
 {
   GPIO_WriteHigh(GPIOD, GPIO_PIN_4); 
   
 }
void Speaker_Off(void)
 {
   GPIO_WriteLow(GPIOD, GPIO_PIN_4); 
 }

void Speaker_Init(void)
 {
   GPIO_Init(GPIOD,GPIO_PIN_4,GPIO_MODE_OUT_PP_LOW_FAST);
   Speaker_Off(); 
 }


