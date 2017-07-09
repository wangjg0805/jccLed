#ifndef __RS232_H__
#define __RS232_H__


#include "stm8s.h"


void UART2_INIT(void);
void UART2_SendString(char* Data,u8 len);
void  Send_RS232(void);
void  Send_RS232_PC(void);
void  Printer_Init(void);

#endif