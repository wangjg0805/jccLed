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
#define CS5550_START_SINGLE_CONVERT_CMD    0xE0
#define CS5550_START_CONTINUE_CONVERT_CMD  0xE8
#define CS5550_SYNC0_CMD                   0xFE
#define CS5550_SYNC1_CMD                   0xFF
#define CS5550_POWER_UP_HALT_CMD           0xA0
#define CS5550_POWER_DOWN_CMD              0x80
#define CS5550_HALT_QUICK_POWER_ON_CMD     0x88
#define CS5550_HALT_SLOW_POWER_ON_CMD      0x90
#define CS5550_CALIBRATION_BASIC_CMD       0xC0
#define CS5550_READ_REGISTER_BASIC_CMD     0x00
#define CS5550_WRITE_REGISTER_BASIC_CMD    0x40
/*define CS5550 Calibration chanel set bits,which is set Calibration basic cmd bit 3 and bit 4*/
#define CS5550_CALIBRATION_AIN1_BITS          0x1
#define CS5550_CALIBRATION_AIN2_BITS          0x2
#define CS5550_CALIBRATION_AIN1_AIN2_BITS     0x3
/*define CS5550 Calibration gain set bit,which is set Calibration basic cmd bit 1*/  
#define CS5550_CALIBRATION_GAIN_NORMAL_BIT    0x0
#define CS5550_CALIBRATION_GAIN_PERFORM_BIT   0x1
/*define CS5550 Calibration offset set bit,which is set Calibration basic cmd bit 0*/
#define CS5550_CALIBRATION_OFFSET_NORMAL_BIT  0x0
#define CS5550_CALIBRATION_OFFSET_PERFORM_BIT 0x1
/*define CS5550 Register address,which is set Read/Write register basic cmd bit 1 to bit 5*/
#define CS5550_CONFIG_REGISTER_ADDRESS        0x00
#define CS5550_AIN1_DCOFF_ADDRESS             0x01
#define CS5550_AIN1_GN_ADDRESS                0x02
#define CS5550_AIN2_DCOFF_ADDRESS             0x03
#define CS5550_AIN2_GN_ADDRESS                0x04
#define CS5550_CYCLE_COUNT_ADDRESS            0x05
#define CS5550_AIN1_OUT_ADDRESS               0x07
#define CS5550_AIN2_OUT_ADDRESS               0x08
#define CS5550_AIN1_FILT_ADDRESS              0x0B
#define CS5550_AIN2_FILT_ADDRESS              0x0C
#define CS5550_STATE_REGISTER_ADDRESS         0x0F
#define CS5550_MASK_REGISTER_ADDRESS          0x1A
#define CS5550_CTRL_REGISTER_ADDRESS          0x1C
/*define SCLK/SDI/SDO/RESET pin position,please modify it for your select pin
*     SCLK     connect     P1.10
*     SDI      connect     P2.11
*     SDO      connect     P2.12
*     RESET    connect     P2.13
*/
#define IO_CS5550_SEL    PINSEL0
#define IO_CS5550_DIR    FIO1DIR
#define IO_CS5550_SET    FIO1SET
#define IO_CS5550_CLR    FIO1CLR
#define IO_CS5550_PIN    FIO1PIN
#define IO_CS5550_MASK   FIO1MASK

#define CS5550_SCLK     (1<<19)            /* SCLK pin  */
#define CS5550_SDI      (1<<19)            /* SDI pin   */
#define CS5550_SDO      (1<<19)            /* SDO pin   */
#define CS5550_RESET    (1<<19)            /* RESET pin */

////////////////////////////////////function
void     CS5550_System_Reset(void);
void     CS5550_SerialPort_Init(void);
uint32   CS5550_Register_Read(uint8 regaddr);
uint32   CS5550_Register_Write(uint8 regaddr,uint32 data);
void     CS5550_Int_Init(void);
void     CS5550_IntHandle_Route(void);
uint32   CS5550_Serial_Read(uint8 cmd);
uint32   CS5550_Serial_Write(uint32 data);
uint32   CS5550_Command_Send(uint8 command);
void     ad_write_byte(uint8 dat);
uint8    ad_read_byte(void);

#endif
