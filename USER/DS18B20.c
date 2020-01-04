#include "DS18B20.h"

static u16 temp_u16=0;
static float temp_f=0;
u8 wflag=0;

void DS18B20_IO(u8 ds18b20_io)
{
  if (ds18b20_io)
  {
    PI_DDR_DDR4=1;
    PI_CR1_C14=1;
    PI_CR2_C24=0;
  }
  else
  {
    PI_DDR_DDR4=0;
    PI_CR1_C14=1;
    PI_CR2_C24=0;
  }
}
void DS18B20_reset(void)
{
  u8 havenoDS=0,haveflag=0;
  u16 count=700;
  DS18B20_IO(1);
  DS18B20_DQ=1;
  delay(1700);
  DS18B20_DQ=0;
  delay(1700);
  DS18B20_DQ=1;
  DS18B20_IO(0);
  delay(65);
  havenoDS=PI_IDR_IDR4;
  //printf("I'm scaning~\r\n");
  while (count--)
  {
    if (!havenoDS) haveflag=1;
    if (haveflag) break;
  }
  if (!haveflag)
  {
    printf("I got nothing!\r\n");
  }
  delay(1150);
}
void DS18B20_Wbyte(u8 data)
{
  u8 i=0, k=0;
  for (i = 0; i < 8; i++)
  {
    k=data & 0X01;
    DS18B20_IO(1);
    if (k)
    {
      DS18B20_DQ=0;
      delay(30);
      DS18B20_DQ=1;
      delay(100);
    }
    else
    {
      DS18B20_DQ=0;
      delay(130);
      DS18B20_DQ=1;
    }
    data =data>>1;
  }
}
u8 DS18B20_RBit(void)
{
  u8 recive_data=0,x=0;
  DS18B20_IO(1);
  DS18B20_DQ=1;
  delay(2);
  DS18B20_DQ=0;
  delay(5);
  DS18B20_DQ=1;
  DS18B20_IO(0);
  delay(10);
  x=DS18B20_DH;
  delay(150);
  if (x)
  {
    recive_data=0X80;
  }
  else
  {
    recive_data=0;
  }
  return recive_data;
}
u8 DS18B20_RByte(void)
{
  u8 recive_byte=0,temp_byte=0;
  for (u8 i = 0; i < 8; i++)
  {
    temp_byte=DS18B20_RBit();
    recive_byte=recive_byte>>1;
    recive_byte |= temp_byte;
  }
  return recive_byte;
}
void DS18B20_ReadTempRequest(void)
{
    DS18B20_reset();    
    delay(1000);
    DS18B20_Wbyte(0XCC);
    DS18B20_Wbyte(0X44);
}
float DS18B20_ReadTemp(void)
{
  u8 LSB=0,HSB=0;
  DS18B20_reset();
  delay(1000);
  DS18B20_Wbyte(0XCC);
  DS18B20_Wbyte(0XBE);
  delay(1000);
  LSB=DS18B20_RByte();
  HSB=DS18B20_RByte();
  //printf("HSB:%d\r\n",HSB);
  //printf("LSB:%d\r\n",LSB);
  temp_u16=HSB;
  temp_u16=temp_u16 << 8;
  temp_u16 |=LSB;
  //printf("temp_u16:%d\r\n",temp_u16);
  if (temp_u16<=0X07FF)
  {
    wflag=1;
    temp_f=temp_u16*0.0625;
  }
  else
  {
    wflag=0;
    temp_u16=~(temp_u16-1);
    temp_f=temp_u16*0.0625;
  }
  
//  printf("temp_f:%f\r\n",temp_f);
  return temp_f;
}