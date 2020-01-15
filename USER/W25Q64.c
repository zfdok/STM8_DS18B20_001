#include "W25Q64.h"


u8 FLASH_Tx_Buffer[]="201115110810725\r\n";//���巢����������
u8 Rx_Buffer[BufferSize];//�����������
/****************************************************************/
//����SPI����ģʽ��ʹ�ܴӻ�Ƭѡ����SPI_FLASH_GPIO_Init()�����βΣ��޷���ֵ
/****************************************************************/
void SPI_FLASH_GPIO_Init(void)
{
  PC_DDR_DDR5=1;//����PC5(SPI_SCK)�˿�Ϊ���ģʽ
  PC_CR1_C15=1;//����PC5(SPI_SCK)�˿�Ϊ�������ģʽ
  PC_CR2_C25=1;//����PC5(SPI_SCK)�˿ڸ��������
  PC_DDR_DDR6=1;//����PC6(SPI_MOSI)�˿�Ϊ���ģʽ
  PC_CR1_C16=1;//����PC6(SPI_MOSI)�˿�Ϊ�������ģʽ
  PC_CR2_C26=1;//����PC6(SPI_MOSI)�˿ڸ��������
  PC_DDR_DDR7=0;//����PC7(SPI_MISO)�˿�Ϊ����ģʽ
  PC_CR1_C17=1;//����PC7(SPI_MISO)�˿�Ϊ����������ģʽ
  PC_CR2_C27=0;//��ֹPC7(SPI_MISO)�˿��ⲿ�ж�
  PE_DDR_DDR5=1;//����PE5�˿�Ϊ���ģʽ
  PE_CR1_C15=1;//����PE5�˿�Ϊ�������ģʽ
  PE_CR2_C25=0;//����PE5�˿ڵ��������
  FLASH_CS=1;//��ʼ��FlashоƬƬѡ�������ӵ�I/O��������ߵ�ƽ
}
/****************************************************************/
//SPIģ�����ú���SPI_CONFIG()�����βΣ��޷���ֵ
/****************************************************************/
void SPI_CONFIG(void)
{
  SPI_CR1=0x07;
  //**************************************************
  //չ��SPI_CR1��ֵ��������ֵΪ��0000 0111 
  //���壺LSBFIRST=0���ȷ��������Чλ
  //      SPE=0����ֹSPI�豸
  //      BR[2:0]=000��������ʱ������Ϊfmaster/2
  //      MSTR=1������Ϊ���豸
  //      CPOL=1������״̬ʱ��SCK���ָߵ�ƽ
  //      CPHA=1�����ݲ����ӵڶ���ʱ�ӱ��ؿ�ʼ
  //*************************************************
  SPI_CR2=0x03;  
  //**************************************************
  //չ��SPI_CR2��ֵ��������ֵΪ��0000 0011 
  //���壺BDM=0��ѡ��˫�ߵ�������ģʽ
  //      BDOE=0������ʹ�ܣ�ֻ����ģʽ��
  //      CRCEN=0��CRC�����ֹ
  //      CRCNEXT=0����һ�����͵���������Tx������
  //      RXOnly=0��ȫ˫����ͬʱ���ͺͽ��գ�
  //      SSM=1��ʹ��������豸����
  //      SSI=1����ģʽ
  //*************************************************
  SPI_ICR=0x00;//��ֹ����ж�               
  SPI_CR1|=0x40;//λ�� 0x40 ��Ŀ�����ڵ����� SPE λ�á�1��  
}
/****************************************************************/
//SPIģ�鷢���ֽں���SPI_SendByte()�����β�byte,�з���ֵ
/****************************************************************/
u8 SPI_SendByte(u8 byte)
{
  while(!(SPI_SR&0x02));//�ȴ����ͻ�����Ϊ��
  SPI_DR=byte;//�����͵�����д�����ݼĴ���
  while(!(SPI_SR&0x01));//�ȴ����ջ������ǿ�
  return SPI_DR;//����SPI���ݼĴ�������
}
/****************************************************************/
//FLASHдʹ�ܺ���SPI_FLASH_WriteEnable()�����β�,�޷���ֵ
/****************************************************************/
void SPI_FLASH_WriteEnable(void)
{
  FLASH_CS=0;//����Ƭѡ��ѡ��оƬ
  SPI_SendByte(WREN);//����дʹ������06H
  FLASH_CS=1;//����Ƭѡ�߲�ѡ��оƬ
}
/****************************************************************/
//��FlashоƬ״̬�Ĵ�����д���ڽ�������SPI_FLASH_WaitForWriteEnd()
//���β�,�޷���ֵ
/****************************************************************/
void SPI_FLASH_WaitForWriteEnd(void)
{
  u8 FLASH_Status=0;//����FlashоƬ״̬�Ĵ���ֵ����
  FLASH_CS=0;//����Ƭѡ��ѡ��оƬ
  SPI_SendByte(RDSR);//���Ͷ�״̬�Ĵ���������ͺ�״̬�Ĵ�����ֵ�ᱻ���͵�STM8 
  //ѭ����ѯ��־λ �ȴ�д���ڽ���
  do
  {
    FLASH_Status=SPI_SendByte(Dummy_Byte);
    //�����Զ�α�ֽ�ָ��0xFF��������FLASHоƬʱ�Ӳ���FLASH״̬�Ĵ���ֵ����STM8
  }
  while((FLASH_Status & 0x01)==1);//�ȴ�оƬ��æµ״̬
  FLASH_CS=1;//����Ƭѡ�߲�ѡ��оƬ
}
/***************************************************************/
//��FLASHд��ҳ���ݺ���SPI_FLASH_PageWrite()��3���βΣ��޷���ֵ
//pBuffer��һ��ָ�룬����ָ����д�����ݣ�WriteAddr����ָ��д���ַ
//NumByteToWrite����˵��д�������ֽ�������д���ݲ��ɳ�������ÿһҳ������
/**************************************************************/
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
  SPI_FLASH_WriteEnable();//��ʹ�ܶ�FLASHоƬ�Ĳ���
  FLASH_CS=0;//����Ƭѡ��ѡ��оƬ
  SPI_SendByte(WRITE);//����ҳд����
  SPI_SendByte((WriteAddr&0xFF0000)>>16);//����24λFLASH��ַ,�ȷ���8λ
  SPI_SendByte((WriteAddr&0xFF00)>>8);//�ٷ��м�8λ
  SPI_SendByte(WriteAddr&0xFF);//��󷢵�8λ
  while(NumByteToWrite--)//���͵�ַ�������д������
  {
    SPI_SendByte(*pBuffer);//������д��FLASH������
    pBuffer++;//ָ����һ��Ҫд�������
  }
  FLASH_CS=1;//����Ƭѡ�߲�ѡ��оƬ
  SPI_FLASH_WaitForWriteEnd();//�ȴ�д��������
}
/***************************************************************/
//��FLASHд��ҳ���ݺ���SPI_FLASH_BufferWrite()��3���βΣ��޷���ֵ
//pBuffer��һ��ָ�룬����ָ����д�����ݣ�WriteAddr����ָ��д���ַ
//NumByteToWrite����˵��д�������ֽ�������д���ݲ��ɳ�������ÿһҳ������
/***************************************************************/
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
  u8 NumOfPage=0,NumOfSingle=0,Addr=0,count=0,temp=0;
  Addr=WriteAddr%256;//�ж�Ҫд��ĵ�ַ�Ƿ�ҳ����
  //ÿһҳ������д256�ֽ�,W25X16����8192ҳ
  count=256-Addr;
  NumOfPage=NumByteToWrite/256;//�ܹ�Ҫд��ҳ
  NumOfSingle=NumByteToWrite%256;//����һҳ�������ֽ���
  if(Addr==0)//д��ĵ�ַ�Ƿ�ҳ����
  {
    if (NumOfPage== 0)//ֻ��дһҳ
    {
      SPI_FLASH_PageWrite(pBuffer,WriteAddr,NumByteToWrite);
    }
    else //��ֹдһҳ
    {
      while(NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer,WriteAddr,256);
        WriteAddr+=256;//ָ����һҳ�ĵ�ַ
        pBuffer+=256;
      }
      SPI_FLASH_PageWrite(pBuffer,WriteAddr,NumOfSingle);//��ʣ�µĲ���һҳ������д��
    }
  }
  else//Ҫд��ĵ�ַ����ҳ����ĵ�ַ
  {
    if (NumOfPage==0)//ֻ��дһҳ
    {
      if (NumOfSingle>count)
      //�ж���Ҫд��ĵ�ַ���ڵ�ҳ�Ƿ����㹻�Ŀռ�д��Ҫ��ŵ����� 
      { //(NumByteToWrite + WriteAddr) > SPI_FLASH_PageSize
        temp=NumOfSingle-count;
        SPI_FLASH_PageWrite(pBuffer,WriteAddr,count);
        WriteAddr+=count;
        pBuffer+=count;//����Ҫд��ĵ�ַ��ҳʣ�µĿռ�д��
        SPI_FLASH_PageWrite(pBuffer,WriteAddr,temp);//�����µ�һҳд��ʣ�µ�����
      }
      else
      {
        SPI_FLASH_PageWrite(pBuffer,WriteAddr,NumByteToWrite);
      }
    }
    else//��ֹдһҳ
    {
      NumByteToWrite-=count;
      NumOfPage=NumByteToWrite/256;//�ܹ�Ҫд��ҳ
      NumOfSingle=NumByteToWrite%256;//����һҳ�������ֽ���
      SPI_FLASH_PageWrite(pBuffer,WriteAddr,count);
      WriteAddr+=count;
      pBuffer+=count;//����Ҫд��ĵ�ַ��ҳʣ�µĿռ�д��
      while(NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer,WriteAddr,256);
        WriteAddr+=256;//ָ����һҳ�ĵ�ַ
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
//��FLASH��ȡN�ֽڵ����ݺ���SPI_FLASH_BufferRead()��3���βΣ��޷���ֵ
//pBufferһ��ָ�룬���ڴ�Ŵ�FLASH��ȡ�����ݻ�������ָ��,ReadAddr����
//��FLASH�ĸõ�ַ��������,NumByteToRead����ָ����Ҫ��ȡ���ֽ���
/***************************************************************/
void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
  FLASH_CS=0;//����Ƭѡ��ѡ��оƬ
  SPI_SendByte(READ);//���Ͷ���������
  SPI_SendByte((ReadAddr&0xFF0000)>>16);//����24λFLASH��ַ,�ȷ���8λ
  SPI_SendByte((ReadAddr&0xFF00)>>8);//�ٷ��м�8λ
  SPI_SendByte(ReadAddr&0xFF);//��󷢵�8λ
  while(NumByteToRead--)//����
  {
    *pBuffer = SPI_SendByte(Dummy_Byte);//��һ���ֽڵ�����
    pBuffer++;//ָ����һ��Ҫ��ȡ������
  }
  FLASH_CS=1;//����Ƭѡ�߲�ѡ��оƬ
}
/***************************************************************/
//��ȡFLASHоƬID���к���SPI_FLASH_ReadID()�����β�,�з���ֵ
/***************************************************************/
u32 SPI_FLASH_ReadID(void)
{
  u32 Temp=0,Temp0=0,Temp1=0,Temp2=0;
  FLASH_CS=0;//����Ƭѡ��ѡ��оƬ
  SPI_SendByte(JEDEC_ID);//���Ͷ�ȡоƬID����
  Temp0 = SPI_SendByte(Dummy_Byte);//��FLASH�ж�ȡ��1���ֽ�����
  Temp1 = SPI_SendByte(Dummy_Byte);//��FLASH�ж�ȡ��2���ֽ�����
  Temp2 = SPI_SendByte(Dummy_Byte);//��FLASH�ж�ȡ��3���ֽ�����
  FLASH_CS=1;//����Ƭѡ�߲�ѡ��оƬ
  Temp=(Temp0<<16)|(Temp1<<8)|Temp2;//ƴ���������оƬID������
  return Temp;//����ID����
}
/***************************************************************/
//FLASHоƬ���������SPI_FLASH_BlockErase()
//���β�BlockAddr����ָ���������ַ���޷���ֵ
/***************************************************************/
void SPI_FLASH_BlockErase(u32 BlockAddr)
{
  SPI_FLASH_WriteEnable();//FLASHдʹ��
  FLASH_CS=0;//����Ƭѡ��ѡ��оƬ
  SPI_SendByte(Block_E);//���Ϳ������������Ҫ�����Ķε�ַ
  SPI_SendByte((BlockAddr&0xFF0000)>>16);//����24λFLASH��ַ,�ȷ���8λ
  SPI_SendByte((BlockAddr&0xFF00)>>8);//�ٷ��м�8λ
  SPI_SendByte(BlockAddr&0xFF);//��󷢵�8λ
  FLASH_CS=1;//����Ƭѡ�߲�ѡ��оƬ
  SPI_FLASH_WaitForWriteEnd();//�ȴ�������������
}
/***************************************************************/
//��������FLASHоƬ���ݺ���SPI_FLASH_ChipErase()�����βΣ��޷���ֵ
/***************************************************************/
void SPI_FLASH_ChipErase(void)
{
  SPI_FLASH_WriteEnable();//FLASHдʹ��
  FLASH_CS=0;//����Ƭѡ��ѡ��оƬ
  SPI_SendByte(Chip_E);//����оƬ��������
  FLASH_CS=1;//����Ƭѡ�߲�ѡ��оƬ
  SPI_FLASH_WaitForWriteEnd();//�ȴ�д�������
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
  printf("|��ϵͳ��ʾ��:д������,��FLASH��ַ:0x000000+");
  printf("%ld\r\n",offset_adress);
  printf("|��ϵͳ��ʾ��:д�볤��Ϊ:%d ������\r\n",BufferSize);
  SPI_FLASH_BufferWrite(FLASH_Tx_Buffer,0x000000+offset_adress, BufferSize);//��W25X16����д��
  printf("|��ϵͳ��ʾ��:������FLASHоƬд������Ϊ:\r\n");
  UART1_SendString(FLASH_Tx_Buffer,BufferSize);//��ӡд������
}
void FLASH_Read_TMP(long offset_adress)
{
  printf("|---------------------------------------------------|\r\n");
  SPI_FLASH_BufferRead(Rx_Buffer,0x000000+offset_adress,BufferSize);//��W25X16���ж�ȡ
  printf("|��ϵͳ��ʾ��:��ȡ����,��FLASH��ַ:0x000000+");
  printf("%ld\r\n",offset_adress);
  printf("|��ϵͳ��ʾ��:��ȡ����Ϊ:%d ������\r\n",BufferSize);
  UART1_SendString(Rx_Buffer,BufferSize);//��ӡ��������
  printf("|***************************************************|\r\n");
}

void FLASH_Read_One_Process(long offset_now)
{
  for (u16 i = 0; i < offset_now/18; i++)
  {
    FLASH_Read_TMP(i*18);
  }
}