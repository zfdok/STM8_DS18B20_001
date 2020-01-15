#include "W25Q64.h"
#include "OLED_codeTab.h"
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
#define HzToSec 10000
#define Stored_Capture_Sec 60 //ÿ60���¼һ��
//------------funcs------------------------
void Welcome_Msg(void);
//------------global----------------------
char give_to_stm8;
float temp;
extern u8 wflag; 
u16 tim4_ms_count=0,tim4_sec_count=0;
long offset_now=0;
//------------main------------------------
void main( void )
{
  CLK_CKDIVR=0x00;
  delay(10);
  uart1_init();
  I2C_init();
  TIM4_init();
  SPI_FLASH_GPIO_Init();//����SPI����ģʽ��ʹ�ܴӻ�Ƭѡ
  SPI_CONFIG();//��ʼ��SPI��Դģ��
  I2C_FindSlaver(0XA0);//AT24C02
  I2C_FindSlaver(0X78);//OLED12864
  Welcome_Msg();
  OLED_Welcome();
  SPI_FLASH_ChipErase();//��������FLASHоƬ����
  delayms(3000);
  OLED_Display();
  asm("rim");
  TIM4_CR1 |=0x01;

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
    if (temp<80)
    {
      if(wflag) 
      {
        printf("��ǰ�¶�:%.4f��\r\n",temp);
        OLED_ChangeTemp(0,temp);
      }
      else
      {
        printf("��ǰ�¶�:-%.4f��\r\n",temp);
        OLED_ChangeTemp(1,temp);
      }
    }
  case '3':
    OLED_ChangeRightSatus(0);
    break;
  case '4':
    OLED_ChangeRightSatus(1);
    break;
  case '5':
    OLED_ChangeRECSatus(0);
    break;
  case '6':
    OLED_ChangeRECSatus(1);
    break;
  case '7':
    OLED_ChangeBLESatus(0);
    break;
  case '8':
    OLED_ChangeBLESatus(1);
    break;
  case 'a':
    OLED_ChangePowerSatus(100);
    break;
  case 'b':
    OLED_ChangePowerSatus(80);
    break;
  case 'c':
    OLED_ChangePowerSatus(60);
    break;
  case 'd':
    OLED_ChangePowerSatus(40);
    break;
  case 'e':
    OLED_ChangePowerSatus(20);
    break;
  case 'f':
    OLED_ChangePowerSatus(10);
    break;
  case 'g':
    OLED_ChangePowerSatus(0);
    break;
  case 'h':
    TIM4_CR1 &=0xFE;
    TIM4_IER =0;
    FLASH_Read_One_Process(offset_now);
    break;  
  case 0:
    break;
  default:
    printf("wrong cmd,retry!\r\n");
    give_to_stm8=0;
    Welcome_Msg();
    break;
  }
}
#pragma vector=0X19
__interrupt void TIM4_UPD_OVF_IRQHandler(void)
{
  tim4_ms_count++;
  if (tim4_ms_count==HzToSec)
  {
    tim4_sec_count++;
    tim4_ms_count=0;
    if(tim4_sec_count%2==0)
    {
      //read 18b20's Temp;
      DS18B20_ReadTempRequest();
      // delay(10000);
      temp= DS18B20_ReadTemp();
      if (temp<80)
      {
        if(wflag) 
        {
          //printf("��ǰ�¶�:%.4f��\r\n",temp);
          OLED_ChangeTemp(0,temp);
        }
        else
        {
          //printf("��ǰ�¶�:-%.4f��\r\n",temp);
          OLED_ChangeTemp(1,temp);
        }    
      }
    }
    if(tim4_sec_count==Stored_Capture_Sec)
    {
      tim4_sec_count=0;
      //д��洢��
      FLASH_Write_TMP(200115,1150,!wflag,temp,offset_now);
      offset_now +=18;
      if (offset_now>180000)
      {
        offset_now=0;
      }
      OLED_ChangeRECNum(offset_now/18);
      printf("һ������!\r\n");
    }
  }
}