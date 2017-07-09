#ifndef	_I2C_H_
#define	_I2C_H_

#define AT24C16_ADDR	0xA0

//定义I2C0的时钟线和数据线
#define I2C_SCL		        GPIO_PIN_1
#define I2C_SDA		        GPIO_PIN_2

//定义I2C0基本操作
#define SET_I2C_INPUT		GPIO_Init(GPIOE,I2C_SDA,       GPIO_MODE_IN_PU_NO_IT)
#define SET_I2C_OUTPUT		GPIO_Init(GPIOE,I2C_SDA,   GPIO_MODE_OUT_PP_LOW_FAST)

#define SET_I2C_SDA		GPIO_WriteHigh(GPIOE, I2C_SDA)
#define SET_I2C_SCL		GPIO_WriteHigh(GPIOE, I2C_SCL)
#define CLR_I2C_SDA		GPIO_WriteLow(GPIOE,  I2C_SDA)
#define CLR_I2C_SCL		GPIO_WriteLow(GPIOE,  I2C_SCL)

#define READ_I2C_SDA		GPIO_ReadInputPin(GPIOE,I2C_SDA)

void I2c_Init(void);
u8 Write_EEPROM(u16 addr, void *wrptr, u16 wlen);
void Read_EEPROM(u16 addr, void *rdptr, u16 rlen);
#endif	/*_I2C_H_*/