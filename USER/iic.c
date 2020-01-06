#include "iic.h"

void I2C_init(void)
{
    PE_DDR_DDR1=1;
    PE_CR1_C11=1;
    PE_CR2_C21=0;
}
void I2C_SDA_DDR(u8 ddr)
{
    if (ddr)
    {
        PE_DDR_DDR2=1;
        PE_CR1_C12=1;
        PE_CR2_C22=0;
    }
    else
    {
        PE_DDR_DDR2=0;
        PE_CR1_C12=1;
        PE_CR2_C22=0;
    }
}
void I2C_Start(void)
{
    I2C_SDA_DDR(1);
    SDA_out=1;
    SCL=1;
    delay(1);
    SDA_out=0;
    delay(1);
    SCL=0;
    delay(1);
}
void I2C_Stop(void)
{
    I2C_SDA_DDR(1);
    SDA_out=0;
    SCL=0;
    delay(1);
    SCL=1;
    delay(1);
    SDA_out=1;
    delay(1);
}
u8 I2C_WBit(u8 data)
{
    u8 num=0,iic_wbit_ACK=0;
    I2C_SDA_DDR(1);
    for (num = 0X80;num!=0;num >>= 1)
    {
        if(data & num) {SDA_out=1;}
        else {SDA_out=0;}
        delay(1);
        SCL=1;        //SCL=1;保持数据，从端采集
        delay(1);
        SCL=0;        //SCL=0;又可以改写SDA数据了
        delay(1);
    }
    SDA_out=1;
    delay(1);
    I2C_SDA_DDR(0);
    SCL=1;
    delay(1);
    iic_wbit_ACK=SDA_in;
    delay(1);
    SCL=0;
    return iic_wbit_ACK;
}
/*********************寻找IIC从机************************************/
void I2C_FindSlaver(u8 address)
{
    u8 ACK=0;
    printf("测试IIC寻址at24c02:\r\n");
    I2C_Start();
    ACK=I2C_WBit(0XA0);
    if (ACK)
    {
        printf("there's no slaver at 0x%x\r\n",address);
    }
    else
    {
        printf("FIND a slaver at 0x%x!\r\n",address);
    }
}