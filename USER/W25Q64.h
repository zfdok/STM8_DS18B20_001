/******************************************************************/
// #include "iostm8s208mb.h"//主控芯片的头文件
// #include "stdio.h"//需要使用printf()函数故而包含该头文件
// #include "stdlib.h"
#include "tim4.h"
/***********************端口/引脚定义区域************************/
#define FLASH_CS       PE_ODR_ODR5//硬件分配FLASH片选引脚芯片
/***********************用户自定义数据区域***********************/
// #define HSIClockFreq 16000000 //系统时钟频率，单位为Hz
// #define BaudRate  9600//欲设定波特率

#define countof(a) (sizeof(a)/sizeof(*(a)))

#define  BufferSize (countof(FLASH_Tx_Buffer)-1)

static u32 FLASH_ID; //全局变量FLASH_ID用于存放FLASH芯片的ID号
/*************************W25Q16操作命令**************************/
#define WREN             0x06  //对W25Q16写使能命令
#define WDIS             0x04  //对W25Q16写禁止命令
#define RDSR             0x05  //读W25Q16状态寄存器命令
#define WRSR             0x01  //写W25Q16状态寄存器命令
#define READ             0x03  //从W25Q16中读取数据命令
#define FASTREAD         0x0B  //从W25Q16中快速读取数据命令
#define FastRead_DualOut 0x3B  //从W25Q16中快速读取双输出数据命令
#define WRITE            0x02  //往W25Q16页编程命令
#define Block_E          0xD8  //块擦除命令
#define Sector_E         0x20  //扇区擦除命令
#define Chip_E           0xC7  //芯片擦除命令
#define PowerD           0xB9  //芯片掉电命令
#define RPowerD_ID       0xAB  //芯片掉电释放/器件ID命令
#define Manufacturer_ID  0x90  //芯片制造商/器件ID命令
#define JEDEC_ID         0x9F  //芯片JEDEC ID序列命令
#define Dummy_Byte       0xFF  //自定义伪字节（FF是随便取的） 
/************************函数声明区域*************************/
void SPI_FLASH_GPIO_Init();//配置SPI引脚模式及使能从机片选函数声明
void SPI_CONFIG(void);//SPI模块配置函数声明
u8 SPI_SendByte(u8 byte);//SPI模块发送字节函数声明
void SPI_FLASH_WriteEnable(void);//FLASH写使能函数声明
void SPI_FLASH_WaitForWriteEnd(void);//读Flash芯片状态寄存器至写周期结束函数声明
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
//向FLASH写入页数据函数声明
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
//向FLASH写入多页数据函数声明
void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead);
//从FLASH读取N字节的数据函数声明
u32 SPI_FLASH_ReadID(void);//读取FLASH芯片ID序列函数声明
void SPI_FLASH_BlockErase(u32 BlockAddr);//FLASH芯片块擦除函数声明
void SPI_FLASH_ChipErase(void);//擦除整个FLASH芯片数据函数

void FLASH_Write_TMP(long date,int time,u8 isNegative,float temp,long offset_adress);//写时间和温度
void FLASH_Read_TMP(long offset_adress); //读时间和温度

void FLASH_Read_One_Process(long offset_now);