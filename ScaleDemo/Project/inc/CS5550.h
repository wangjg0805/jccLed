/*************************************************************************************
*    CS5550.h:  Header file for CS5550 driver
*    Describtion: The header file is the definition of CS5550 for the CS5550
*                 family microprocessor.
*    Author:    WJG
*    Date:      2013.10.10
*    History:
*
**************************************************************************************/
#ifndef __CS5550_H__
#define __CS5550_H__

/*Define CS5550 Serial Port Command Word */
#define CS5550_SOFT_RESET_CMD                0x80
#define CS5550_SINGLE_CONVERT_CMD            0xE0
#define CS5550_CONTINUE_CONVERT_CMD          0xE8
#define CS5550_SYNC0_CMD                     0xFE
#define CS5550_SYNC1_CMD                     0xFF

#define CS5550_CAL_CH1                       0xC8
#define CS5550_CAL_GAIN                      0x02
#define CS5550_CAL_OFF                       0x01
 
#define CS5550_READ                          0x00
#define CS5550_WRITE                         0x40

//configuration reg bit
#define GAIN_10X                           (0<<16)
#define GAIN_50X                           (1<<16)
//
#define INT_LOW_LEVEL                      (0<<11)
#define INT_HIGH_LEVEL                     (1<<11)
#define INT_LOW_PULSE                      (2<<11)
#define INT_HIGH_PULSE                     (3<<11)
//
#define AIN2_HPF_DISABLE                   (0<<6)
#define AIN2_HPF_ENABLE                    (1<<6)
#define AIN1_HPF_DISABLE                   (0<<5)
#define AIN1_HPF_ENABLE                    (1<<5)
//
#define iCPU_NORMAL                        (0<<4)
#define iCPU_ENABLE                        (1<<4)
//
#define CLK_DIV_1                              1
#define CLK_DIV_16                             0

/*define CS5550 Calibration chanel set bits,which is set Calibration basic cmd bit 3 and bit 4*/
#define CS5550_CAL_AIN1_BITS                  0x1
#define CS5550_CAL_AIN2_BITS                  0x2
#define CS5550_CAL_AIN1_AIN2_BITS             0x3
/*define CS5550 Calibration gain set bit,which is set Calibration basic cmd bit 1*/  
#define CS5550_CAL_GAIN_NORMAL_BIT            0x0
#define CS5550_CAL_GAIN_PERFORM_BIT           0x1
/*define CS5550 Calibration offset set bit,which is set Calibration basic cmd bit 0*/
#define CS5550_CAL_OFFSET_NORMAL_BIT          0x0
#define CS5550_CAL_OFFSET_PERFORM_BIT         0x1
/*define CS5550 Register address,which is set Read/Write register basic cmd bit 1 to bit 5*/
#define CS5550_CONFIG_REG                    (0x00<<1)
#define CS5550_AIN1_OFF                      (0x01<<1)
#define CS5550_AIN1_GAIN                     (0x02<<1)
#define CS5550_AIN2_OFF                      (0x03<<1)
#define CS5550_AIN2_GAIN                     (0x04<<1)

#define CS5550_CYCLE_REG                     (0x05<<1)
#define CS5550_AIN1_OUT                      (0x07<<1)
#define CS5550_AIN2_OUT                      (0x08<<1)
#define CS5550_AIN1_FILT_OUT                 (0x0B<<1)
#define CS5550_AIN2_FILT_OUT                 (0x0C<<1)
#define CS5550_STATE_REG                     (0x0F<<1)
#define CS5550_MASK_REG                      (0x1A<<1)
#define CS5550_CTRL_REG                      (0x1C<<1)

///////////////////////////////////////////
//×´Ì¬¼Ä´æÆ÷Î»ÖÃ±êÖ¾
#define DRDY                                0x00800000
#define CRDY                                0x00100000
#define CS5550_CYCLE_COUNT                    0x0000fa

#define GAIN_4                                0xFFFFFF
#define GAIN_2                                0x800000
#define GAIN_1                                0x400000
#define GAIN_0_5                              0x200000
#define GAIN_0_25                             0x100000
#define GAIN_0_125                            0x080000
#define GAIN_0_0625                           0x040000
#define GAIN_0_03125                          0x020000

/*define SCLK/SDI/SDO/RESET pin position,please modify it for your select pin*/
#define CS5550_INT      GPIO_PIN_6            /* INT pin   B6 */
#define CS5550_SDI      GPIO_PIN_7            /* SDI pin   B7 */
#define CS5550_SCLK     GPIO_PIN_6            /* SCLK pin  A6 */
#define CS5550_SDO      GPIO_PIN_5            /* SDO pin   A5 */

#define CS5550_CS       GPIO_PIN_4            /* CS pin    A4 */ 
#define CS5550_RESET    GPIO_PIN_5            /* RESET pin B5 */

#define CS5550_CLK_HIGH     GPIO_WriteHigh(GPIOA, CS5550_SCLK)
#define CS5550_CLK_LOW       GPIO_WriteLow(GPIOA, CS5550_SCLK)
#define CS5550_SDI_HIGH     GPIO_WriteHigh(GPIOB, CS5550_SDI)
#define CS5550_SDI_LOW       GPIO_WriteLow(GPIOB, CS5550_SDI)

#define READ_CS5550_SDO	GPIO_ReadInputPin(GPIOA,CS5550_SDO)
#define READ_CS5550_INT	GPIO_ReadInputPin(GPIOB,CS5550_INT)
////////////////////////////////////function
extern  const u32 Energy_AMPDATA[8];


void     cs5550_pin_init(void);
void     cs5550_Init(void);
u32      cs5550_read(u8 cmd);
void     cs5550_config(u8 cmd, u32 value);
#endif
