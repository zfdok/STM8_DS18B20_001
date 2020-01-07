#include "tim4.h"

void TIM4_init()
{
    TIM4_PSCR=0X06;     //由main函数得主时钟频率16MHZ 预分配为2的6次方=64  故频率为250KHZ
    TIM4_ARR=0XFA;      //自动装载值250 故每次中断为 250*1/250KHZ=0.001秒
    TIM4_IER=0X01;      //中断使能
    TIM4_CNTR=0XFA;      //刚开始就触发溢出
}