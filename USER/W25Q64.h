/******************************************************************/
// #include "iostm8s208mb.h"//����оƬ��ͷ�ļ�
// #include "stdio.h"//��Ҫʹ��printf()�����ʶ�������ͷ�ļ�
// #include "stdlib.h"
#include "tim4.h"
/***********************�˿�/���Ŷ�������************************/
#define FLASH_CS       PE_ODR_ODR5//Ӳ������FLASHƬѡ����оƬ
/***********************�û��Զ�����������***********************/
// #define HSIClockFreq 16000000 //ϵͳʱ��Ƶ�ʣ���λΪHz
// #define BaudRate  9600//���趨������

#define countof(a) (sizeof(a)/sizeof(*(a)))

#define  BufferSize (countof(FLASH_Tx_Buffer)-1)

static u32 FLASH_ID; //ȫ�ֱ���FLASH_ID���ڴ��FLASHоƬ��ID��
/*************************W25Q16��������**************************/
#define WREN             0x06  //��W25Q16дʹ������
#define WDIS             0x04  //��W25Q16д��ֹ����
#define RDSR             0x05  //��W25Q16״̬�Ĵ�������
#define WRSR             0x01  //дW25Q16״̬�Ĵ�������
#define READ             0x03  //��W25Q16�ж�ȡ��������
#define FASTREAD         0x0B  //��W25Q16�п��ٶ�ȡ��������
#define FastRead_DualOut 0x3B  //��W25Q16�п��ٶ�ȡ˫�����������
#define WRITE            0x02  //��W25Q16ҳ�������
#define Block_E          0xD8  //���������
#define Sector_E         0x20  //������������
#define Chip_E           0xC7  //оƬ��������
#define PowerD           0xB9  //оƬ��������
#define RPowerD_ID       0xAB  //оƬ�����ͷ�/����ID����
#define Manufacturer_ID  0x90  //оƬ������/����ID����
#define JEDEC_ID         0x9F  //оƬJEDEC ID��������
#define Dummy_Byte       0xFF  //�Զ���α�ֽڣ�FF�����ȡ�ģ� 
/************************������������*************************/
void SPI_FLASH_GPIO_Init();//����SPI����ģʽ��ʹ�ܴӻ�Ƭѡ��������
void SPI_CONFIG(void);//SPIģ�����ú�������
u8 SPI_SendByte(u8 byte);//SPIģ�鷢���ֽں�������
void SPI_FLASH_WriteEnable(void);//FLASHдʹ�ܺ�������
void SPI_FLASH_WaitForWriteEnd(void);//��FlashоƬ״̬�Ĵ�����д���ڽ�����������
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
//��FLASHд��ҳ���ݺ�������
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
//��FLASHд���ҳ���ݺ�������
void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead);
//��FLASH��ȡN�ֽڵ����ݺ�������
u32 SPI_FLASH_ReadID(void);//��ȡFLASHоƬID���к�������
void SPI_FLASH_BlockErase(u32 BlockAddr);//FLASHоƬ�������������
void SPI_FLASH_ChipErase(void);//��������FLASHоƬ���ݺ���

void FLASH_Write_TMP(long date,int time,u8 isNegative,float temp,long offset_adress);//дʱ����¶�
void FLASH_Read_TMP(long offset_adress); //��ʱ����¶�

void FLASH_Read_One_Process(long offset_now);