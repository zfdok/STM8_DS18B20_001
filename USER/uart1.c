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
//UART1�����ַ�������UART1_SendString()�����β�Data��len���޷���ֵ
//�β�Data���ַ������ݣ�len���ַ����ĳ���
//���þ�����UART1_SendString("˼�޵���",sizeof("˼�޵���"))
/****************************************************************/
void UART1_SendString(u8* Data,u16 len)
{
  u16 i=0;//����ѭ������i���ڿ����ַ�������ַ���ӡ
  for(;i<len;i++)
    uart1_sendByte(Data[i]);//��������ַ�����
}
/****************************************************************/
//UART1�����ֽ����ݺ���UART1_ReceiveByte()�����βΣ��з���ֵ
/****************************************************************/
u8 UART1_ReceiveByte(void)
{
  u8 RX_data;//������������ݴ����
  while (!(UART1_SR&0x20));//����������Ҫ������
    RX_data=(u8)UART1_DR;//ȡ�����ݸ�ֵ�����������ݴ����
  return  RX_data;//���ؽ�������
}
