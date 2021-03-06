#ifndef __LED_H__
#define __LED_H__
#include "stm8s.h"
#include "global.h"
//----------------------------------------------------------------------------   
//取前三位   
#define COMMAND 0x80   
#define READ    0xC0   
#define WRITE   0xA0   
//----------------------------------------------------------------------------   
//发送完后，补发两位XX，可以是0，也可以是1   
#define LCD_OFF 0x02   
#define LCD_ON  0x03   
#define WDT_DIS 0x05   
#define TON_OFF 0x08   
#define IRQ_DIS 0x80   
#define Rosc    0x18   
#define XTAL    0x18//0x14   
#define BIS1_2  0x29//0x10   
#define SYS_EN  0x01   
#define SYS_DIS 0x00   
#define NORMAL  0xE3 

//定义HT1621 管教
#define CSL     GPIO_WriteLow(GPIOC,  GPIO_PIN_6)
#define CSH     GPIO_WriteHigh(GPIOC, GPIO_PIN_6)
#define WRL     GPIO_WriteLow(GPIOD,  GPIO_PIN_0)
#define WRH     GPIO_WriteHigh(GPIOD, GPIO_PIN_0)
#define DATAL   GPIO_WriteLow(GPIOC,  GPIO_PIN_7)
#define DATAH   GPIO_WriteHigh(GPIOC, GPIO_PIN_7)

#define SEG_A 0x80
#define SEG_B 0x40
#define SEG_C 0x20
#define SEG_D 0x01
#define SEG_E 0x02
#define SEG_F 0x08
#define SEG_G 0x04
#define SEG_P 0x10

//extern area
extern void HT1621_Init(void);
extern void DIS_Update(void);

extern void DIS_On_All(void);
extern void DIS_Off_All(void);
extern void DIS_SpecialChar(u8 data);
extern void DIS_Area1(u32 data,u8 dot);
extern void DIS_Area2(u32 data,u8 dot);
extern void DIS_Area3(u32 data,u8 dot);

#endif