#ifndef __KEY_H__
#define __KEY_H__
#include "stm8s.h"

#define KEY_NUM                          6       //6������
//2ms ɨ��һ�ΰ���
#define KEY_PRESS_TIME                  25       //��������ʱ�䱻�Ͽ�
#define CLICK_TIME                     150       //�����������ʱ������
#define KEY_3S_TIME                   1500       //����3S

//����ʶ����
//ԭ����
#define KEY_ESC           4
#define KEY_SAVE          5
#define KEY_MODE          0
#define KEY_UP            2
#define KEY_DOWN          1
#define KEY_ENTER         3
/*/�������ĵ� 
#define KEY_ESC           1
#define KEY_SAVE          0
#define KEY_MODE          5
#define KEY_UP            4
#define KEY_DOWN          3
#define KEY_ENTER         2
*/

//����״̬��־  2ms*
#define KEY_PRESSED          0x10
#define KEY_PRESSED_3S       0x20
//������

#define BEEP_PIN       GPIO_PIN_4

 void Beep_Start(void);
 void Beep_Stop(void);
 void Beep_Pin_Init(void);
 void Key_Pin_Init(void);
 
 void Key_Scan(void);
 u8 Get_Key_Code(void);
#endif