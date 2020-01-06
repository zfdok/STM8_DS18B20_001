#include "DS18B20.h"

void I2C_init(void);
void I2C_SDA_DDR(u8 ddr);
void I2C_Start(void);
void I2C_Stop(void);
u8 I2C_WBit(u8 data);
void I2C_FindSlaver(u8 address);

#define SCL PE_ODR_ODR1
#define SDA_out  PE_ODR_ODR2
#define SDA_in  PE_IDR_IDR2
