#include <reg52.h>
#include "typedefs.h"
#include "cs5550.h"

union {  UINT32 rlong;
         struct { 
		  UINT8 byte3;     
		  UINT8 byte2;   
		  UINT8 byte1;   
		  UINT8 byte0;  
		}bytes;
      }utemp; 

void delay( UINT16 milisecond )
{   UINT8 i;
    while(milisecond--)
      {   i=110;
          while(i--);
      }
}

void CS5550_Sync(void)
{ 
   CS5550_SCLK = 0; 
   CS5550_Write_Byte(SYNC1);
   CS5550_Write_Byte(SYNC1);
   CS5550_Write_Byte(SYNC1);
   CS5550_Write_Byte(SYNC0);  
}

UINT8 CS5550_Read_Byte(void)
{  UINT8 i=8, dbyte=SYNC0;
   while(i--)
    {  if(dbyte&0x80)
         CS5550_SDI=1;
   	   else
	     CS5550_SDI=0;
	   dbyte = dbyte << 1;
	   if(CS5550_SDO==1)
	     dbyte++;  
	   CS5550_SCLK = 1; 
	   CS5550_SCLK = 0;	      	   
    }
   return dbyte;
}


void CS5550_Write_Byte(UINT8 dbyte)
{  UINT8 i=8;
   while(i--)
    {  if(dbyte&0x80)
         CS5550_SDI=1;
       else
         CS5550_SDI=0;
       CS5550_SCLK = 1;
       CS5550_SCLK = 0; 	
       dbyte = dbyte << 1;
    }
}

void CS5550_Write_Reg(UINT8 regn,UINT32 ld)
{ 
   utemp.rlong=ld;  
   CS5550_Write_Byte(ACT_WRITE+(regn<<1));  
   CS5550_Write_Byte(utemp.bytes.byte2);
   CS5550_Write_Byte(utemp.bytes.byte1);
   CS5550_Write_Byte(utemp.bytes.byte0); 
}

UINT32 CS5550_Read_Reg(UINT8 regn)
{  
   CS5550_Write_Byte(ACT_READ+(regn<<1)); 
   utemp.bytes.byte3=0;
   utemp.bytes.byte2=CS5550_Read_Byte();
   utemp.bytes.byte1=CS5550_Read_Byte();
   utemp.bytes.byte0=CS5550_Read_Byte();  
   return utemp.rlong;
}


void CS5550_Reset(void)
{  CS5550_CS = 0;
   CS5550_Sync();
   CS5550_Write_Byte( SOFTWARE_RESET );
   delay(10);   
   CS5550_Write_Reg(REG_CONFIG, AIN1_GAIN_10+CLK_DIV_1);
   CS5550_Write_Reg(REG_CYCLE_COUNT, 40);  // 100 SPS
   
   CS5550_Write_Byte(START_CONTINUOUS);       
}







