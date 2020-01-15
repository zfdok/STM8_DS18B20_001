#include "iostm8s208mb.h"
#include "stdio.h"
#include "stm_init.h"



void uart1_init(void);
void uart1_sendByte(u8 data);
void UART1_SendString(u8* Data,u16 len);//UART1发送字符串函数声明
u8 UART1_ReceiveByte(void);//UART1接收字节数据函数声明
