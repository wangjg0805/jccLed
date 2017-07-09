/********************************************************************************
 *    CS5550.c: C file for CS5550
 *    Describtion: The C file is the definition of CS5550 for the LPC2378
 *                 family microprocessor.
********************************************************************************/
#include  "global.h"
#include  "lpc2400.h"
#include  "CS5550.h"


#define FPCLK_DELAY    (Fpclk/100000)

////////////////////////////////////////////
/* 1 delay */
////////////////////////////////////////////
static void delay (int length)
	{
	  while (length >0)
    	 length--;
	 }
////////////////////////////////////////////
/* 2 System soft reset cmd*/
////////////////////////////////////////////
void CS5550_System_Reset(void)
{
    ad_write_byte(CS5550_POWER_DOWN_CMD);
}
////////////////////////////////////////////
/* 3 CS5550 Serial Port initial*/
////////////////////////////////////////////
void CS5550_SerialPort_Init(void)
  {
	 IO_CS5550_SEL &= 0xFFFF033F; 
     IO_CS5550_DIR |= (CS5550_SCLK|CS5550_SDI|CS5550_RESET);
	 IO_CS5550_DIR &= (~CS5550_SDO); 
     IO_CS5550_MASK &= ~(CS5550_SCLK|CS5550_SDI|CS5550_SDO|CS5550_RESET);
	 IO_CS5550_CLR = CS5550_SCLK|CS5550_SDI|CS5550_SDO|CS5550_RESET;
	 IO_CS5550_SET = CS5550_RESET|CS5550_SDO;
	 delay(100);

	 /*Initial,modify by source code of ad_sample.c*/
	 ad_write_byte(CS5550_SYNC1_CMD);
     ad_write_byte(CS5550_SYNC1_CMD);
     ad_write_byte(CS5550_SYNC1_CMD);
     ad_write_byte(CS5550_SYNC0_CMD);

     CS5550_Register_Write(CS5550_CONFIG_REGISTER_ADDRESS,0x10001L);
     CS5550_Register_Write(CS5550_CYCLE_COUNT_ADDRESS,0x01F4L);
	 CS5550_Register_Write(CS5550_MASK_REGISTER_ADDRESS,0x800001L);
	 CS5550_Register_Write(CS5550_STATE_REGISTER_ADDRESS,0xFFFFFFL);

	 ad_write_byte(CS5550_START_CONTINUE_CONVERT_CMD);	//start continuous conversion
   }

////////////////////////////////////////////
/* 4 CS5550 CS5550_Register_Read*/
////////////////////////////////////////////
uint32 CS5550_Register_Read(uint8 regaddr)
{
    uint32 data;
	data = CS5550_Serial_Read(CS5550_READ_REGISTER_BASIC_CMD|(regaddr<<1));
	return data;
}
////////////////////////////////////////////
/* 5 CS5550_Register_Write*/
////////////////////////////////////////////
uint32 CS5550_Register_Write(uint8 regaddr,uint32 data)
{
	CS5550_Serial_Write(((CS5550_WRITE_REGISTER_BASIC_CMD|(regaddr<<1))<<24)|(data&0xFFFFFF));
	  return 1;
}

////////////////////////////////////////////
/* 6 CS5550_Command_Send*/
////////////////////////////////////////////
uint32  CS5550_Command_Send(uint8 command)
{
    uint32 data = command<<24;
    CS5550_Serial_Write(data);
    return 1;
}

////////////////////////////////////////////
/* 7 CS5550_Serial_Read*/
////////////////////////////////////////////
uint32 CS5550_Serial_Read(uint8 cmd)
{
    uint32 data = 0;
	uint8 *p=((uint8*)&data)+3;
	ad_write_byte(cmd);
	*p--=0;
	*p--=ad_read_byte();
	*p--=ad_read_byte();
	*p=ad_read_byte();
    
    return data;
}

////////////////////////////////////////////
/* 8 CS5550_Serial_Write*/
////////////////////////////////////////////
uint32 CS5550_Serial_Write(uint32 data)
{
    uint8 *p=((uint8*)&data)+3;
	ad_write_byte(*p--);
	ad_write_byte(*p--);
	ad_write_byte(*p--);
	ad_write_byte(*p);
    
    return 1;
}

////////////////////////////////////////////
/* 9 ad_write_byte*/
////////////////////////////////////////////
void ad_write_byte(uint8 dat)
{
	uint8 i;
	unsigned short j;
	for(i=0;i<8;i++)
	{
		IO_CS5550_CLR = CS5550_SCLK;    //Clear SCLK for low level.
		//for(j = 0; j < 4; j++);  //delay 50ns,it must be to debug.
		if((dat & 0x80)>>7)
		    IO_CS5550_SET = CS5550_SDI; //Set SDI for hign level.
		else
		    IO_CS5550_CLR = CS5550_SDI; //Clear SDI for low level.

		dat=dat<<1;
		//for(j = 0; j < 8; j++); //delay 100ns,it must be to debug.
		IO_CS5550_SET = CS5550_SCLK;    //Set SCLK for hign level.
		//for(j = 0; j < 16; j++); //delay 200ns,it must be to debug.
	}
	IO_CS5550_CLR = CS5550_SCLK;       //Clear SCLK for low level.
	for(j = 0; j < 1; j++);  //delay 200ns,it must be to debug.
}

////////////////////////////////////////////
/* 10 ad_read_byte*/
////////////////////////////////////////////
uint8 ad_read_byte()
{
	uint8 i;
	unsigned short j;
	uint8  dat=0;
	for(i=0;i<8;i++)
	{
		IO_CS5550_CLR = CS5550_SCLK;    //Clear SCLK for low level.
		//for(j = 0; j < 4; j++);  //delay 50ns,it must be to debug.
		if(i==7)
		    IO_CS5550_CLR = CS5550_SDI; //Clear SDI for low level.
		else
		    IO_CS5550_SET = CS5550_SDI; //Set SDI for hign level.
	    //for(j = 0; j < 8; j++); //delay 100ns,it must be to debug.
		IO_CS5550_SET = CS5550_SCLK;   //Set SCLK for hign level.
		dat=dat<<1;
		dat |= (IO_CS5550_PIN&CS5550_SDO)>>5;  //Get SDO level
		//for(j = 0; j < 16; j++); //delay 200ns,it must be to debug.
	}
	IO_CS5550_CLR = CS5550_SCLK;       //Clear SCLK for low level.
	for(j = 0; j < 1; j++);  //delay 200ns,it must be to debug.
	return dat;
}
