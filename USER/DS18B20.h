#include "iostm8s208mb.h"
#include "stdio.h"
#include "uart1.h"

#define DS18B20_DQ PI_ODR_ODR4
#define DS18B20_DH PI_IDR_IDR4


void DS18B20_reset(void);
void DS18B20_IO(u8 ds18b20_io);
void DS18B20_Wbyte(u8 data);
u8 DS18B20_RBit(void);
u8 DS18B20_RByte(void);
float DS18B20_ReadTemp(void);
void DS18B20_ReadTempRequest(void);