#include "stm8s.h"
#include "stdio.h"
#include "math.h"

#include "global.h"
#include "ht1621.h"
#include "I2C.h"

#define   DISP_C       0x0a
#define   DISP_A       0x0b
#define   DISP_L       0x0c
#define   DISP_P       0x0d
#define   DISP_Q       0x0e
#define   DISP_F       0x0f
#define   DISP_U       0x10
#define   DISP_E       0x11
#define   DISP_r       0x12
#define   DISP_X       0x13  // -
#define   DISP_b       0x14
#define   DISP_o       0x15
#define   DISP_t       0x16
#define   DISP_d       0x17
#define   DISP_N       0x18
#define   DISP_NULL    0x19

const u8 display_code[] = {SEG_A+SEG_B+SEG_C+SEG_D+SEG_E+SEG_F,      //0
                           SEG_B+SEG_C,                              //1  
                           SEG_A+SEG_B+SEG_D+SEG_E+SEG_G,            //2
                           SEG_A+SEG_B+SEG_C+SEG_D+SEG_G,            //3
                           SEG_B+SEG_C+SEG_F+SEG_G,                  //4
                           SEG_A+SEG_C+SEG_D+SEG_F+SEG_G,            //5
                           SEG_A+SEG_C+SEG_D+SEG_E+SEG_F+SEG_G,      //6
                           SEG_A+SEG_B+SEG_C,                        //7
                           SEG_A+SEG_B+SEG_C+SEG_D+SEG_E+SEG_F+SEG_G,//8
                           SEG_A+SEG_B+SEG_C+SEG_D+SEG_F+SEG_G,      //9
						  
                           SEG_A+SEG_D+SEG_E+SEG_F,                  //C    0x0a
                           SEG_A+SEG_B+SEG_C+SEG_E+SEG_F+SEG_G,      //A	  0x0b
						   SEG_D+SEG_E+SEG_F,				         //L    0x0c
						   SEG_A+SEG_B+SEG_E+SEG_F+SEG_G,            //P	  0x0d
						   SEG_A+SEG_B+SEG_C+SEG_F+SEG_G,			 //Q    0x0e	
						   SEG_A+SEG_E+SEG_F+SEG_G,                  //F    0x0f
						   SEG_B+SEG_C+SEG_D+SEG_E+SEG_F,            //U    0x10
                           SEG_A+SEG_D+SEG_E+SEG_F+SEG_G,            //E    0x11                      
                           SEG_E+SEG_F+SEG_G,                        //r    0x12
						   SEG_G,                                    //-	  0x13
						   SEG_C+SEG_D+SEG_E+SEG_F+SEG_G,            //b..  0x14 
						   SEG_C+SEG_D+SEG_E+SEG_G,                  //o    0x15
                           SEG_D+SEG_E+SEG_F+SEG_G,                  //t  	0x16		
					  	   SEG_B+SEG_C+SEG_D+SEG_E+SEG_G,            //d  	0x17
 						   SEG_A+SEG_B+SEG_C+SEG_E+SEG_F,            //N    0x18
                           0                                         //NULL 0x19                        
};

//FULL
u8 const display_FULL[]  = {DISP_NULL,DISP_NULL,DISP_F,DISP_U,DISP_L,DISP_L,DISP_NULL}; 
//CAL
u8 const display_CAL_INFO1[]  = {DISP_NULL,DISP_C,DISP_A,DISP_L,DISP_NULL};

u8 const display_CAL_INFO2[]  = {DISP_NULL,DISP_C,DISP_A,DISP_L,DISP_NULL};
u8 const display_CAL_INFO3[]  = {DISP_NULL,DISP_C,DISP_A,DISP_L,DISP_NULL};

u8 const display_ERR_INFO[]   = {DISP_NULL,DISP_E,DISP_r,DISP_r,DISP_NULL,0,1};

//
u8 const display_CFG_INFO[]   = {DISP_U, DISP_C,     0,1,DISP_NULL,       0,1};

u8 const display_F_CFG_INFO[] = {DISP_F, DISP_C,     0,1,DISP_NULL,       0,1};
//
u8 const display_BOOT_INFO[]  = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

u8 const display_MODEL_INFO[] = {DISP_NULL, DISP_X, 0, 0, 0, 0,  DISP_X};
//
u8 const display_PCS_INFO[]   = {DISP_P, DISP_C,     5,DISP_X,   0,  0,  0};


////////////////////////////
//HT1621 管脚初始化
///////////////////////////
static void HT1621_PinInit(void)
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
void HT1621_Init(void)   
{   
    u8 i;
    HT1621_PinInit();
    
    Send_Com(COMMAND,SYS_EN);   
    Send_Com(COMMAND,Rosc);   
    Send_Com(COMMAND,BIS1_2);   
    //Send_Com(COMMAND,NORMAL);        
    Send_Com(COMMAND,LCD_ON);   
    
    All_OFF_Must();
}  
////////////////////////////
//HT1621 刷新显示区
///////////////////////////
void Update_Display(void)
{
    u8 i,j,data;   
    CSL;   
    Send_LBit(WRITE,3);   
    Send_LBit(0x00,6);   
    for( i = 0; i < 16; i++) {
        data = display_buffer[15 - i];
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
        display_buffer[i] = 0xff;
    
    Update_Display();
}   

//熄灭
void All_OFF_Must(void)   
{   
    u8 i;   
    for( i = 0; i < 16; i++)
        display_buffer[i] = 0x00;
    
    Update_Display();
}   

//-----------------------------------------------------------------------------   
//全部点亮 
void All_Special_Char(u8 data)   
{   
    u8 i;   
    for( i = 0; i < 16; i++)
        display_buffer[i] = display_code[data];
    
    Update_Display();
}   
//-----------------------------------------------------------------------------   

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
void Display_Boot_Info(void)
 {
    u8 i;
	for(i=0;i<16;i++)	
        display_buffer[i] = display_code[display_BOOT_INFO[i]];
    
}

 
//////////////////////////////////////////////////////////////////////
void Display_Area1(u32 data,u8 dot)
{
    u8 i;
    u8 buf[16];
    snprintf(buf,6,"%d",data);
    
    for(i=0;i<5;i++)
        display_buffer[i] = display_code[buf[i]-0x30];

}	
void Display_Area2(u32 data,u8 dot)
{
    u8 i,buf[5];
    if(data > 99999)
        data = 99999;
    buf[0] =  data / 10000;
    buf[1] = (data % 10000) / 1000;
    buf[2] = (data % 1000) / 100;
    buf[3] = (data % 100) / 10;
    buf[4] =  data % 10;
   
    for(i=0;i<5;i++)
        display_buffer[5+i] = display_code[buf[i]]; 
    
}	

void Display_Area3(u32 data,u8 dot)
{
    u8 i,buf[6];
    if(data > 999999)
        data = 999999;
    buf[0] =  data / 100000;
    buf[1] = (data % 100000) / 10000;
    buf[2] = (data % 10000) / 1000;
    buf[3] = (data % 1000) / 100;
   
    buf[4] = (data % 100) / 10;

    buf[5] =  data % 10;
    for(i=0;i<6;i++)
        display_buffer[10+i] = display_code[buf[i]];
    
}	

