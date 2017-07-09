#include "stm8s.h"
#include "stm8s_uart2.h" 
#include "global.h" 
////////////////////////////////////////////
//uart2 application
////////////////////////////////////////////
void UART2_INIT(void)
 {
  UART2_DeInit();
  UART2_Init((u32)9600, UART2_WORDLENGTH_8D, UART2_STOPBITS_1, UART2_PARITY_NO, UART2_SYNCMODE_CLOCK_DISABLE, UART2_MODE_TX_ENABLE);
  UART2_ITConfig(UART2_IT_RXNE, ENABLE);
  UART2_Cmd(ENABLE);
 }
////////////////////////////////////////////
//
////////////////////////////////////////////
void UART2_SendByte(char data)
{
  while (UART2_GetFlagStatus(UART2_FLAG_TXE) == RESET);  
  UART2_SendData8((unsigned char)data);
  /* Loop until the end of transmission */
}
////////////////////////////////////////////
//
////////////////////////////////////////////
void UART2_SendString(char* Data,u8 len)
{
  u8 i=0;
  for(;i<len;i++)
    UART2_SendByte(Data[i]);
}
////////////////////////////////////////////
//print init
////////////////////////////////////////////
void  Printer_Init(void) 
 {
   char  OutString[8];
        
        OutString[0] =  0x1b;
        OutString[1] =  0x40;
        OutString[2] =  0x1b;
        OutString[3] =  0x63;
        OutString[4] =  0x00;
        UART2_SendString(OutString,5);
 }
//////////////////////////////////
void  Send_RS232(u16 ad) 
 {
   char  OutString[8];
   /*  OutString[0] = 0x20;
     OutString[1] = 0x20;
     OutString[2] = '%';
     OutString[3] = 'T';
     OutString[4] = 0x0d;
     OutString[5] = 0x0a;*/
    //使用PRINTER
     UART2_SendString(Send_buf,13);
 }
//USE PC
void  Send_RS232_PC(u16 ad) 
 {
   char  OutString[9];
   //PC
   //把send_buf 重新组装
   //UART2_SendString(Send_buf,13);
   if('s' == Send_buf[10])
     {
      OutString[0] = 'A';
      OutString[1] = '=';
      OutString[2] = Send_buf[0];
      OutString[3] = Send_buf[1];
      OutString[4] = Send_buf[2];
      OutString[5] = Send_buf[3];
      OutString[6] = Send_buf[4];
      OutString[7] = Send_buf[5];
      OutString[8] = 0x0d;
      UART2_SendString(OutString,9);
     }
   else if('T' == Send_buf[10])
     {
      OutString[0] = 'T'; 
      OutString[1] = '=';
      OutString[2] = Send_buf[0];
      OutString[3] = Send_buf[1];
      OutString[4] = Send_buf[2];
      OutString[5] = Send_buf[3];
      OutString[6] = Send_buf[4];
      OutString[7] = Send_buf[5];
      OutString[8] = 0x0d;
      UART2_SendString(OutString,9);
     }
   else
     {
      OutString[0] = 'C';
      OutString[1] = '=';
      OutString[2] = Send_buf[1];
      OutString[3] = Send_buf[2];
      OutString[4] = Send_buf[3];
      OutString[5] = Send_buf[4];
      OutString[6] = Send_buf[5];
      OutString[7] = 0x0d;
      UART2_SendString(OutString,8);
     } 
 }