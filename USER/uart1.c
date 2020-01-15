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
/****************************************************************/
//UART1发送字符串函数UART1_SendString()，有形参Data和len，无返回值
//形参Data是字符串数据，len是字符串的长度
//引用举例：UART1_SendString("思修电子",sizeof("思修电子"))
/****************************************************************/
void UART1_SendString(u8* Data,u16 len)
{
  u16 i=0;//定义循环变量i用于控制字符串逐个字符打印
  for(;i<len;i++)
    uart1_sendByte(Data[i]);//逐个发送字符数据
}
/****************************************************************/
//UART1接收字节数据函数UART1_ReceiveByte()，无形参，有返回值
/****************************************************************/
u8 UART1_ReceiveByte(void)
{
  u8 RX_data;//定义接收数据暂存变量
  while (!(UART1_SR&0x20));//若有数据需要被接收
    RX_data=(u8)UART1_DR;//取出数据赋值给接收数据暂存变量
  return  RX_data;//返回接收数据
}
