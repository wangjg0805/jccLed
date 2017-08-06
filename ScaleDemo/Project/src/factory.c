#include "stm8s_conf.h"

#include "stdio.h" 
#include "global.h"

#include "timer1.h"

#include "ht1621.h" 
#include "CS1231.h"
#include "I2C.h"
#include "key.h"


//key proc when cal

void Key_CalTareProc(void)
{
    u8 buf[8];
    
    if(CalData.calstep == 1) {
        CalData.calstep++;
        MachData.weigh_ad_zero = MData.ad_dat_avg;
        U32toBUF(MachData.weigh_ad_zero,buf);
        buf[4] = buf[0];
        buf[5] = buf[1];
        buf[6] = buf[2];
        buf[7] = buf[3];
        Write_EEPROM(EEP_WEIGHTZERO_ADDR,buf,8);

        buf[0] = 0x55;
        buf[1] = 0x55;        
        Write_EEPROM(EEP_CALFLAG_ADDR,buf,2);  //erase caldata
        
    } else if(CalData.calstep == 2) {
        CalData.calstep++;
        MachData.weigh_ad_full = MData.ad_dat_avg -  MachData.weigh_ad_zero;
        MachData.weigh_coef = MachData.weigh_fullrange /  (MachData.weigh_ad_full + 0.1);
        U32toBUF(MachData.weigh_ad_full,buf);
        buf[4] = buf[0];
        buf[5] = buf[1];
        buf[6] = buf[2];
        buf[7] = buf[3]; 
        Write_EEPROM(EEP_WEIGHTFULL_ADDR,buf,8);
        buf[0] = 0xaa;
        buf[1] = 0xaa;        
        Write_EEPROM(EEP_CALFLAG_ADDR,buf,2);  //caldata ok flag

    } else if(CalData.calstep == 3) {
        CalData.calstart = 0;
        CalData.calstep = 0;
        MachData.mode = MachData.mode - MACHINE_CAL_MODE;
    }
    
}


void Key_CalZeroProc(void)
{
   

}


//key proc when factory mode
void Key_FactoryTareProc(void)
{
    

}


void Key_FactoryZeroProc(void)
{
   

}


void Key_Proc_Factory(u16 i)
{



}

void Key_Proc_Cal(u16 key)
{
    switch(key){
    case KEY_PRESSED+KEY_0:
    case KEY_PRESSED+KEY_1:
    case KEY_PRESSED+KEY_2:
    case KEY_PRESSED+KEY_3:
    case KEY_PRESSED+KEY_4:
    case KEY_PRESSED+KEY_5:
    case KEY_PRESSED+KEY_6:
    case KEY_PRESSED+KEY_7:
    case KEY_PRESSED+KEY_8:
    case KEY_PRESSED+KEY_9:
    case KEY_PRESSED+KEY_DOT:
        break;
    case KEY_PRESSED+KEY_TARE:
        Key_CalTareProc();
        break;
    case KEY_PRESSED+KEY_ZERO:
        Key_CalZeroProc();
        break;        
    default:
        break;
    }

}


void Display_Factory(void)
{

    
}



void Display_Cal(void)
 {
     u32 i,j;
     
     if(0 == CalData.calstart)
         return;
     DIS_CalInfo(); //general display
     switch(CalData.calstep) {
     case 1:
         i = MachData.weigh_fullrange + MachData.weigh_onestep;
         DIS_Area1(i,2);
         //DIS_Area2();
         DIS_Area3(MData.ad_dat_avg,0);
         break;
     case 2:
         i = MachData.weigh_fullrange;
         DIS_Area1(i,0);
         //DIS_Area2();
         DIS_Area3(MData.ad_dat_avg-MachData.weigh_ad_zero,0);
         break;
         
     case 3:
         DIS_Area1(0,0);
         DIS_Area2(0,0);
         DIS_Area3(0,0);
         break;

     default:break;
     }
     
     DIS_Update();
 }