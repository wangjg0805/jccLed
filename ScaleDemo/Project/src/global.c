#include "stm8s.h"
#include "math.h"
#include "stdio.h"

#include "global.h"
#include "key.h"
#include "ad_proc.h"
#include "ad_filter.h"

#include "I2C.h"


u8 Flag_10ms,Flag_100ms,Flag_500ms,Flag_30s;
u8 display_buffer[16];
u8 RS232_buf[16];

MachineData MachData; 
MeasureData MData;
RuningData RunData;
FilterProcData FilterData;
CalProcData CalData;

 ///////////////////////////////////////////
void delay(u32 length)
    {
    while (length >0)
        length--;
    }


//////////////////////////////////////////////////
//1
///////////////////////////////////////////////////
void InitGlobalVarible(void)
{
    //
	RunData.power_on_flag = 1;
    RunData.stable_flag = 0;
	RunData.full_flag = 0;
    RunData.key_sound_time = 0;
    RunData.current_mode = STAT_WEIGH;
    RunData.current_unit = UNIT_KG;
    RunData.do_zero_flag = 0;
    RunData.do_tare_flag = 0;
    RunData.no_key_time = 0;
    RunData.price_data = 0;
    
    MData.ad_zero_data = 0;
    MData.ad_tare_data = 0;
     
    CalData.cal_start = 0;
    CalData.cal_step = 0;
}
 
///////////////////////////////////////////////
//根据机器型号 初始化系统变量包含了
// 分辨率
// 满量程重量
// 满量程内码最小数值
// 零点内码最小数值
// 
// 四舍五入数值
// 最小显示数值
/////////////////////////////////////////////// 
void  Init_MachineParam(void)
{	 
    MachData.dot_pos = 0;
    MachData.weigh_step = 1;
    MachData.weigh_full = 1;
    MachData.ad_full_data = 1;
} 
		 		 
//////////////////////////////////////////////////
//初始化 用户参数1
///////////////////////////////////////////////////	 
u8 Init_SysConfigParam(void) 
{  
    u32 i,j;
	u8  buf[8];	 
    Read_EEPROM(EEP_SYS_FULL_ADDR, buf, 8);
    //first param is fullvalue
    //next is division

}

//////////////////////////////////////////////////
//初始化 用户参数2
///////////////////////////////////////////////////
void Init_UserConfigParam(void)
{
    u32 i,j;
	u8 buf[16];	
 	Read_EEPROM(EEP_USR_FUN1_ADDR, buf, 16);

	 
}
//////////////////////////////////////////////////
//
//////////////////////////////////////////////////
void Init_UserCalParam(void)
{
    u32 i,j;
	u8 buf[16];	
 	
    Read_EEPROM(EEP_USR_FUN1_ADDR, buf, 16);
}	  

///////////////////////////////////////////////////
u8  System_Init(void)
{   
    u16 i,j; 
    u16 key_buf[6] = {0x00,0x00,0x00,0x00,0x00};
    u8 cnt;
    cnt = 0;
    
    InitGlobalVarible();
    Init_SysConfigParam();
    Init_UserConfigParam();
    Init_UserCalParam();
   
    i = 600;
	do {
        if(Flag_10ms) {
		    Flag_10ms = FALSE; 
			i--;
            j = Key_GetCode();
            if(j!=0) {
                key_buf[cnt++] = j;
                if(cnt == 6)
                    cnt = 0;
            //check password
            if((key_buf[0] == (KEY_0+KEY_PRESSED))&&
               (key_buf[1] == (KEY_1+KEY_PRESSED))&&
               (key_buf[2] == (KEY_2+KEY_PRESSED))&&
               (key_buf[3] == (KEY_3+KEY_PRESSED))&&
               (key_buf[4] == (KEY_4+KEY_PRESSED)))
                break;
            }
        }
	} while(i!=0);
		 
    if(0 == i)
	    return(MACHINE_NORMAL_MODE);
     else 
       return(MACHINE_FACTORY_MODE);		 
}

	
////////////////////////////////////////////////////
/////////////////校准函数		
void  Cal_Proc(u32 weight_tmp)
{
    static u32 tmp;
    u32 i;
    u8  buf[8];
    if(1 == CalData.cal_step) {
	    if(1 == RunData.stable_flag) {
		    MData.ad_zero_data = weight_tmp;
            MData.ad_tare_data = 0;
            //save
            buf[0] = (u8)( weight_tmp>>24);
  	        buf[1] = (u8)((weight_tmp>>16)&0xff);
  	        buf[2] = (u8)((weight_tmp>>8)&0xff);
  	        buf[3] = (u8)( weight_tmp&0xff); 
  	        buf[4] = buf[0];
  	        buf[5] = buf[1];
  	        buf[6] = buf[2];
  	        buf[7] = buf[3]; 
  	        Write_EEPROM(EEP_WEIGHTZERO_ADDR, buf, 8);	 //?????????
            Filter_Init();  //stable status is destoried
	        CalData.cal_step++;
		}
    } else if(2 == CalData.cal_step) {
		if(weight_tmp>(MData.ad_zero_data + 10000))
			CalData.cal_step++;	
    } else if(3 == CalData.cal_step) {     
		if(TRUE==RunData.stable_flag) {
            MachData.ad_full_data = weight_tmp - MData.ad_zero_data;
            MachData.weigh_coef =  MachData.weigh_full / MachData.ad_full_data;		
			FilterData.ad_filter_para = MachData.ad_full_data/MachData.ad_full_data;
 		    //
            buf[0] = (u8)(MachData.ad_full_data>>24);
  	        buf[1] = (u8)((MachData.ad_full_data>>16)&0xff);
  	        buf[2] = (u8)((MachData.ad_full_data>>8)&0xff);
  	        buf[3] = (u8)(MachData.ad_full_data&0xff);
  	        buf[4] = buf[0];
  	        buf[5] = buf[1];
  	        buf[6] = buf[2];
  	        buf[7] = buf[3]; 
            
            Write_EEPROM(EEP_WEIGHTFULL_ADDR, buf, 8);
  	        CalData.cal_step++;
        }
    } else if(4 == CalData.cal_step) {     
	    CalData.cal_start = 0;	
		CalData.cal_step = 0;
        RunData.current_mode = STAT_WEIGH;  //返回到称重模式
	} else {
       ;
    }				   		 
}

void update_new_data(void)
{
	u32 grossw_code;

    if(1 == CalData.cal_start) {
        Cal_Proc(MData.ad_dat_avg); 
    }	
  
    //do zero proc when power_on
    if((1 == RunData.power_on_flag)&&(1 == RunData.stable_flag)) {
        RunData.power_on_cnt--;
        if(0 == RunData.power_on_cnt) {
            RunData.power_on_flag = FALSE;
		 	if(abs(MData.ad_dat_avg-MData.ad_zero_data) < (MachData.ad_full_data*POWER_ON_ZERO_RANGE/100)) {
                MData.ad_zero_data = MData.ad_dat_avg;              
            }
            /////////////////////////////
            MData.power_on_zero_data = MData.ad_zero_data;      //??????????,???????????             
        } 
    }
    
    if((1==FilterData.zero_track_enable)&&(0==MData.ad_tare_data)&&(0==RunData.power_on_flag)) //auto zero 
        auto_zero_proc();      //auto track proc
      
    if((1 == RunData.do_zero_flag)&&(1==RunData.stable_flag)) {
        RunData.do_zero_flag = 0;
        do_zero_proc();
    }
    if((1 == RunData.do_tare_flag)&&(1==RunData.stable_flag)) {
        RunData.do_tare_flag = 0;
        do_tare_proc();
    }
/*    
    grossw_code = abs(MData.ad_dat_avg - MData.ad_zer0_data);    //得到毛重 内码
    if(1 == flag_load_track_enable)
        grossw_code = load_zero_proc(grossw_code);    //Load track

    ///////////////////////////////
    if((0!=MachData.weigh_coef)||(1!=MachData.weigh_coef))
        MData.grossw = grossw_code * MachData.weigh_coef; 
        MData.netw = (grossw_code - MData.ad_tare_data) * MachData.weigh_coef;
        if(MData.ad_dat_avg < MData.ad_zero_data) {
            RunData.positive_flag  = 0;
            //net_tmp = gross_weight + tare_weight ;
        } else {
            if(gross_weight < tare_weight) {
                positive_flag = FALSE;
                net_tmp     = tare_weight - gross_weight ;
            } else {
                positive_flag = TRUE;
                net_tmp = gross_weight - tare_weight  ;
             }
        }
    //
    if(net_weight < display_min_value)
        net_weight = 0.0;
    //根据净重 和毛重得到是否溢出
	if((net_weight>weight_alarm)||(gross_weight>weight_alarm))
        full_flag = TRUE;
	else
	    full_flag = FALSE;
    
    */
}
 