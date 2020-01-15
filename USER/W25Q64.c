#include "W25Q64.h"


u8 FLASH_Tx_Buffer[]="201115110810725\r\n";//定义发送内容数组
u8 Rx_Buffer[BufferSize];//定义接收数组
/****************************************************************/
//配置SPI引脚模式及使能从机片选函数SPI_FLASH_GPIO_Init()，无形参，无返回值
/****************************************************************/
void SPI_FLASH_GPIO_Init(void)
{
  PC_DDR_DDR5=1;//配置PC5(SPI_SCK)端口为输出模式
  PC_CR1_C15=1;//配置PC5(SPI_SCK)端口为推挽输出模式
  PC_CR2_C25=1;//配置PC5(SPI_SCK)端口高速率输出
  PC_DDR_DDR6=1;//配置PC6(SPI_MOSI)端口为输出模式
  PC_CR1_C16=1;//配置PC6(SPI_MOSI)端口为推挽输出模式
  PC_CR2_C26=1;//配置PC6(SPI_MOSI)端口高速率输出
  PC_DDR_DDR7=0;//配置PC7(SPI_MISO)端口为输入模式
  PC_CR1_C17=1;//配置PC7(SPI_MISO)端口为弱上拉输入模式
  PC_CR2_C27=0;//禁止PC7(SPI_MISO)端口外部中断
  PE_DDR_DDR5=1;//配置PE5端口为输出模式
  PE_CR1_C15=1;//配置PE5端口为推挽输出模式
  PE_CR2_C25=0;//配置PE5端口低速率输出
  FLASH_CS=1;//初始化Flash芯片片选引脚所接的I/O引脚输出高电平
}
/****************************************************************/
//SPI模块配置函数SPI_CONFIG()，无形参，无返回值
/****************************************************************/
void SPI_CONFIG(void)
{
  SPI_CR1=0x07;
  //**************************************************
  //展开SPI_CR1赋值二进制数值为：0000 0111 
  //含义：LSBFIRST=0；先发送最高有效位
  //      SPE=0；禁止SPI设备
  //      BR[2:0]=000；波特率时钟配置为fmaster/2
  //      MSTR=1；配置为主设备
  //      CPOL=1；空闲状态时，SCK保持高电平
  //      CPHA=1；数据采样从第二个时钟边沿开始
  //*************************************************
  SPI_CR2=0x03;  
  //**************************************************
  //展开SPI_CR2赋值二进制数值为：0000 0011 
  //含义：BDM=0；选择双线单向数据模式
  //      BDOE=0；输入使能（只接收模式）
  //      CRCEN=0；CRC计算禁止
  //      CRCNEXT=0；下一个发送的数据来自Tx缓冲区
  //      RXOnly=0；全双工（同时发送和接收）
  //      SSM=1；使能软件从设备管理
  //      SSI=1；主模式
  //*************************************************
  SPI_ICR=0x00;//禁止相关中断               
  SPI_CR1|=0x40;//位或 0x40 的目的在于单独将 SPE 位置“1”  
}
/****************************************************************/
//SPI模块发送字节函数SPI_SendByte()，有形参byte,有返回值
/****************************************************************/
u8 SPI_SendByte(u8 byte)
{
  while(!(SPI_SR&0x02));//等待发送缓冲区为空
  SPI_DR=byte;//将发送的数据写到数据寄存器
  while(!(SPI_SR&0x01));//等待接收缓冲区非空
  return SPI_DR;//返回SPI数据寄存器内容
}
/****************************************************************/
//FLASH写使能函数SPI_FLASH_WriteEnable()，无形参,无返回值
/****************************************************************/
void SPI_FLASH_WriteEnable(void)
{
  FLASH_CS=0;//拉低片选线选中芯片
  SPI_SendByte(WREN);//传送写使能命令06H
  FLASH_CS=1;//拉高片选线不选中芯片
}
/****************************************************************/
//读Flash芯片状态寄存器至写周期结束函数SPI_FLASH_WaitForWriteEnd()
//无形参,无返回值
/****************************************************************/
void SPI_FLASH_WaitForWriteEnd(void)
{
  u8 FLASH_Status=0;//定义Flash芯片状态寄存器值变量
  FLASH_CS=0;//拉低片选线选中芯片
  SPI_SendByte(RDSR);//发送读状态寄存器命令，发送后状态寄存器的值会被传送到STM8 
  //循环查询标志位 等待写周期结束
  do
  {
    FLASH_Status=SPI_SendByte(Dummy_Byte);
    //发送自定伪字节指令0xFF用于生成FLASH芯片时钟并将FLASH状态寄存器值读回STM8
  }
  while((FLASH_Status & 0x01)==1);//等待芯片非忙碌状态
  FLASH_CS=1;//拉高片选线不选中芯片
}
/***************************************************************/
//向FLASH写入页数据函数SPI_FLASH_PageWrite()有3个形参，无返回值
//pBuffer是一个指针，用于指向欲写入数据，WriteAddr用于指定写入地址
//NumByteToWrite用于说明写入数据字节数，所写数据不可超过超过每一页的限制
/**************************************************************/
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
  SPI_FLASH_WriteEnable();//先使能对FLASH芯片的操作
  FLASH_CS=0;//拉低片选线选中芯片
  SPI_SendByte(WRITE);//发送页写命令
  SPI_SendByte((WriteAddr&0xFF0000)>>16);//发送24位FLASH地址,先发高8位
  SPI_SendByte((WriteAddr&0xFF00)>>8);//再发中间8位
  SPI_SendByte(WriteAddr&0xFF);//最后发低8位
  while(NumByteToWrite--)//发送地址后紧跟欲写入数据
  {
    SPI_SendByte(*pBuffer);//发送欲写入FLASH的数据
    pBuffer++;//指向下一个要写入的数据
  }
  FLASH_CS=1;//拉高片选线不选中芯片
  SPI_FLASH_WaitForWriteEnd();//等待写操作结束
}
/***************************************************************/
//向FLASH写入页数据函数SPI_FLASH_BufferWrite()有3个形参，无返回值
//pBuffer是一个指针，用于指向欲写入数据，WriteAddr用于指定写入地址
//NumByteToWrite用于说明写入数据字节数，所写数据不可超过超过每一页的限制
/***************************************************************/
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
  u8 NumOfPage=0,NumOfSingle=0,Addr=0,count=0,temp=0;
  Addr=WriteAddr%256;//判断要写入的地址是否页对齐
  //每一页最多可以写256字节,W25X16共有8192页
  count=256-Addr;
  NumOfPage=NumByteToWrite/256;//总共要写几页
  NumOfSingle=NumByteToWrite%256;//不足一页的数据字节数
  if(Addr==0)//写入的地址是否页对齐
  {
    if (NumOfPage== 0)//只需写一页
    {
      SPI_FLASH_PageWrite(pBuffer,WriteAddr,NumByteToWrite);
    }
    else //不止写一页
    {
      while(NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer,WriteAddr,256);
        WriteAddr+=256;//指向下一页的地址
        pBuffer+=256;
      }
      SPI_FLASH_PageWrite(pBuffer,WriteAddr,NumOfSingle);//把剩下的不足一页的数据写完
    }
  }
  else//要写入的地址不是页对齐的地址
  {
    if (NumOfPage==0)//只需写一页
    {
      if (NumOfSingle>count)
      //判断所要写入的地址所在的页是否还有足够的空间写下要存放的数据 
      { //(NumByteToWrite + WriteAddr) > SPI_FLASH_PageSize
        temp=NumOfSingle-count;
        SPI_FLASH_PageWrite(pBuffer,WriteAddr,count);
        WriteAddr+=count;
        pBuffer+=count;//将所要写入的地址的页剩下的空间写完
        SPI_FLASH_PageWrite(pBuffer,WriteAddr,temp);//再往新的一页写入剩下的数据
      }
      else
      {
        SPI_FLASH_PageWrite(pBuffer,WriteAddr,NumByteToWrite);
      }
    }
    else//不止写一页
    {
      NumByteToWrite-=count;
      NumOfPage=NumByteToWrite/256;//总共要写几页
      NumOfSingle=NumByteToWrite%256;//不足一页的数据字节数
      SPI_FLASH_PageWrite(pBuffer,WriteAddr,count);
      WriteAddr+=count;
      pBuffer+=count;//将所要写入的地址的页剩下的空间写完
      while(NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer,WriteAddr,256);
        WriteAddr+=256;//指向下一页的地址
        pBuffer+=256;
      }
      if(NumOfSingle!=0)
      {
        SPI_FLASH_PageWrite(pBuffer,WriteAddr,NumOfSingle);
      }
    }
  }
}
/***************************************************************/
//从FLASH读取N字节的数据函数SPI_FLASH_BufferRead()有3个形参，无返回值
//pBuffer一个指针，用于存放从FLASH读取的数据缓冲区的指针,ReadAddr用于
//从FLASH的该地址处读数据,NumByteToRead用于指定需要读取的字节数
/***************************************************************/
void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
  FLASH_CS=0;//拉低片选线选中芯片
  SPI_SendByte(READ);//发送读数据命令
  SPI_SendByte((ReadAddr&0xFF0000)>>16);//发送24位FLASH地址,先发高8位
  SPI_SendByte((ReadAddr&0xFF00)>>8);//再发中间8位
  SPI_SendByte(ReadAddr&0xFF);//最后发低8位
  while(NumByteToRead--)//计数
  {
    *pBuffer = SPI_SendByte(Dummy_Byte);//读一个字节的数据
    pBuffer++;//指向下一个要读取的数据
  }
  FLASH_CS=1;//拉高片选线不选中芯片
}
/***************************************************************/
//读取FLASH芯片ID序列函数SPI_FLASH_ReadID()，无形参,有返回值
/***************************************************************/
u32 SPI_FLASH_ReadID(void)
{
  u32 Temp=0,Temp0=0,Temp1=0,Temp2=0;
  FLASH_CS=0;//拉低片选线选中芯片
  SPI_SendByte(JEDEC_ID);//发送读取芯片ID命令
  Temp0 = SPI_SendByte(Dummy_Byte);//从FLASH中读取第1个字节数据
  Temp1 = SPI_SendByte(Dummy_Byte);//从FLASH中读取第2个字节数据
  Temp2 = SPI_SendByte(Dummy_Byte);//从FLASH中读取第3个字节数据
  FLASH_CS=1;//拉高片选线不选中芯片
  Temp=(Temp0<<16)|(Temp1<<8)|Temp2;//拼合数据组成芯片ID码序列
  return Temp;//返回ID序列
}
/***************************************************************/
//FLASH芯片块擦除函数SPI_FLASH_BlockErase()
//有形参BlockAddr用于指定块区域地址，无返回值
/***************************************************************/
void SPI_FLASH_BlockErase(u32 BlockAddr)
{
  SPI_FLASH_WriteEnable();//FLASH写使能
  FLASH_CS=0;//拉低片选线选中芯片
  SPI_SendByte(Block_E);//发送块擦除命令，随后发送要擦除的段地址
  SPI_SendByte((BlockAddr&0xFF0000)>>16);//发送24位FLASH地址,先发高8位
  SPI_SendByte((BlockAddr&0xFF00)>>8);//再发中间8位
  SPI_SendByte(BlockAddr&0xFF);//最后发低8位
  FLASH_CS=1;//拉高片选线不选中芯片
  SPI_FLASH_WaitForWriteEnd();//等待块清除操作完成
}
/***************************************************************/
//擦除整个FLASH芯片数据函数SPI_FLASH_ChipErase()，无形参，无返回值
/***************************************************************/
void SPI_FLASH_ChipErase(void)
{
  SPI_FLASH_WriteEnable();//FLASH写使能
  FLASH_CS=0;//拉低片选线选中芯片
  SPI_SendByte(Chip_E);//发送芯片擦除命令
  FLASH_CS=1;//拉高片选线不选中芯片
  SPI_FLASH_WaitForWriteEnd();//等待写操作完成
}
void FLASH_Write_TMP(long date,int time,u8 isNegative,float temp,long offset_adress)
{
  FLASH_Tx_Buffer[0]=48+(date%1000000)/100000;
  FLASH_Tx_Buffer[1]=48+(date%100000)/10000;
  FLASH_Tx_Buffer[2]=48+(date%10000)/1000;
  FLASH_Tx_Buffer[3]=48+(date%1000)/100;
  FLASH_Tx_Buffer[4]=48+(date%100)/10;
  FLASH_Tx_Buffer[5]=48+date%10;
  
  FLASH_Tx_Buffer[6]=48+(time%10000)/1000;
  FLASH_Tx_Buffer[7]=48+(time%1000)/100;
  FLASH_Tx_Buffer[8]=48+(time%100)/10;
  FLASH_Tx_Buffer[9]=48+time%10;

  if(isNegative)
    FLASH_Tx_Buffer[10]=49;
  else
  FLASH_Tx_Buffer[10]=48;
  FLASH_Tx_Buffer[11]=48+(((int)(temp*100))%10000)/1000;
  FLASH_Tx_Buffer[12]=48+(((int)(temp*100))%1000)/100;
  FLASH_Tx_Buffer[13]=48+(((int)(temp*100))%100)/10;
  FLASH_Tx_Buffer[14]=48+((int)(temp*100))%10;
  printf("|---------------------------------------------------|\r\n");
  printf("|【系统提示】:写入数据,在FLASH地址:0x000000+");
  printf("%ld\r\n",offset_adress);
  printf("|【系统提示】:写入长度为:%d 的数据\r\n",BufferSize);
  SPI_FLASH_BufferWrite(FLASH_Tx_Buffer,0x000000+offset_adress, BufferSize);//对W25X16进行写入
  printf("|【系统提示】:正在向FLASH芯片写入数据为:\r\n");
  UART1_SendString(FLASH_Tx_Buffer,BufferSize);//打印写入数据
}
void FLASH_Read_TMP(long offset_adress)
{
  printf("|---------------------------------------------------|\r\n");
  SPI_FLASH_BufferRead(Rx_Buffer,0x000000+offset_adress,BufferSize);//对W25X16进行读取
  printf("|【系统提示】:读取数据,在FLASH地址:0x000000+");
  printf("%ld\r\n",offset_adress);
  printf("|【系统提示】:读取长度为:%d 的数据\r\n",BufferSize);
  UART1_SendString(Rx_Buffer,BufferSize);//打印读出数据
  printf("|***************************************************|\r\n");
}

void FLASH_Read_One_Process(long offset_now)
{
  for (u16 i = 0; i < offset_now/18; i++)
  {
    FLASH_Read_TMP(i*18);
  }
}