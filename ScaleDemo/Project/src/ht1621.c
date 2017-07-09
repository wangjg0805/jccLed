#include "stm8s.h"
#include "stdio.h"
#include "math.h"

#include "global.h"
#include "ht1621.h"
#include "I2C.h"



//76543210
//.CGBDEFA

        #define SEG_A 0x80
	#define SEG_B 0x40
        #define SEG_C 0x20
	#define SEG_D 0x01
	#define SEG_E 0x02
	#define SEG_F 0x08
	#define SEG_G 0x04
	#define SEG_P 0x10

/*
#define   SEG_A     0x01
#define   SEG_B     0x10
#define   SEG_C     0x40
#define   SEG_D     0x08
#define   SEG_E     0x04
#define   SEG_F     0x02
#define   SEG_G     0x20
#define   SEG_P     0x80
*/
//////////////////////////符号位
#define   FLAG_T        0x10
#define   FLAG_A        0x20
#define   FLAG_C        0x40
#define   FLAG_P        0x80
#define   FLAG_MM       0x08
#define   FLAG_ABS      0x04
#define   FLAG_TRANS    0x02
#define   FLAG_LOGO     0x01
//#define   FLAG_LOGO     0x00

#define   FLAG_RM       0x80  //依附在RAM[1]中
#define   FLAG_NEG      0x80  //依附在RAM[4]中 
#define   DISP_NULL   17    //最后显示码是 “空” 显示码

const u8 display_code[18]={SEG_A+SEG_B+SEG_C+SEG_D+SEG_E+SEG_F,      //0
                           SEG_B+SEG_C,                              //1  
                           SEG_A+SEG_B+SEG_D+SEG_E+SEG_G,            //2
                           SEG_A+SEG_B+SEG_C+SEG_D+SEG_G,            //3
                           SEG_B+SEG_C+SEG_F+SEG_G,                  //4
                           SEG_A+SEG_C+SEG_D+SEG_F+SEG_G,            //5
                           SEG_A+SEG_C+SEG_D+SEG_E+SEG_F+SEG_G,      //6
                           SEG_A+SEG_B+SEG_C,                        //7
                           SEG_A+SEG_B+SEG_C+SEG_D+SEG_E+SEG_F+SEG_G,//8
                           SEG_A+SEG_B+SEG_C+SEG_D+SEG_F+SEG_G,      //9
                           SEG_A+SEG_B+SEG_C+SEG_E+SEG_F+SEG_G,      //A
                           SEG_C+SEG_D+SEG_E+SEG_F+SEG_G,            //b
                           SEG_A+SEG_D+SEG_E+SEG_F,                  //C
                           SEG_B+SEG_C+SEG_D+SEG_E+SEG_G,            //d
                           SEG_A+SEG_D+SEG_E+SEG_F+SEG_G,            //E
                           SEG_A+SEG_E+SEG_F+SEG_G,                  //F    15  
                           SEG_E+SEG_F+SEG_G,                        //r    0x10
                           0                                         //NULL 0x11
                              };
///////////////////////////////////
//ram0   ram1   ram2   ram3   ram4   ram5   ram6   ram7
//flag   S个    S十    S百    个     十     百     千
//
static u8 display_ram[16];

////////////////////////////////////////////
/* 1 delay */
////////////////////////////////////////////
static void delay(u32 length)
	{
	  while (length >0)
    	         length--;
	 }

////////////////////////////////
////////////////////////////
//HT1621 管脚初始化
///////////////////////////
void  HT1621_Pin_Init(void)
  {
   GPIO_Init(GPIOC,GPIO_PIN_6,GPIO_MODE_OUT_PP_LOW_FAST); //CS
   GPIO_Init(GPIOD,GPIO_PIN_0,GPIO_MODE_OUT_PP_LOW_FAST); //WR
   GPIO_Init(GPIOC,GPIO_PIN_7,GPIO_MODE_OUT_PP_LOW_FAST); //DATA
  } 
//-----------------------------------------------------------------------------   
//传送Length位数据给1621   
static void Send_LBit(u8 Sdata,u8 Length)   
{   
    u8 i=Length;   
    while(i--)   
    {   
        WRL;      //nWR=0;   
        if(Sdata&0x80)   
            DATAH;//DATA=1;   
        else   
            DATAL;//DATA=0;   
        Sdata<<=1;   
        WRH;      //nWR=1;   
            asm("nop");
            asm("nop");
    }      
} 
//-----------------------------------------------------------------------------   
//送控制指令   
static void Send_Com(u8 con,u8 sdata)   
{   
    CSL;//nCS=0;   
    Send_LBit(con,3);   
    Send_LBit(sdata,8);   
    Send_LBit(0x00,2);   
    CSH;//nCS=1;   
}   
//-----------------------------------------------------------------------------   
//初始化   
void Init_1621(void)   
{   
    u8 i; 
    Send_Com(COMMAND,SYS_EN);   
    Send_Com(COMMAND,Rosc);   
    Send_Com(COMMAND,BIS1_2);   
    //Send_Com(COMMAND,NORMAL);        
    Send_Com(COMMAND,LCD_ON);   
    
    for(i=0;i<8;i++)
      display_ram[i] = 0;
}  
///////////////////////////////////////////////////
//符号处理
 void flag_disp_proc(u8 TestMode)
 {
   if(APP_TEST_MODE_TRANS == TestMode)
     {
      display_ram[0] |= FLAG_T+FLAG_TRANS+FLAG_LOGO;
      display_ram[0] &= ~(FLAG_A+FLAG_C+FLAG_P+FLAG_ABS+FLAG_MM);
     }
   if(APP_TEST_MODE_ABS == TestMode)
     {
      display_ram[0] |= FLAG_A+FLAG_ABS+FLAG_LOGO;
      display_ram[0] &= ~(FLAG_T+FLAG_C+FLAG_P+FLAG_TRANS+FLAG_MM);
     }
    if(APP_TEST_MODE_CONC == TestMode)
     {
      display_ram[0] |= FLAG_C+FLAG_LOGO;
      display_ram[0] &= ~(FLAG_T+FLAG_A+FLAG_P+FLAG_TRANS+FLAG_ABS+FLAG_MM);
     }
    if(APP_TEST_MODE_PARAMETER == TestMode)
     {
      display_ram[0] |= FLAG_P+FLAG_LOGO;
      display_ram[0] &= ~(FLAG_T+FLAG_A+FLAG_C+FLAG_TRANS+FLAG_ABS+FLAG_MM);
     }
   if(flag_MM)
     {
      MM_display_time--;
      if(0 == MM_display_time)
        flag_MM = 0;
       display_ram[0] |= FLAG_MM;
     }
   else
       display_ram[0] &= ~FLAG_MM;
 }

/////////////////////////////////////////////////////////
///////////////////////////////////////////////
//
/////////////////////////////////////////////
void   display_ad_data(u16 ad_data)
{
      display_ram[7] = display_code[ad_data/10000];         //万
      display_ram[6] = display_code[(ad_data%10000)/1000];
      display_ram[5] = display_code[(ad_data%1000)/100];
      display_ram[4] = display_code[(ad_data%100)/10];
      display_ram[1] = display_code[ad_data%10];
      flag_disp_proc(APP_Configuration.TestMode);
}
//////////////////////////////////////////
u8 pre_proc_data(float *InValue)
{
  u8 dot_position;
     dot_position = 0;
     
    if(*InValue>=999.95)
        dot_position = 0;
    else if(*InValue>=99.995)
       {
        dot_position = 1;
        *InValue = (*InValue)*10;
       }
    else if(*InValue>=9.9995)
       {
        dot_position = 2;
        *InValue = (*InValue)*100;
       }
    else
       {
        dot_position = 3;
        *InValue = (*InValue)*1000;
       }
 return(dot_position);
}
////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////
void Update_Display_Buf(float InValue)
{
  //先处理修改阶段的显示
  u8 tmp1,tmp2,tmp3,tmp4;
  neg_flag = 0;
  switch(APP_Configuration.TestMode)
  {
  case APP_TEST_MODE_TRANS:
                           dot_position = pre_proc_data(&InValue);
                           neg_flag = 0;
                           break;

  case APP_TEST_MODE_ABS:
                           if(InValue < 0.0)
                           {
                            InValue = 0.00000001 - InValue;
                            InValue = InValue*1000; 
                            neg_flag = 1;
                           }
                           else
                           {
                           dot_position = pre_proc_data(&InValue);
                           neg_flag = 0;
                           }  
                           dot_position = 3;  
                           break;
                           
  case APP_TEST_MODE_CONC:
                           if(0 == Cur_Display_Dynamic)
                          {
                             if(InValue < 0.0)
                           {
                            InValue = 0.0 - InValue;
                            //InValue = InValue*1000; 
                            neg_flag = 1;
                           }
                           else
                           neg_flag = 0;
                           dot_position = pre_proc_data(&InValue);
           
                          }
                          else
                          {
                          dot_position = 0;
                          }
                           break;
    
  case APP_TEST_MODE_PARAMETER:
                          if(0 == Cur_Display_Dynamic)
                          {
                             if(InValue < 0.0)
                             {
                              InValue = 0.0 - InValue;
                              //InValue = InValue*1000; 
                              neg_flag = 1;
                             }
                             else
                              neg_flag = 0;
                          }
                          else
                             neg_flag = 0;
                          
                          dot_position = 0;
                           break;
                           
  default:                 break;
  }
    
      tmp1 = (u32)InValue/1000;
      tmp2 = ((u32)InValue%1000)/100;
      tmp3 = ((u32)InValue%100)/10;
      tmp4 = (u32)InValue%10;
      
      display_ram[7] = display_code[tmp1];
      display_ram[6] = display_code[tmp2];
      display_ram[5] = display_code[tmp3];
      display_ram[4] = display_code[tmp4];
      if(0 != dot_position)
      display_ram[4+dot_position] |= SEG_P;
      
      ///////////////////////////T 状态下拦截
      if(APP_TEST_MODE_TRANS == APP_Configuration.TestMode)
      {
        if(dot_position == 2)
        {
         display_ram[4] = display_ram[5];
         display_ram[5] = display_ram[6];
         display_ram[6] = display_ram[7];
         display_ram[7] = 0;
        }
        if(dot_position == 3)  
        {
         display_ram[4] = display_ram[6];
         display_ram[5] = display_ram[7];
         display_ram[6] = 0;
         display_ram[7] = 0;
        }
      }

      //////////////////////////////////////C 状态下拦截
       if(APP_TEST_MODE_CONC == APP_Configuration.TestMode)
      {
         if(dot_position == 1)
        {
         display_ram[4] = display_ram[5];
         display_ram[5] = display_ram[6];
         display_ram[6] = display_ram[7];
         display_ram[7] = 0;
        }
        else  if(dot_position == 2)
        {
         display_ram[4] = display_ram[6];
         display_ram[5] = display_ram[7];
         display_ram[6] = 0;
         display_ram[7] = 0;
        }
        else  if(dot_position == 3)  
        {
         display_ram[4] = display_ram[7];
         display_ram[5] = 0;
         display_ram[6] = 0;
         display_ram[7] = 0;
        }
      }
           ////////////////////////////////////////////////////////////ABS下构建打印数组
      if((APP_TEST_MODE_CONC == APP_Configuration.TestMode)||(APP_TEST_MODE_TRANS == APP_Configuration.TestMode)||(APP_TEST_MODE_ABS == APP_Configuration.TestMode))
      {
        if(1==neg_flag)
          Send_buf[0] = '-';
        else
          Send_buf[0] = 0x20;
        
        switch(dot_position)
        {
         case 0: Send_buf[1] = tmp1+0x30;
                 Send_buf[2] = tmp2+0x30;
                 Send_buf[3] = tmp3+0x30;
                 Send_buf[4] = tmp4+0x30;
                 Send_buf[5] = 0x20;
                 //         
                 break;
         case 1: Send_buf[1] = tmp1+0x30;
                 Send_buf[2] = tmp2+0x30;
                 Send_buf[3] = tmp3+0x30;
                 Send_buf[4] = '.';
                 Send_buf[5] = tmp4+0x30; 
                 break;
         case 2: Send_buf[1] = tmp1+0x30;
                 Send_buf[2] = tmp2+0x30;
                 Send_buf[3] = '.';
                 Send_buf[4] = tmp3+0x30;
                 Send_buf[5] = tmp4+0x30; 
                 break;
         case 3: Send_buf[1] = tmp1+0x30;
                 Send_buf[2] = '.';
                 Send_buf[3] = tmp2+0x30;
                 Send_buf[4] = tmp3+0x30;
                 Send_buf[5] = tmp4+0x30; 
                 break;
         default: break;
        }
                 Send_buf[6]  = 0x20; 
                 Send_buf[7]  = 0x20;
                 if(APP_TEST_MODE_TRANS == APP_Configuration.TestMode)
                 {
                  Send_buf[8]  = 0x20; 
                  Send_buf[9]  = '%';
                  Send_buf[10] = 'T';
                 }
                 else if(APP_TEST_MODE_ABS == APP_Configuration.TestMode)
                 {
                  Send_buf[8]  = 'A'; 
                  Send_buf[9]  = 'b';
                  Send_buf[10] = 's';
                 } 
                 else
                 {
                  Send_buf[8]  = 0x20; 
                  Send_buf[9]  = 0x20;
                  Send_buf[10] = 0x20;
                 } 
                 Send_buf[11] = 0x0d;
                 Send_buf[12] = 0x0a;
      } 
      
     if(1 == neg_flag)
       display_ram[4] |= FLAG_NEG; 
      else
       display_ram[4] &= ~FLAG_NEG; 
      
 ////////////////////////////以下是符号处理
      flag_disp_proc(APP_Configuration.TestMode);
 ////////////////////////////////////存储记录总数的显示
      display_ram[3] = display_code[ rec_total/100];
      display_ram[2] = display_code[(rec_total%100)/10];
      display_ram[1] = display_code[ rec_total%10];
}
////////////////////////////////////////////////////
//显示当前的记录
void Update_Display2_Buf(void)
{
  u8 tmp1,tmp2,tmp3,tmp4;
 static u16 display_rec_index; 
         u8 i,rec_tmp[4];
        u16 tmp_data;
 if(0 == current_display_rec)
 {
    for(i=0;i<8;i++)
    display_ram[i] = 0x00; 
    
    display_ram[3] = display_code[0];
    display_ram[2] = display_code[0];
    display_ram[1] = display_code[0];
  
 }
 else if(current_display_rec != display_rec_index)
 {
    display_rec_index = current_display_rec;
    for(i=0;i<8;i++)
    display_ram[i] = 0x00; 
    ///////////////////////////读出来
    Read_EEPROM(EEPROM_REC_START_ADDR+REC_DATA_LENGTH*(display_rec_index-1),rec_tmp,4);
     //恢复 
      tmp_data = (rec_tmp[1]<<8) + rec_tmp[2];
      tmp1 =  tmp_data/1000;
      tmp2 = (tmp_data%1000)/100;
      tmp3 = (tmp_data%100)/10;
      tmp4 =  tmp_data%10;
    //////////////////////////////////////////
    display_ram[7] = display_code[tmp1];
    display_ram[6] = display_code[tmp2];
    display_ram[5] = display_code[tmp3];
    display_ram[4] = display_code[tmp4];
    ////////////////////////////////////////////////
      if((0!=rec_tmp[3])&&(rec_tmp[3]<4))
      display_ram[4+rec_tmp[3]] |= SEG_P; 
      
       if(0x80 == (rec_tmp[0]&0x80))
       display_ram[4] |= FLAG_NEG; 
      else
       display_ram[4] &= ~FLAG_NEG; 
      
    flag_disp_proc(rec_tmp[0]);
   /////////////////////////////////////////////组建
   // if((APP_TEST_MODE_TRANS==(rec_tmp[0]&0x0f))||(APP_TEST_MODE_ABS==(rec_tmp[0]&0x0f)))
   //   {  
        if(0x80==(rec_tmp[0]&0x80))
          Send_buf[0] = '-';
        else
          Send_buf[0] = 0x20;
        
        switch(rec_tmp[3])
        {
         case 0: Send_buf[1] = tmp1+0x30;
                 Send_buf[2] = tmp2+0x30;
                 Send_buf[3] = tmp3+0x30;
                 Send_buf[4] = tmp4+0x30;
                 Send_buf[5] = 0x20;
                 //         
                 break;
         case 1: Send_buf[1] = tmp1+0x30;
                 Send_buf[2] = tmp2+0x30;
                 Send_buf[3] = tmp3+0x30;
                 Send_buf[4] = '.';
                 Send_buf[5] = tmp4+0x30; 
                 break;
         case 2: Send_buf[1] = tmp1+0x30;
                 Send_buf[2] = tmp2+0x30;
                 Send_buf[3] = '.';
                 Send_buf[4] = tmp3+0x30;
                 Send_buf[5] = tmp4+0x30; 
                 break;
         case 3: Send_buf[1] = tmp1+0x30;
                 Send_buf[2] = '.';
                 Send_buf[3] = tmp2+0x30;
                 Send_buf[4] = tmp3+0x30;
                 Send_buf[5] = tmp4+0x30; 
                 break;
         default: break;
        }
                 Send_buf[6]  = 0x20; 
                 Send_buf[7]  = 0x20;
                 if(APP_TEST_MODE_TRANS==(rec_tmp[0]&0x0f))
                 {
                  Send_buf[8]  = 0x20; 
                  Send_buf[9]  = '%';
                  Send_buf[10] = 'T';
                 }
                 else if(APP_TEST_MODE_ABS==(rec_tmp[0]&0x0f))
                 {
                  Send_buf[8]  = 'A'; 
                  Send_buf[9]  = 'b';
                  Send_buf[10] = 's';
                 } 
                 else
                 {
                  Send_buf[8]  = 0x20; 
                  Send_buf[9]  = 0x20;
                  Send_buf[10] = 0x20;
                 }
                 Send_buf[11] = 0x0d;
                 Send_buf[12] = 0x0a;
    
  ///////////////////////////////////////////////  
    display_ram[3] = display_code[display_rec_index/100];
    display_ram[2] = display_code[(display_rec_index%100)/10];
    display_ram[1] = display_code[display_rec_index%10];
 }
    ////////////////////////////////////点亮符号位
    display_ram[0] |= FLAG_LOGO;
    display_ram[1] |= FLAG_RM;
}
////////////////////////////
//显存更新
///////////////////////////
void Refresh_DisplayBuf(void)
{ 
  //根据不同的工作模式，将数据写入显示缓存中
  u8  i;
  //char Send_Buf[8];
  
  switch(APP_Configuration.TestMode)
  {
   case APP_TEST_MODE_TRANS:
        //APP_GetFormatString(TransP1,Send_Buf);
        Update_Display_Buf(TransP1);
        break;
   case APP_TEST_MODE_ABS:	
        //APP_GetFormatString(AbsP2,Send_Buf);
        Update_Display_Buf(AbsP2);
        break;
   case APP_TEST_MODE_CONC:
        if(0 == Cur_Display_Dynamic)
        { //动态显示
         //APP_GetFormatString(ConcP3,Send_Buf);
         Update_Display_Buf(ConcP3);
        }
        else
        {  //修改参数阶段
         Update_Display_Buf(Conc_tmp);
        }
        break;
   case APP_TEST_MODE_PARAMETER:       
        if(0 == Cur_Display_Dynamic)
        {
         //APP_GetFormatString(ParaP4,Send_Buf);
         Update_Display_Buf(ParaP4);
        }
        else
        {
         Update_Display_Buf(ParaK_tmp);
        }
        break;
   case APP_TEST_MODE_RECALL:  
        Update_Display2_Buf();
        break;
  default: break;
  }
}
////////////////////////////
//HT1621 刷新显示区
///////////////////////////
void Refresh_Ht1621(void)
{
    u8 i,j,data;   
    CSL;   
    Send_LBit(WRITE,3);   
    Send_LBit(0x00,6);   
    for( i = 0; i < 16; i++) {
        data = display_ram[i];
        Send_LBit(data,8); 
    }
    CSH;
}
//-----------------------------------------------------------------------------   
//全部点亮   
void All_ON_Must(void)   
{   
    u8 i;   
    for( i = 0; i < 16; i++)
        display_ram[i] = 0xff;
    
    Refresh_Ht1621();
}   

//全部点亮   
void All_OFF_Must(void)   
{   
    u8 i;   
    for( i = 0; i < 16; i++)
        display_ram[i] = 0x00;
    
    Refresh_Ht1621();
}   

//-----------------------------------------------------------------------------   
//全部点亮 
const u8 buf[8] = {SEG_A,SEG_B,SEG_C,SEG_D,SEG_E,SEG_F,SEG_G,SEG_P};
void All_Special_Char(u8 data)   
{   
  
    u8 i;   
    for( i = 0; i < 16; i++)
        display_ram[i] = display_code[data];
    
    Refresh_Ht1621();
}   
//-----------------------------------------------------------------------------   


void  Display_Err(void)
{
  u8 i;
  for(i=0;i<8;i++)
    display_ram[i] = 0x00; 
    //display Err
    display_ram[4] = display_code[0x0E];
    display_ram[5] = display_code[0x10];
    display_ram[6] = display_code[0x10];
    Refresh_Ht1621();
    delay(1234567);
}