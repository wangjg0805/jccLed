//**********************************************************************
//�ļ���:global.h
//**********************************************************************
#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "energy.h"
///////////////////////////////���������Χ
//#define APP_MIN_TRANS  (-1.0)

#define APP_MIN_TRANS  (0.001)
#define APP_MAX_TRANS  (200.0)

#define APP_MIN_ABS  (-0.4)	 //�������С
#define APP_MAX_ABS  (3.0)	 //��������
/*
#define APP_MIN_TRANS  (0.000001)
#define APP_MAX_TRANS     (100.0)
#define APP_MIN_ABS        (-0.4)  //�������С
#define APP_MAX_ABS         (3.0)  //��������
*/

#define APP_MAX_PARA       (9999)  //��������������Сֵ
#define APP_MIN_PARA          (1)

#define APP_MAX_PARA_DISPLAY       (9999)  //��������������Сֵ
#define APP_MIN_PARA_DISPLAY      (-9999)


#define APP_MAX_CONC       (9999)  //���õ�Ũ�Ȳ���
#define APP_MIN_CONC       (-999)

//���ü����Ŵ�ȼ�
#define ENERGY_AMP_TOTAL          8
#define ENERGY_AMP_DEF            5

#define ENERGY_DARK_DEF       1000
#define ENERGY_DARK_MIN          0
#define ENERGY_DARK_MAX       3000

#define ENERGY_FULL_DEF       40000
#define ENERGY_FULL_MIN       40000
#define ENERGY_FULL_MAX       60000

////////////////////////////////////////////////////����EEPROM ADDRESS
//������Ϣ���ڴ洢�û���������
//AT24C01 128Bytes   AT24C08 1024Bytes   AT24C64   8192Bytes   
//AT24C02 256Bytes   AT24C16 2048Bytes   AT24C128 16384Bytes
//AT24C04 512Bytes   AT24C32 4096Bytes   AT24C256 32768Bytes
//���㹫ʽ 2048/4 = 512 - 4  
//ʹ��AT24C16 ���Դ洢���508����¼��Ŀǰʹ�õ�500��
#define EEPROM_REC_TOTAL_ADDR         0x00   // ��ౣ��200����¼
#define EEPROM_REC_TOTAL_ADDR_BAK     0x01  

#define REC_DATA_LENGTH               0x04   //ÿ����¼ռ��4���ֽ�
#define EEPROM_REC_START_ADDR         0x10


//����ģʽ
typedef enum
{
APP_TEST_MODE_TRANS,	        //T
APP_TEST_MODE_ABS,		//A
APP_TEST_MODE_CONC,		//C
APP_TEST_MODE_PARAMETER,        //P
APP_TEST_MODE_RECALL,           //�ؿ�ģʽ ����SAVE�����ģʽ
APP_TEST_MODE_TOTAL,
}APP_TEST_MODE_INDEX;

////////////////������
typedef struct
 {
   u16 DarkEnergy[ENERGY_AMP_TOTAL];  //ÿ���Ŵ����µ�ADֵ
   //u16 DarkEnergy;                     //�������µ�AD��ֵ
   u16 FullEnergy;		      //��ĳ���̶��Ŵ����µ�AD��ֵ
    u8 EnergyAmp;
   u16 Conc;
   float AbsData;
   float ConcK;
   u16 ParaK;
   u8 TestMode;
   u16 RecordID;
  }APP_CONFIG_T;

///////////////////////////////////////��������
extern            u8 Key_Sound_Time;

extern            APP_CONFIG_T APP_Configuration; 
extern            float TransP1,AbsP2,ConcP3,ParaP4;
extern            char  Send_buf[13], neg_flag;
extern            u8 dot_position;
extern            u8 Cur_Display_Dynamic;    //ָʾ��ǰ���޸Ľ׶λ�������׶Σ�Ĭ��������׶Σ�ֻ�а������¼����Ϊ���Ľ׶�
extern           u16 Conc_tmp;
extern           u16 ParaK_tmp;
extern            u8 flag_MM,MM_display_time;
extern           u16 rec_total,current_display_rec;

//������
void APP_Init(void);

void  APP_GetFormatString(float InValue,char *OutString);
void  Data_Cal(u16 ad_data);
void  Data_Cal_Debug(u16 ad_data);

//////////////////////////
void  Key_Mode_Pro(void);
void  Key_Down_Pro(void);
void  Key_Up_Pro(void);
void  Key_Enter_Pro(void);
void  Key_Save_Pro(void);
void  Key_ESC_Pro(void);
/////////////��������
void  Key_Up_Fast(void);
void  Key_Down_Fast(void);

//rec 
void  Key_Recall_Pro(void);
void  Key_Rec_Clear_Pro(void);
void  Search_Up_Recorder(void);
void  Search_Next_Recorder(void);

#endif
