#include "iostm8s208mb.h"
#include "stdio.h"
#include "stm_init.h"



void uart1_init(void);
void uart1_sendByte(u8 data);
void UART1_SendString(u8* Data,u16 len);//UART1�����ַ�����������
u8 UART1_ReceiveByte(void);//UART1�����ֽ����ݺ�������
