#include "ds18b20.h"
//------------common-----------------------
#ifndef u8
  #define u8 uint8_t
#endif 
#ifndef u16
  #define u16 uint16_t
#endif 
#ifndef u32
 #define u32 uint32_t
#endif
//------------marco------------------------
//------------funcs------------------------
void Welcome_Msg(void);
//------------global----------------------
char give_to_stm8;
float temp;
extern u8 wflag;
//------------main------------------------
void main( void )
{
  CLK_CKDIVR=0x00;
  delay(10);
  uart1_init();
  Welcome_Msg();
  asm("rim");
  while (1);
}
//------------funcs------------------------
void Welcome_Msg(void)
{
  printf("*************************");
  printf("\r\n");
  printf("choose your cmd pls!");
  printf("\r\n");
  printf("2-read the DS18B20'S TMP!");
  printf("\r\n");
  printf("*************************");
  printf("\r\n");
}
#pragma vector=0X14
__interrupt void UART1_RX_IRQHandler(void)
{
  char rev_data;
  UART1_SR &=0XDF;
  rev_data = UART1_DR;
  give_to_stm8=rev_data;
  switch (give_to_stm8) 
  {
  case '1':
    //read 18b20's ROM;
    DS18B20_reset();
    give_to_stm8=0;
    break;
  case '2':
    //read 18b20's Temp;
    DS18B20_ReadTempRequest();
    // delay(10000);
    temp= DS18B20_ReadTemp();
    give_to_stm8=0;
    if(wflag) {printf("当前温度:%.4f℃\r\n",temp);}
    else{printf("当前温度:-%.4f℃\r\n",temp);}
    
  case 0:
    break;
  default:
    printf("wrong cmd,retry!\r\n");
    give_to_stm8=0;
    Welcome_Msg();
    break;
  }
}