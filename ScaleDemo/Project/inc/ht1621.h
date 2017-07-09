#ifndef __LED_H__
#define __LED_H__
#include "stm8s.h"
//----------------------------------------------------------------------------   
//ȡǰ��λ   
#define COMMAND 0x80   
#define READ    0xC0   
#define WRITE   0xA0   
//----------------------------------------------------------------------------   
//������󣬲�����λXX��������0��Ҳ������1   
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

//����HT1621 �ܽ�
#define CSL     GPIO_WriteLow(GPIOC,  GPIO_PIN_6)
#define CSH     GPIO_WriteHigh(GPIOC, GPIO_PIN_6)
#define WRL     GPIO_WriteLow(GPIOD,  GPIO_PIN_0)
#define WRH     GPIO_WriteHigh(GPIOD, GPIO_PIN_0)
#define DATAL   GPIO_WriteLow(GPIOC,  GPIO_PIN_7)
#define DATAH   GPIO_WriteHigh(GPIOC, GPIO_PIN_7)

 void HT1621_Pin_Init(void);
 void Init_1621(void);
 
 void flag_disp_proc(u8 TestMode);
 void display_ad_data(u16 ad_tmp);
 
 void Update_Display_Buf(float InValue);
 void Refresh_DisplayBuf(void);
 void Refresh_Ht1621(void);

 void All_on(void);
 void All_on_Must(void);
 void All_off_Must(void);
 void All_off(void);
 void Display_Err(void);
#endif