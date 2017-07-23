#include "stm8s.h"
#include "math.h"
#include "stdio.h"

#include "global.h"
#include "key.h"
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
   
    MData.ad_zero_data = 0;
    MData.ad_tare_data = 0;
     
    CalData.cal_start = 0;
    CalData.cal_step = 0;
}
 
///////////////////////////////////////////////
//���ݻ����ͺ� ��ʼ��ϵͳ����������
// �ֱ���
// ����������
// ������������С��ֵ
// ���������С��ֵ
// 
// ����������ֵ
// ��С��ʾ��ֵ
/////////////////////////////////////////////// 
void  Init_MachineParam(void)
{	 
    MachData.dot_pos = 0;
    MachData.weigh_step = 1;
    MachData.weigh_full = 1;
    MachData.ad_full_data = 1;
} 
		 		 
//////////////////////////////////////////////////
//��ʼ�� �û�����1
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
//��ʼ�� �û�����2
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
    InitGlobalVarible();
    Init_SysConfigParam();
    Init_UserConfigParam();
    Init_UserCalParam();
   
    i = 450;
	do {
        if(Flag_10ms) {
		    Flag_10ms = FALSE; 
			i--; 
			if((0X4444) == Get_Key_Code())
				break;			
			}
	} while(i!=0);
		 
    if(0 == i)
	    return(MACHINE_NORMAL_MODE);
     else 
       return(MACHINE_FACTORY_MODE);		 
}

	
///////////////////////////////////////////////////////////
//15 ??????
///////////////////////////////////////////////////////////	
void  Set_Zero_Flag(void)
{
  RunData.do_zero_flag = TRUE;
}
///////////////////////////////////////////////////////////
//16 ??????
///////////////////////////////////////////////////////////
void  Set_Tare_Flag(void)
{
  RunData.do_tare_flag = TRUE;
}


////////////////////////////////////////////////////
/////////////////У׼����		
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
            stable_manual_break();
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
        RunData.current_mode = STAT_WEIGH;  //���ص�����ģʽ
	} else {
       ;
    }				   		 
}


void update_new_data(void)
{
    float  net_tmp;
	u32 weight1,weight2,weight_tmp;

    weight_tmp = Get_Filter_Data();
    //
    if(1 == CalData.cal_start) {
        Cal_Proc(weight_tmp);     //��У׼ 
    }	
    //ȥƤ״̬�²������Զ����� 
    if((1==FilterData.zero_track_enable)&&(0==MData.ad_tare_data))
        auto_zer0(weight_tmp);              //�޸�zer0_data
 /*    
     //////////////////////////////////////////������0����
     if((TRUE == power_on_flag)&&(TRUE==stable_flag))
     {
        power_on_clr_cnt--;
        if(0 == power_on_clr_cnt)
				 {
					power_on_flag = FALSE;
		 	    if(abs(weight_tmp-zer0_data) < full_code/START_ZER0_MAX) //������0 ��Χ
             {
              zer0_data = weight_tmp;          //??????
             }
           /////////////////////////////
           manual_zer0_data = zer0_data;      //??????????,???????????             
          } 
      }    
     //////////////////////////////////////////////////////////	
		 ///////////////////////////////////////�ֶ���0
     //if((TRUE == do_zer0_flag)&&(TRUE==stable_flag))
		 if(TRUE == do_zer0_flag)	 
      {
       do_zer0_flag = FALSE;
       if(abs(weight_tmp-manual_zer0_data)<((full_code*USER_ZER0_MAX)/100)) 
          {
           zer0_data = weight_tmp;
           tare_data = 0;
          }                   
      }       
     ////////////////////////////////����ȥƤ���� ȥƤ���Զ�����ʧЧ
     //if((TRUE == do_tare_flag)&&(weight_tmp>zer0_data)&&(TRUE==stable_flag))
     if((TRUE == do_tare_flag)&&(weight_tmp>zer0_data))
  		{
       do_tare_flag = FALSE;
       tare_data  = new_data;
      }
  
     weight2 = abs(weight_tmp - zer0_data);    //�õ�ë�� ����

		 if(1 == flag_load_track_enable)
      new_data = stable_load_zer0(weight2);    //�ȶ����ٹ���
     else
      new_data = weight2; 
     ///////////////////////////////
		 if(TRUE == point2_cal_ok)               //???????
      {
		    gross_weight  = weigh_coef1  * new_data;    //ë��
        tare_weight   = weigh_coef1  * tare_data;   //ȥƤ��   
    
        if(weight_tmp < zer0_data) 
          {
           positive_flag  = FALSE;
           net_tmp     = gross_weight + tare_weight ;
          }
        else
          {
           if(gross_weight < tare_weight)
             {
              positive_flag = FALSE;
              net_tmp     = tare_weight - gross_weight ;
             }
           else        
             {
              positive_flag = TRUE;
              net_tmp = gross_weight - tare_weight  ;
             }
          }
         ////////////////////////////////////����λ ����
				 if(QIAN_DIVISION == dot_position)	
				  net_tmp += 0.0005;
         else
					net_tmp += 0.00005;
			
				 net_weight = display_to_int(net_tmp);
					
				 ////////////////////////////////////�ظ��Լ�� ���� ��ʱ�� �ȶ����ٴ���
				 //net_weight = repeat_funct_proc(net_weight);	
					
         if(net_weight < display_min_value)
            net_weight = 0.0;
				 /////////////////////////////////////////////���ݾ��� ��ë�صõ��Ƿ����
				 if((net_weight>weight_alarm)||(gross_weight>weight_alarm))
            full_flag = TRUE;
				 else
					  full_flag = FALSE;
       }
    else
      {
       gross_weight   = new_data;  
       net_weight     = new_data;
      }
    */
   }   

