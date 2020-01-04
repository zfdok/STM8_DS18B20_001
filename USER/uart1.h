#include "iostm8s208mb.h"
#include "stdio.h"
#include "stm_init.h"



void uart1_init(void);
void uart1_sendByte(u8 data);

void delay(u16 Count);