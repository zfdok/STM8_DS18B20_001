#include "tim4.h"

void TIM4_init()
{
    TIM4_PSCR=0X06;     //��main��������ʱ��Ƶ��16MHZ Ԥ����Ϊ2��6�η�=64  ��Ƶ��Ϊ250KHZ
    TIM4_ARR=0XFA;      //�Զ�װ��ֵ250 ��ÿ���ж�Ϊ 250*1/250KHZ=0.001��
    TIM4_IER=0X01;      //�ж�ʹ��
    TIM4_CNTR=0XFA;      //�տ�ʼ�ʹ������
}