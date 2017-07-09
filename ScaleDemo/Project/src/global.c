#include "stm8s.h"
#include "math.h"
#include "stdio.h"

#include "global.h"

#include "key.h"
#include "energy.h"
#include "cs5550.h"
#include "ht1621.h"
#include "I2C.h"

//////////////////////////////////////定义全局变量
          u8 Key_Sound_Time;
       float TransP1,AbsP2,ConcP3,ParaP4;
        char Send_buf[13],neg_flag;
          u8 dot_position;
          u8 Cur_Display_Dynamic;    //指示当前是修改阶段还是输出阶段，默认是输出阶段，只有按了上下键后变为更改阶段
          //2: 第一次按 调出已有数值
          //1: 修改模式
          //0: 正常显示模式
         u16 Conc_tmp,ParaK_tmp;     //修改阶段使用
   ///////////////存储使用的变量
          u8 flag_MM,MM_display_time;
         u16 rec_total,current_display_rec;

APP_CONFIG_T APP_Configuration; 

////////////////////////////////////////////
//检查数据是否有效
////////////////////////////////////////////
u8 SYSTEM_CheckF32Valid(float *F32Value)
{
    #define NaN      0xFFFFFFFF
    #define plusINF  0x7F800000
    #define minusINF 0xFF800000

    if( (*((u32 *)F32Value) == NaN) ||(*((u32 *)F32Value) == plusINF) || (*((u32 *)F32Value) == minusINF))
     return FALSE;
    else
     return TRUE;
}
///////////////////////////////////////
//格式化显示数据
///////////////////////////////////////
void  APP_GetFormatString(float InValue,char *OutString)
{
    if(InValue>=999.95)
    {
        sprintf(OutString,"%4.0f",InValue);
    }
    else if(InValue>=99.995)
    {
        sprintf(OutString,"%4.1f",InValue);
    }
    else if(InValue>=9.9995)
    {
        sprintf(OutString,"%4.2f",InValue);
    }
    else
    {
        sprintf(OutString,"%4.3f",InValue);
    }
}
//////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////
static void APP_SetBaseEnergy(void)	 //get 0%T
{
     u8   i,loop_idx;
     u8   TempAmp;
    u16   TempEerngy;
    Energy_GetAMP(&TempAmp);   
    
    i = 8; //首先使用最大系数
    Energy_SetAMP(i-1);
    for(loop_idx=0;loop_idx<20;loop_idx++)
        Energy_Read();    //read 16 times
    TempEerngy = Get_Ad_Data_8();
    if(TempEerngy > 10000)
    {
      Key_Sound_Time = 200;
      Display_Err();
    }  
    else
    {
       do
      {
       Energy_SetAMP(i-1);  //首先使用最大放大倍数 并保存此放大倍数 
       for(loop_idx=0;loop_idx<50;loop_idx++)
        Energy_Read();    //read 16 times
       //最大放大倍数下的AD 不能超过10000
       APP_Configuration.DarkEnergy[i-1] = Get_Ad_Data_8();
       i--;
      }
      while(i!=0);
    }
    Energy_SetAMP(TempAmp);  //恢复
}
//////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////
static void APP_SetFullLoading(void)
{
    u8 i,loop_idx;
    
    for(loop_idx=0;loop_idx<ENERGY_AMP_TOTAL;loop_idx++)
    {
        Energy_SetAMP(ENERGY_AMP_TOTAL-loop_idx-1);
        for(i=0;i<50;i++)
            Energy_Read();    
           ////////////////////////////
          APP_Configuration.FullEnergy = Get_Ad_Data_8();
        if((APP_Configuration.FullEnergy<40000) && (APP_Configuration.FullEnergy > (APP_Configuration.DarkEnergy[ENERGY_AMP_TOTAL-loop_idx-1]+100)))	  //正常情况下取100%的AD不能超过50000
        { //当AD数小于50000时候的最大放大倍数是多少
            APP_Configuration.EnergyAmp = (ENERGY_AMP_TOTAL-loop_idx-1);
            return;
        }
    }
    
    if(loop_idx == ENERGY_AMP_TOTAL)
    {
      Key_Sound_Time = 200;
      Display_Err();
    }
}
////////////////////////////////////////////////
//
////////////////////////////////////////////////
static void APP_Energy2Trans(u16 *inValue,float *OutValue)
{
/*
InputData[0] = Current read AD data;
InputData[1] = Full loading;
InputData[2] = Zeroloading;
*/
    float RecordData;

    if (inValue[1] <= inValue[2])
    {
        *OutValue = APP_MIN_TRANS;
        return;
    }
    if(inValue[0] >= inValue[2])
    {
        RecordData = (float)(inValue[0] -inValue[2])*100.0;
    }
    else
    {
        RecordData = (-1.0)*(float)(inValue[2] -inValue[0])*100.0;
    }
    
    *OutValue = RecordData/((float)(inValue[1] - inValue[2]));
    
    if(SYSTEM_CheckF32Valid(OutValue)== FALSE)
    {
        *OutValue = APP_MIN_TRANS;
        return;
    }
    if(*OutValue < APP_MIN_TRANS)
    {
        *OutValue = APP_MIN_TRANS;
        return;
    }
    if(*OutValue >APP_MAX_TRANS)
    {
        *OutValue = APP_MAX_TRANS;
        return;
    }
    if((*OutValue > 99.9)&&(*OutValue <100.1))
    {
        *OutValue = 100.0;
    }
    return;
}
//////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////
static void APP_Energy2Abs(u16 *inValue,float *OutValue)
{
    APP_Energy2Trans(inValue,OutValue);	 //首先得到透过率
    ////////////////////////////////
    if(*OutValue < 0)
    {
        *OutValue = 0;
    }
    //透过率 与 吸光度的计算公司
    *OutValue = -log10((*OutValue/100.0)); 
    
    //条件检查
    if(SYSTEM_CheckF32Valid(OutValue)== FALSE)
    {
        *OutValue = APP_MAX_ABS;
        return;
    }
    if(*OutValue > APP_MAX_ABS)
    {
        *OutValue = APP_MAX_ABS;
    }
    if(*OutValue < APP_MIN_ABS)
    {
        *OutValue = APP_MIN_ABS;
    }
    return; 
}

///////////////////////////////////////////////
//根据不同的状态计算数值
///////////////////////////////////////////////
void   Data_Cal(u16 ad_data)
  {
        u16 EnergyData[3];
        //float OutValue;
        EnergyData[0] = ad_data;
        EnergyData[1] = APP_Configuration.FullEnergy;
        //EnergyData[2] = APP_Configuration.DarkEnergy;
        EnergyData[2] = APP_Configuration.DarkEnergy[APP_Configuration.EnergyAmp];
        switch(APP_Configuration.TestMode)
        {
            case APP_TEST_MODE_TRANS:
                APP_Energy2Trans(EnergyData,&TransP1);
                if(TransP1 == 100.0)
                  {	//自动校正
                    APP_Configuration.FullEnergy = EnergyData[0];
                  }
                break;
            case APP_TEST_MODE_ABS:
                APP_Energy2Abs(EnergyData,&AbsP2);
                if(AbsP2 == 0.0)
                  {	//自动校正
                    APP_Configuration.FullEnergy = EnergyData[0];
                  }
                break;
            case APP_TEST_MODE_CONC:
                APP_Energy2Abs(EnergyData,&ConcP3);
                if(ConcP3 == 0.0)
                  {
                    APP_Configuration.FullEnergy = EnergyData[0];
                  }
		//得到的吸光度*浓度斜率
                if(99999.99999 == APP_Configuration.ConcK) //开机还没有输入
                  ConcP3 = ConcP3 * ((float)APP_Configuration.Conc / AbsP2);
                else
                  ConcP3 = ConcP3*(float)APP_Configuration.ConcK;
                if(ConcP3 < APP_MIN_CONC)
                  {
                    ConcP3 = APP_MIN_CONC;
                  }
                if(ConcP3 > APP_MAX_CONC)
                  {
                    ConcP3 = APP_MAX_CONC;
                  }
                break;
            case APP_TEST_MODE_PARAMETER:
                APP_Energy2Abs(EnergyData,&ParaP4);
                if(ParaP4 == 0.0)
                  {
                    APP_Configuration.FullEnergy = EnergyData[0];
                  }
                ParaP4 = ParaP4*(float)APP_Configuration.ParaK;
                if(ParaP4 < APP_MIN_PARA_DISPLAY)
                  {
                    ParaP4 = APP_MIN_PARA_DISPLAY;
                  }
                if(ParaP4 > APP_MAX_PARA_DISPLAY)
                  {
                    ParaP4 = APP_MAX_PARA_DISPLAY;
                  }
                   break;
           default:break;  
        }
  }  

///////////////////////////////////////
//初始化
///////////////////////////////////////
void APP_Init(void)
{   
    u8 tmp[4];
    u16 rec_tmp1,rec_tmp2;
    APP_Configuration.DarkEnergy[0] = 1500-500;
    APP_Configuration.DarkEnergy[1] = 1500-500;
    APP_Configuration.DarkEnergy[2] = 1500-500;
    APP_Configuration.DarkEnergy[3] = 1500-500;
    APP_Configuration.DarkEnergy[4] = 1500-500;
    APP_Configuration.DarkEnergy[5] = 1500-500;
    APP_Configuration.DarkEnergy[6] = 1500-500;
    APP_Configuration.DarkEnergy[7] = 1500-500;
  
    //APP_Configuration.DarkEnergy = ENERGY_DARK_DEF;
    APP_Configuration.FullEnergy = ENERGY_FULL_DEF;
    APP_Configuration.EnergyAmp  =  ENERGY_AMP_DEF;
    APP_Configuration.TestMode = APP_TEST_MODE_ABS;
    APP_Configuration.ConcK = 99999.99999;
    APP_Configuration.Conc = 20;
    APP_Configuration.ParaK = 1000;
    APP_Configuration.RecordID = 0;
    
    //APP_SetFullLoading(); //开机设置为100%
    //开机把当前记录数读出来
    Read_EEPROM(EEPROM_REC_TOTAL_ADDR,tmp, 4);
    rec_tmp1 = (tmp[0]<<8) + tmp[1];
    rec_tmp2 = (tmp[2]<<8) + tmp[3];
    if((rec_tmp1>0)&&(rec_tmp1<501))
     {
      rec_total = rec_tmp1;
     }
    else if((rec_tmp2>0)&&(rec_tmp2<501))
    {
      rec_total = rec_tmp2;
    } 
    else
       rec_total = 0;  
       
}

////////////////////////////////以下为按键处理区
///////////////////////////mode
void  Key_Mode_Pro(void)
 {
  if(APP_TEST_MODE_RECALL == APP_Configuration.TestMode)//当前处在回看模式，无效
    return;                                             //回看模式下只有按ESC返回到TACP
  //TACP 
  APP_Configuration.TestMode++;
  
  if(APP_TEST_MODE_RECALL==APP_Configuration.TestMode) //跳过RECALL 状态 在TACP之间切换
     APP_Configuration.TestMode = APP_TEST_MODE_TRANS;
  //如果处在CP 状态，默认是显示状态
  if((APP_TEST_MODE_PARAMETER==APP_Configuration.TestMode)||(APP_TEST_MODE_CONC==APP_Configuration.TestMode))
    Cur_Display_Dynamic = 0; 
  
 }
///////////////////////////增加按键
void  Key_Up_Pro(void)
{
  if(APP_TEST_MODE_RECALL == APP_Configuration.TestMode)//当前处在回看模式
    Search_Up_Recorder(); //查看上一条记录
  else if(APP_TEST_MODE_CONC == APP_Configuration.TestMode)
  {
   if(0 == Cur_Display_Dynamic)
   {
    Cur_Display_Dynamic++;
    Conc_tmp =  APP_Configuration.Conc;  //call
   }
   else if(1 == Cur_Display_Dynamic)
     { 
      Conc_tmp++;
      if(Conc_tmp > APP_MAX_CONC)
        Conc_tmp = APP_MAX_CONC;
     }
   }
   else if(APP_TEST_MODE_PARAMETER == APP_Configuration.TestMode)
   {
     if(0 == Cur_Display_Dynamic)
      {
       Cur_Display_Dynamic++;
       ParaK_tmp =  APP_Configuration.ParaK;  //call
      }
     else if(1 == Cur_Display_Dynamic)
      { 
       ParaK_tmp++;
      if(ParaK_tmp > APP_MAX_PARA)
        ParaK_tmp = APP_MAX_PARA;
      }
   }

  else if(APP_TEST_MODE_TRANS == APP_Configuration.TestMode) //
    APP_SetFullLoading();  //其他情况下校准100% T A
  //else if(APP_TEST_MODE_ABS == APP_Configuration.TestMode)
  //  APP_SetBaseEnergy();  //其他情况下校准0 //A
  else if(APP_TEST_MODE_ABS == APP_Configuration.TestMode)
    APP_SetFullLoading();  //其他情况下校准0 //B
}

///////////////////////////减小按键
void  Key_Down_Pro(void)
{
 if(APP_TEST_MODE_RECALL == APP_Configuration.TestMode)//当前处在回看模式
    Search_Next_Recorder(); //查看下一条记录
 else if(APP_TEST_MODE_CONC == APP_Configuration.TestMode)
  {
    if(0 == Cur_Display_Dynamic)
    {
      Cur_Display_Dynamic++;
      Conc_tmp =  APP_Configuration.Conc;  //call
     }
    else if(1 == Cur_Display_Dynamic)
    {
      Conc_tmp--;
      if(Conc_tmp < APP_MIN_PARA)
         Conc_tmp = APP_MIN_PARA;
    } 
   }
  else if(APP_TEST_MODE_PARAMETER == APP_Configuration.TestMode)
  {
     if(0 == Cur_Display_Dynamic)
      {
       Cur_Display_Dynamic++;
       ParaK_tmp =  APP_Configuration.ParaK;  //call
       }
     else if(1 == Cur_Display_Dynamic)
     {
       ParaK_tmp--;
        if(ParaK_tmp < APP_MIN_PARA)
           ParaK_tmp = APP_MIN_PARA;
     }
  }   
 else //if(APP_TEST_MODE_TRANS == APP_Configuration.TestMode)
     //APP_SetFullLoading();  //其他情况下校准100% T A
    APP_SetBaseEnergy();  //其他情况下校准0
}

///////////////////////////mode
void  Key_Enter_Pro(void)
{ //当前处在参数模式
     u16 EnergyData[3];
        float OutValue;
        
  if((APP_TEST_MODE_CONC==APP_Configuration.TestMode)&&(1==Cur_Display_Dynamic))
   {
        APP_Configuration.Conc = Conc_tmp;
        Cur_Display_Dynamic = 0;    //重新回到正常显示模式
        //重新计算该因子
        EnergyData[0] = Get_Ad_Data_8();
        EnergyData[1] = APP_Configuration.FullEnergy;
        //EnergyData[2] = APP_Configuration.DarkEnergy;
        EnergyData[2] = APP_Configuration.DarkEnergy[APP_Configuration.EnergyAmp];
        APP_Energy2Abs(EnergyData,&OutValue); //重新计算一次吸光度
        if(OutValue <= 0.0)
          { //报警处理
            Key_Sound_Time = 200;
            Display_Err();
          }
        else
        {
         APP_Configuration.ConcK = (float)APP_Configuration.Conc / (float)OutValue;
    
        }
   }
  else if((APP_TEST_MODE_PARAMETER==APP_Configuration.TestMode)&&(1==Cur_Display_Dynamic))
    {
     APP_Configuration.ParaK = ParaK_tmp;
     APP_Configuration.TestMode = APP_TEST_MODE_CONC;
     Cur_Display_Dynamic = 0;    //重新回到正常显示模式
    } 
  else if((APP_TEST_MODE_CONC==APP_Configuration.TestMode)||(APP_TEST_MODE_RECALL==APP_Configuration.TestMode)||(APP_TEST_MODE_TRANS==APP_Configuration.TestMode)||(APP_TEST_MODE_ABS==APP_Configuration.TestMode))
  {  //在 T A C RECALL状态下输出数据
   //Send_RS232(); //printer
   Send_RS232_PC();
   //UART2_SendString(Send_buf,10);
  }
}
///////////////////////////mode
void  Key_Save_Pro(void)
{
  u8 i,tmp_data[4];
  u16 tmp;
  float   tmp2;
 //只要不是在查看模式下都能存储数据 
 if(APP_TEST_MODE_RECALL != APP_Configuration.TestMode)
  {
   tmp_data[0] = APP_Configuration.TestMode;
   if(1==neg_flag)
     tmp_data[0] |= 0x80;  //正负 符号位 
   
   switch(APP_Configuration.TestMode)
   {
    case APP_TEST_MODE_TRANS:       tmp2=TransP1;break;
    case APP_TEST_MODE_ABS:         tmp2=AbsP2;break;
    case APP_TEST_MODE_CONC:        tmp2=ConcP3;break;
    case APP_TEST_MODE_PARAMETER:   tmp2=ParaP4;break;
    default:break;  
   }
    if(0==dot_position)
       tmp = (u16)tmp2;
    else
    {
       for(i=0;i<dot_position;i++)
              tmp2 = tmp2*10;
           tmp = (u16)tmp2; 
    }
   ///////////////////////////////////////////////////////
   tmp_data[1] =(tmp>>8)&0xff;
   tmp_data[2] = tmp&0xff;
   tmp_data[3] = dot_position;
   if(500 == rec_total) //当前正存满
     rec_total = 0;
   Write_EEPROM(EEPROM_REC_START_ADDR+REC_DATA_LENGTH*rec_total, tmp_data,4); //写入一条记录
   flag_MM = 1;
   MM_display_time = 10; //100ms *10 = 1S 
  
   rec_total++;
   tmp_data[0] = (rec_total>>8)&0xff;
   tmp_data[1] =  rec_total&0xff;
   tmp_data[2] =  tmp_data[0];
   tmp_data[3] =  tmp_data[1];
   Write_EEPROM(EEPROM_REC_TOTAL_ADDR,tmp_data,4); //保存记录总数 
  } 
}
///////////////////////////mode
void  Key_ESC_Pro(void)
{ //只有在回看模式下才能返回
  if(APP_TEST_MODE_RECALL == APP_Configuration.TestMode)//当前处在回看模式
    APP_Configuration.TestMode = APP_TEST_MODE_TRANS;   //默认返回到 T 状态 
}
///////////////////////////快速增加
void  Key_Up_Fast(void)
{
  if((APP_TEST_MODE_CONC==APP_Configuration.TestMode)&&(1==Cur_Display_Dynamic))
  {  //快速增加
    if(Conc_tmp < (APP_MAX_PARA-10))
     Conc_tmp+=10;
  }
  else if((APP_TEST_MODE_PARAMETER==APP_Configuration.TestMode)&&(1==Cur_Display_Dynamic))
  { 
    if(ParaK_tmp < (APP_MAX_PARA-10))
     ParaK_tmp+=10;
  }  
}
void  Key_Down_Fast(void)
{
   if((APP_TEST_MODE_CONC==APP_Configuration.TestMode)&&(1==Cur_Display_Dynamic))
   {  //快速减少
     if(Conc_tmp > (APP_MIN_PARA+10))
     Conc_tmp = Conc_tmp-10;
   }
  else if((APP_TEST_MODE_PARAMETER==APP_Configuration.TestMode)&&(1==Cur_Display_Dynamic))
   {
    if(ParaK_tmp > (APP_MIN_PARA+10))
     ParaK_tmp = ParaK_tmp-10;
   }  
}
///////////////////////////mode
void  Key_Recall_Pro(void)
{
  if(APP_TEST_MODE_RECALL != APP_Configuration.TestMode)
   {
    APP_Configuration.TestMode = APP_TEST_MODE_RECALL;
    current_display_rec = rec_total;
   } 
}

void  Key_Rec_Clear_Pro(void)
{
  u8 tmp_data[4];
     rec_total   = 0;
     tmp_data[0] = 0;
     tmp_data[1] = 0;
     tmp_data[2] = 0;
     tmp_data[3] = 0;
     Write_EEPROM(EEPROM_REC_TOTAL_ADDR,tmp_data,4); //  
}  
////////////////////////////////////eeprom 相关操作
void  Search_Up_Recorder(void)
{
  //if(rec_total==0) return; //记录为空
  
  if(current_display_rec > 1)
     current_display_rec--;
}
void  Search_Next_Recorder(void)
{ 
   //if(rec_total==0) return; //记录为空
  
   if(current_display_rec < rec_total)
     current_display_rec++;
}
////////////////////////////key over