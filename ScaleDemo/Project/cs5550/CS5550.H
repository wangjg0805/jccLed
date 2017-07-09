sbit CS5550_CS   = P1^5;
sbit CS5550_SCLK = P1^6;
sbit CS5550_SDI  = P1^7;
sbit CS5550_SDO  = P3^2;

#define SYNC0             0xfe
#define SYNC1             0xff

#define START_SINGLE      0xe0
#define START_CONTINUOUS  0xe8

#define POWERUP_HALT      0xa0

#define SOFTWARE_RESET    0x80
#define STANDY            0x88
#define SLEEP             0x90


#define CAL               0xc0
#define CAL_AIN2          (0x01<<4)
#define CAL_AIN1          (0x01<<3)

#define CAL_GAIN          (0x01<<1)
#define CAL_OFFSET        (0x01<<0) 

#define ACT_READ                       0x00
#define ACT_WRITE                      0x40

#define REG_CONFIG                     0X00
#define REG_AIN1_OFFSET                0X01   
#define REG_AIN1_GAIN                  0X02
#define REG_AIN2_OFFSET                0X03
#define REG_AIN2_GAIN                  0X04
#define REG_CYCLE_COUNT                0X05

#define REG_AIN1_OUT                   0X07 
#define REG_AIN2_OUT                   0X08

#define REG_AIN1_FLT                   0X0B      
#define REG_AIN2_FLT                   0X0C

#define REG_STATUS                     0X0F
#define REG_MASK                       0x1A
#define REG_CONTROL                    0X1C  

// REG_CONFIG
#define AIN1_GAIN_10                   (0x00L << 16) 
#define AIN1_GAIN_50                   (0x01L << 16)
#define CLK_DIV_1                      0x01L                        
#define CLK_DIV_2                      0x02L     
// REG_STATUS
#define STAT_DRDY                      (0x01L << 23 )
#define STAT_CRDY                      (0x01L << 20 )
#define STAT_OR1                       (0x01L << 17 )
#define STAT_OR2                       (0x01L << 16 )
#define STAT_FOR1                      (0x01L << 14 )
#define STAT_FOR2                      (0x01L << 13 )
#define STAT_OD2                       (0x01L << 5 )
#define STAT_OD1                       (0x01L << 4 )
void CS5550_Sync(void);
void CS5550_Reset(void);
UINT32 CS5550_Read_Reg(UINT8 regn);
void CS5550_Write_Reg(UINT8 regn,UINT32 ld);
void CS5550_Write_Byte(UINT8 dbyte);


