#include "stm8s.h"
#include "math.h"
#include "stdio.h"

#include "global.h"
#include "key.h"
#include "ad_proc.h"
#include "ad_filter.h"
#include "ht1621.h"
#include "I2C.h"


u8 Flag_10ms,Flag_100ms,Flag_500ms,Flag_30s;
u8 display_buffer[16];
u8 RS232_buf[16];

MachineData MachData; 
MeasureData MData;
RuningData RunData;
FilterProcData FilterData;
CalProcData CalData;
FactoryProcData FactoryData;

 ///////////////////////////////////////////
void delay(u32 length)
    {
    while (length >0)
        length--;
    }

/////////////////////////////////////////声音处理
void Speaker_Proc(void)
{
    if(0!=RunData.key_sound_time) {
        BEEP_Cmd(ENABLE);
        RunData.key_sound_time--;
        if(RunData.key_sound_time == 0)
            BEEP_Cmd(DISABLE);
    }
}
 

void U32toBUF(u32 data,u8* p)
{
    *p = data&0xff;
    *(p+1) = (data>>8)&0xff;
    *(p+2) = (data>>16)&0xff;
    *(p+3) = (data>>24)&0xff;
    
}

u32 BUFtoU32(u8* p)
{
    u32 tmp;
    tmp = (*(p+3)<<24)+(*(p+2)<<16)+(*(p+1)<<8)+ (*p);
    return(tmp);
    
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
    
    MData.weigh_ad_zero = 0;
    MData.ad_tare_data = 0;
     
    CalData.calstart = 0;
    CalData.calstep = 0;

    FactoryData.factorystart = 0;
    FactoryData.factorystep = 0;
    FactoryData.factoryindex = 0;
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
    u32 i,j;
	u8  buf[8];	 
    Read_EEPROM(EEP_SYS_FULL_ADDR, buf, 8);
    //
    MachData.dot_pos = 0;
    MachData.weigh_onestep = 1;
    MachData.weigh_fullrange = 50000;
    MachData.weigh_division = MachData.weigh_fullrange/MachData.weigh_onestep;    
    
    MachData.weigh_calpoint_num = 1;
    MachData.weigh_ad_zero = 10000;
    MachData.weigh_ad_calpoint[0] = 80000; 
    
} 
		 		 
void SystemBootDisplay_Init(void)
{

    u16 i; 
    i = 0;
    do {
        if(1 == Flag_100ms) {
            Flag_100ms = 0;
            i++;
            DIS_Area1(88,2);
            DIS_Area2(99,2);
            DIS_Area3(77,2);
            DIS_Update();
        }
    } while(i!=20);
		 
    RunData.key_sound_time = 20;
}
//////////////////////////////////////////////////
//初始化 用户参数2
///////////////////////////////////////////////////
void Init_UserConfigParam(void)
{
    u32 i,j;
	u8 buf[16];	
    Read_EEPROM(EEP_CALFLAG_ADDR, buf, 2); 
    if((buf[0]==buf[1])&&(buf[0]==0xaa)) {
        Read_EEPROM(EEP_WEIGHTZERO_ADDR, buf, 4);
        MachData.weigh_ad_zero = BUFtoU32(buf);
        Read_EEPROM(EEP_WEIGHTFULL_ADDR, buf, 4);
        MachData.weigh_ad_full = BUFtoU32(buf);
       
    } else {
        MachData.weigh_ad_zero = MACHINE_AD_ZERO;
        MachData.weigh_ad_full = MACHINE_FULL_ZERO;
    }
    
	MachData.weigh_coef = MachData.weigh_fullrange / (MachData.weigh_ad_full + 0.1);
}




///////////////////////////////////////////////////
u8  System_Init(void)
{   
    u16 i,j; 
    u16 key_buf[6] = {0x00,0x00,0x00,0x00,0x00};
    u8 cnt;
    cnt = 0;
    i = 0;
    do {
        if(i == 0)
            InitGlobalVarible();
        if(i == 10)
            Init_MachineParam();
        if(i == 20)
            Init_UserConfigParam();
       

        if(0 == (i%60))
            DIS_SpecialChar(i/60);
       
        if(0 == (i%50))
            LedCpu_Reverse();
        
        if(1 == Flag_10ms) {
            i++;
            Key_Scan();
            Flag_10ms = FALSE; 
        }
        
	    if(1 == Flag_100ms) {
            Flag_100ms = 0;
            j = Key_GetCode();
            if(j!=0) {
                key_buf[cnt++] = j;
                if(cnt == 6)
                    cnt = 0;
            }
        }
	} while(i!=600);
		 
    RunData.key_sound_time = 20;
    
    if((key_buf[0] == (KEY_0+KEY_PRESSED))&&
       (key_buf[1] == (KEY_1+KEY_PRESSED))&&
       (key_buf[2] == (KEY_2+KEY_PRESSED))&&
       (key_buf[3] == (KEY_3+KEY_PRESSED))&&
       (key_buf[4] == (KEY_4+KEY_PRESSED)))
        return(MACHINE_NORMAL_MODE + MACHINE_FACTORY_MODE);

    if((key_buf[0] == (KEY_1+KEY_PRESSED))&&
       (key_buf[1] == (KEY_2+KEY_PRESSED))&&
       (key_buf[2] == (KEY_3+KEY_PRESSED))&&
       (key_buf[3] == (KEY_4+KEY_PRESSED))&&
       (key_buf[4] == (KEY_5+KEY_PRESSED)))
        return(MACHINE_NORMAL_MODE + MACHINE_CAL_MODE);
 
    return(MACHINE_NORMAL_MODE);
}

	
////////////////////////////////////////////////////
/////////////////校准函数		
void  Cal_Proc(u32 weight_tmp)
{
    static u32 tmp;
    u32 i;
    u8  buf[8];
    if(1 == CalData.calstep) {
	    if(1 == RunData.stable_flag) {
		    MData.weigh_ad_zero = weight_tmp;
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
	        CalData.calstep++;
		}
    } else if(2 == CalData.calstep) {
		if(weight_tmp>(MData.weigh_ad_zero + 10000))
			CalData.calstep++;	
    } else if(3 == CalData.calstep) {     
		if(TRUE==RunData.stable_flag) {
            MachData.weigh_ad_full = weight_tmp - MData.weigh_ad_zero;
            MachData.weigh_coef =  MachData.weigh_fullrange / MachData.weigh_ad_full;		
			FilterData.ad_filter_para = MachData.weigh_ad_full/MachData.weigh_ad_full;
 		    //
            buf[0] = (u8)(MachData.weigh_ad_full>>24);
  	        buf[1] = (u8)((MachData.weigh_ad_full>>16)&0xff);
  	        buf[2] = (u8)((MachData.weigh_ad_full>>8)&0xff);
  	        buf[3] = (u8)(MachData.weigh_ad_full&0xff);
  	        buf[4] = buf[0];
  	        buf[5] = buf[1];
  	        buf[6] = buf[2];
  	        buf[7] = buf[3]; 
            
            Write_EEPROM(EEP_WEIGHTFULL_ADDR, buf, 8);
  	        CalData.calstep++;
        }
    } else if(4 == CalData.calstep) {     
	    CalData.calstart = 0;	
		CalData.calstep = 0;
        RunData.current_mode = STAT_WEIGH;  //返回到称重模式
	} else {
       ;
    }				   		 
}

void MData_update_normal(void)
{
	u32 grossw_code;

    //do zero proc when power_on
    if((1 == RunData.power_on_flag)&&(1 == RunData.stable_flag)) {
        RunData.power_on_cnt--;
        if(0 == RunData.power_on_cnt) {
            RunData.power_on_flag = 0;
		 	if(abs(MData.ad_dat_avg-MData.weigh_ad_zero) < (MachData.weigh_ad_full*POWER_ON_ZERO_RANGE/100)) {
                MData.weigh_ad_zero = MData.ad_dat_avg;              
            }
            /////////////////////////////
            MData.power_on_zero_data = MData.weigh_ad_zero;      //??????????,???????????             
        } 
    }
   /* 
    if((1==FilterData.zero_track_enable)&&(0==MData.ad_tare_data)&&(0==RunData.power_on_flag)) //auto zero 
        auto_zero_proc();      //auto track proc
   */   
    if((1 == RunData.do_zero_flag)&&(1==RunData.stable_flag)) {
        RunData.do_zero_flag = 0;
        do_zero_proc();
    }
    if((1 == RunData.do_tare_flag)&&(1==RunData.stable_flag)) {
        RunData.do_tare_flag = 0;
        do_tare_proc();
    }
   
    MData.grossw = (MData.ad_dat_avg - MachData.weigh_ad_zero) * MachData.weigh_coef;
    //MData.netw = (grossw_code - MData.ad_tare_data) * MachData.weigh_coef;
    
    /*
    grossw_code = abs(MData.ad_dat_avg - MData.ad_zer0_data);    //得到毛重 内码
    if(1 == flag_load_track_enable)
        grossw_code = load_zero_proc(grossw_code);    //Load track

    ///////////////////////////////
    if((0!=MachData.weigh_coef)||(1!=MachData.weigh_coef))
        MData.grossw = grossw_code * MachData.weigh_coef; 
        MData.netw = (grossw_code - MData.ad_tare_data) * MachData.weigh_coef;
        if(MData.ad_dat_avg < MData.weigh_ad_zero) {
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
void MData_update_cal(void)
{
	u32 grossw_code;

    //Cal_Proc(MData.ad_dat_avg); 
   
}
 