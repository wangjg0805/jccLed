//**********************************************************************
//文件名:global.h
//**********************************************************************
#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "stm8s.h"

#define MACHINE_NORMAL_MODE     0
#define MACHINE_FACTORY_MODE    1
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
    u8 dot_pos;
    u8 weigh_step;
    u32 weigh_full;
    u32 weigh_division;
    u32 ad_full_data;
    float weigh_coef;
}MachineData;

typedef struct{
    u8 do_tare_flag;
    u8 do_zero_flag;
    u8 full_flag;
    u8 positive_flag;
    u8 stable_flag;
    u8 power_on_flag;
    u8 key_sound_time;
    SYSStatus current_mode;
    UNITType current_unit;
    u32 no_key_time;
}RuningData;

typedef struct{
    u32 hx711_data;
    u32 ad_dat_avg;
    u32 ad_zero_data;
    u32 ad_tare_data;
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
    u8 cal_start;
    u8 cal_step;
}CalProcData;

///////////////////////////////////////变量声明
extern u8 Flag_10ms,Flag_100ms,Flag_500ms,Flag_30s;
extern u8 display_buffer[16];
extern u8 RS232_buf[16];

extern MachineData MachData; 
extern MeasureData MData;
extern RuningData RunData;
extern FilterProcData FilterData;
extern CalProcData CalData;

////////////////////////////////////////////
extern void delay(u32 length);
#endif
