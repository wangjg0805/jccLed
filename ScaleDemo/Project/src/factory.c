#include "stm8s_conf.h"

#include "stdio.h" 
#include "global.h"

#include "timer1.h"

#include "ht1621.h" 
#include "CS1231.h"
#include "I2C.h"
#include "key.h"


void Factory_Pro(void)
{
    u16 i;
    
    while(1){
        if(Flag_10ms) {
            Flag_10ms = 0;
            Key_Scan();
            /////////////////////////////////////////…˘“Ù¥¶¿Ì
            if(0!=RunData.key_sound_time) {
                BEEP_Cmd(ENABLE);
                RunData.key_sound_time--;
                if(RunData.key_sound_time == 0)
                    BEEP_Cmd(DISABLE);
            }
	    }
        if(Flag_100ms) {
            Flag_100ms = 0;
            i = Key_GetCode();
            if(0 != i) {
                Key_Proc(i);
                RunData.key_sound_time = 20;
            }
            
            if(1 == CS1231_Read()) {
                ad_filter(MData.hx711_data);
                update_new_data();
                Display_Area3(MData.hx711_data,2);
                MData.grossw = (MData.hx711_data - MData.ad_zero_data) *30000.0 / MachData.ad_full_data; 
                Display_Area1(MData.grossw,2);
            }
            Update_Display();
        }
        
        if(1 == Flag_500ms) {
            Flag_500ms = 0;      
            //Read_EEPROM(0x10,buf,0x08);
            //Read_EEPROM(0x20,&buf[8],0x08);
        } 
    }  
}




