#include "stm8s_conf.h"

#include "stdio.h" 
#include "global.h"

#include "timer1.h"

#include "ht1621.h" 
#include "CS1231.h"
#include "I2C.h"
#include "key.h"


static void Display_Normal(void)
{
     u32 i,j;
     
     DIS_Area1(MData.grossw,2);
     DIS_Area2(RunData.price_data,2);
     DIS_Area3(MData.grossw * RunData.price_data / 100,3);  
     DIS_Update();

}



void Normal_Pro(void)
{
    u16 i;
    
    while(1){
        if(Flag_10ms) {
            Flag_10ms = 0;
            Key_Scan();
	    }
        if(Flag_100ms) {
            Flag_100ms = 0;
            i = Key_GetCode();
            if(0 != i) {
                switch(MachData.mode) {
                case MACHINE_NORMAL_MODE + MACHINE_FACTORY_MODE:
                    Key_Proc_Factory(i);
                    break;
                case MACHINE_NORMAL_MODE + MACHINE_CAL_MODE:
                    Key_Proc_Cal(i);
                    break;
                case MACHINE_NORMAL_MODE:
                    Key_Proc(i);
                    RunData.no_key_time = 0;
                    break;
                default:
                    break;
                }
                RunData.key_sound_time = 10;
            }
            
            if(1 == CS1231_Read()) {
                ad_filter(MData.hx711_data);
                //
                switch(MachData.mode) {
                case MACHINE_NORMAL_MODE + MACHINE_FACTORY_MODE:
                    MData_update_normal();
                    Display_Factory();
                    break;
                case MACHINE_NORMAL_MODE + MACHINE_CAL_MODE:
                    MData_update_cal();
                    Display_Cal();
                    break;
                case MACHINE_NORMAL_MODE:
                    MData_update_normal();
                    Display_Normal();
                    break;
                default:
                    break;
                }
            }
        }
        
        if(1 == Flag_500ms) {
            Flag_500ms = 0; 
            if(RunData.no_key_time > 70)
                BkLight_Off();
            else
                BkLight_On();
            //BkLight_Reverse();
            //Read_EEPROM(0x10,buf,0x08);
            //Read_EEPROM(0x20,&buf[8],0x08);
        } 
    }  
}




