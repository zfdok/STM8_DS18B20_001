#include "uart1.h"
#include "iostm8s208mb.h"
#include "stdio.h"

void uart1_init(void)
{
  UART1_CR1=0X00;
  UART1_CR3=0X00;
  UART1_BRR2 = 0x02;
  UART1_BRR1 = 0x68;
  UART1_CR2=0X6C;
}
void uart1_sendByte(u8 data)
{
  UART1_SR &= 0XBF;
  UART1_DR=data;
  while(!(UART1_SR &0X40));
  UART1_SR &= 0XBF;
}
int putchar(int ch)
{
  while((UART1_SR & 0X80)==0x00);
  uart1_sendByte((u8)ch);
  return (ch);
}
//---------------interrupt-----------------

//-------------------------------------
void delay(u16 Count)
{
  do{ }
  while(Count--);
}