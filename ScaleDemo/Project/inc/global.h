//**********************************************************************
//文件名:global.h
//**********************************************************************
#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "stm8s.h"

#define MACHINE_NORMAL_MODE     0x01
#define MACHINE_FACTORY_MODE    0x02
#define MACHINE_CAL_MODE        0x04



#define MACHINE_AD_ZERO         10000
#define MACHINE_FULL_ZERO       100000



#define POWER_ON_ZERO_RANGE     4  //4%


//MACH STAT
typedef enum {
    STAT_WEIGH = 0,
    STAT_CAL,
    STAT_CONFIG
}SYSStatus;

typedef enum {
    UNIT_KG = 0,
    UNIT_G,
    UNIT_LB,
    UNIT_OZ,
}UNITType;


typedef struct{
    u8 mode;
    u8 dot_pos;

    u8 weigh_onestep;
    u32 weigh_fullrange;
    u32 weigh_division;
    
    u8 weigh_calpoint_num;   
    u32 weigh_ad_zero;
    u32 weigh_ad_full;
    u32 weigh_ad_calpoint[10];  //multi point cal
    
    float weigh_coef;
    
}MachineData;

typedef struct{
    u8 displaystep_min;
    u8 powerofftime;
    u8 holdenable;
    u8 peakenable;
    u8 cheatenable;
    u8 rsv1;
    u8 rsv2;
    u8 rsv3;
    u8 rsv4;
    
}UserConfigData;


typedef struct{
    u8 do_tare_flag;
    u8 do_zero_flag;
    u8 full_flag;
    u8 positive_flag;
    u8 stable_flag;
    u8 power_on_flag;
    u8 power_on_cnt;
    u8 key_sound_time;
    SYSStatus current_mode;
    UNITType current_unit;
    u32 no_key_time;
    
    u8 price_dot;
    u32 price_data;
}RuningData;

typedef struct{
    u32 hx711_data;
    u32 ad_dat_avg;
    u32 weigh_ad_zero;
    u32 ad_tare_data;
    u32 power_on_zero_data;  //for manual zero proc
    float grossw;
    float netw;
}MeasureData;
    
typedef struct{
    u8 zero_track_enable;
    u8 zero_track_cnt;
    u8 load_track_enable;
    u8 load_track_cnt;
    u8 ad_filter_para;
}FilterProcData;  
 
typedef struct{
    u8 calstart;
    u8 calstep;
}CalProcData;

typedef struct{
    u8 factorystart;
    u8 factorystep;
    u8 factoryindex;
}FactoryProcData;

///////////////////////////////////////变量声明
extern u8 Flag_10ms,Flag_100ms,Flag_500ms,Flag_30s;
extern u8 display_buffer[16];
extern u8 RS232_buf[16];

extern MachineData MachData; 
extern MeasureData MData;
extern RuningData RunData;
extern FilterProcData FilterData;
extern CalProcData CalData;
extern FactoryProcData FactoryData;
////////////////////////////////////////////
extern void delay(u32 length);
extern void U32toBUF(u32 data,u8* p);
extern u32 BUFtoU32(u8* p);

extern u8 System_Init(void);
extern void SystemBootDisplay_Init(void);

#endif
