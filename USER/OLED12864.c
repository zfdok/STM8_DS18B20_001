#include "OLED12864.h"

extern u8 F6x8[][6];
extern u8 F8X16[];
extern u8 F16x16[];
extern u8 F16x32[];
extern u8 F32x32[];
/*********************OLEDд����************************************/
void OLED_Wcmd(u8 cmd)
{
    I2C_Start();
    I2C_WBit(0X78);
    I2C_WBit(0X00);
    I2C_WBit(cmd);
    I2C_Stop();
}
/*********************OLEDд����************************************/
void OLED_Wdata(u8 data)
{
    I2C_Start();
    I2C_WBit(0X78);
    I2C_WBit(0X40);
    I2C_WBit(data);
    I2C_Stop();
}
/*********************OLED������ʾ�����************************************/
void OLED_SetPos(u8 x,u8 y)
{
    OLED_Wcmd(0xb0+y);
    OLED_Wcmd((( x & 0xf0)>>4)|0x10);
    OLED_Wcmd(( x & 0x0f)|0x01);
}
/*********************OLEDȫ��************************************/
void OLED_Fill(u8 bmp_data) 
{
	unsigned char y,x;
	for(y=0;y<8;y++)
	{
		OLED_Wcmd(0xb0+y);
		OLED_Wcmd(0x00);
		OLED_Wcmd(0x10);
		for(x=0;x<128;x++)
		OLED_Wdata(bmp_data);
	}
}
/*********************OLED��λ************************************/
void OLED_CLS(void)
{
	u8 y,x;
	for(y=0;y<8;y++)
	{
		OLED_Wcmd(0xb0+y);
		OLED_Wcmd(0x00);
		OLED_Wcmd(0x10);
		for(x=0;x<128;x++)
		OLED_Wdata(0);
	}
}
/*********************OLED��ʼ��************************************/
void OLED_init(void)
{
    delayms(500);
    OLED_Wcmd(0xae);//--turn off oled panel  //�ر����
    OLED_Wcmd(0x00);//---set low column address    //���õ��е�ַ
    OLED_Wcmd(0x10);//---set high column address   //���ø��е�ַ
    OLED_Wcmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F) //���ÿ�ʼ��ַ
    OLED_Wcmd(0x81);//--set contrast control register  //���õ��е�ַ
    OLED_Wcmd(Brightness); // Set SEG Output Current Brightness  //���öԱȶȿ��ƼĴ���
    OLED_Wcmd(0xa1);//--Set SEG/Column Mapping     //���÷ֶ�/��ӳ��
	OLED_Wcmd(0xc8);//Set COM/Row Scan Direction   ������/��ɨ�跽��
    OLED_Wcmd(0xa6);//--set normal display         ����������ʾ
    OLED_Wcmd(0xa8);//--set multiplex ratio(1 to 64)  ���ö�·���ñ���
    OLED_Wcmd(0x3f);//--1/64 duty  
    OLED_Wcmd(0xd3);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)  ������ʾƫ����λӳ��RAM������
	OLED_Wcmd(0x00);//-not offset  ��ƫ��
	OLED_Wcmd(0xd5);//--set display clock divide ratio/oscillator frequency  ������ʾ��ʱ��/����Ƶ��
	OLED_Wcmd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec ���÷�Ƶ�ȣ���ʱ������Ϊ100֡/��
	OLED_Wcmd(0xd9);//--set pre-charge period ����Ԥ���׶�
	OLED_Wcmd(0xf1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock Ԥ�����Ϊ15��ʱ�ӣ��ŵ���Ϊ1��ʱ��
	OLED_Wcmd(0xda);//--set com pins hardware configuration ����com����Ӳ������
	OLED_Wcmd(0x12);
	OLED_Wcmd(0xdb);//--set vcomh     ����VCOM h
	OLED_Wcmd(0x40);//Set VCOM Deselect Level 
	OLED_Wcmd(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02) ����ҳ��Ѱַģʽ
	OLED_Wcmd(0x02);// 
	OLED_Wcmd(0x8d);//--set Charge Pump enable/disable ���ó�������/����
	OLED_Wcmd(0x14);//--set(0x10) disable              ������
	OLED_Wcmd(0xa4);// Disable Entire Display On (0xa4/0xa5) ����0xa4/0xa5�ϵ�������ʾ
	OLED_Wcmd(0xa6);// Disable Inverse Display On (0xa6/a7)  ����0xa6/a7������ʾ
	OLED_Wcmd(0xaf);//--turn on oled panel //�����
	OLED_CLS(); //��ƽ
	OLED_SetPos(0,0);
}
void OLED_P6x8Str(u8 x,u8 y,u8 ch[])
{
	u8 c=0,i=0,j=0;
	while (ch[j]!='\0')
	{
		c =ch[j]-32;
		if(x>126){x=0;y++;}
		OLED_SetPos(x,y);
		for(i=0;i<6;i++)
        	OLED_Wdata(F6x8[c][i]);
		x+=6;
		j++;
	}
}
void OLED_P8x16Str(u8 x,u8 y,u8 ch[])
{
	unsigned char c=0,i=0,j=0;
	while (ch[j]!='\0')
	{
		c =ch[j]-32;
		if(x>120){x=0;y++;}
		OLED_SetPos(x,y);
		for(i=0;i<8;i++)
			OLED_Wdata(F8X16[c*16+i]);
		OLED_SetPos(x,y+1);
		for(i=0;i<8;i++)
			OLED_Wdata(F8X16[c*16+i+8]);
		x+=8;
		j++;
	}
}
void OLED_P16x16Ch(u8 x,u8 y,u8 N)
{
	u8 wm=0;
	unsigned int adder=32*N;
	OLED_SetPos(x , y);
	for(wm = 0;wm < 16;wm++)
	{
		OLED_Wdata(F16x16[adder]);
		adder += 1;
	}
	OLED_SetPos(x,y + 1);
	for(wm = 0;wm < 16;wm++)
	{
		OLED_Wdata(F16x16[adder]);
		adder += 1;
	} 	  	
}
void OLED_P16x32Ch(u8 x,u8 y,u8 N)
{
	u8 wm=0;
	unsigned int adder=64*N;
	OLED_SetPos(x , y);
	for(wm = 0;wm < 16;wm++)
	{
		OLED_Wdata(F16x32[adder]);
		adder += 1;
	}
	OLED_SetPos(x,y + 1);
	for(wm = 0;wm < 16;wm++)
	{
		OLED_Wdata(F16x32[adder]);
		adder += 1;
	} 	
	OLED_SetPos(x,y + 2);
	for(wm = 0;wm < 16;wm++)
	{
		OLED_Wdata(F16x32[adder]);
		adder += 1;
	} 
	OLED_SetPos(x,y + 3);
	for(wm = 0;wm < 16;wm++)
	{
		OLED_Wdata(F16x32[adder]);
		adder += 1;
	}
}
void OLED_P32x32Ch(u8 x,u8 y,u8 N)
{
	u8 wm=0;
	unsigned int adder=64*N;
	OLED_SetPos(x , y);
	for(wm = 0;wm < 32;wm++)
	{
		OLED_Wdata(F32x32[adder]);
		adder += 1;
	}
	OLED_SetPos(x,y + 1);
	for(wm = 0;wm < 32;wm++)
	{
		OLED_Wdata(F32x32[adder]);
		adder += 1;
	} 	
	OLED_SetPos(x,y + 2);
	for(wm = 0;wm < 32;wm++)
	{
		OLED_Wdata(F32x32[adder]);
		adder += 1;
	} 
	OLED_SetPos(x,y + 3);
	for(wm = 0;wm < 32;wm++)
	{
		OLED_Wdata(F32x32[adder]);
		adder += 1;
	}
}
void OLED_Display()
{
	OLED_init();
	OLED_ChangeRightSatus(1);     	//�жϱ��λ�Ʒ�ϸ���
	OLED_ChangeBLESatus(1);	 		//����״̬
	OLED_P16x16Ch(32,0,12);			//�Ƿ��¼��
	OLED_ChangeTime(7,18); 		//Ĭ��ʱ��7��18
	OLED_ChangePowerSatus(0);		//��ص����ٷֱ�
	OLED_ChangeTemp(0,0);   		//Ĭ���¶�0.0
	OLED_P32x32Ch(96,2,0); 			//��
	OLED_ChangeRECNum(125477);		//�ı��¼����
    OLED_ChangeDate(2020,12,21);    //
}
void OLED_ChangeTemp(u8 under0,float temp)
{
	if (under0)
	{
		OLED_P16x32Ch(0,2,13);
	}
	else
	{
		OLED_P16x32Ch(0,2,17);
	}
	OLED_P16x32Ch(16,2,((int)temp)/10);
	OLED_P16x32Ch(32,2,((int)temp)%10);
	OLED_P16x32Ch(48,2,10);
	OLED_P16x32Ch(64,2,((int)(temp*10))%10);
	OLED_P16x32Ch(80,2,((int)(temp*100))%10);
}
void OLED_ChangeTime(u8 hour,u8 min)
{
	witeNum8X16(64,0,hour/10);
	witeNum8X16(72,0,hour%10);
	OLED_P8x16Str(80,0,":");
	witeNum8X16(88,0,min/10);
	witeNum8X16(94,0,min%10);
}
void OLED_ChangeDate(u16 year,u8 month,u8 day)
{
	witeNum6X8(64,7,year/1000);
	witeNum6X8(70,7,(year%1000)/100);
	witeNum6X8(76,7,(year%100)/10);
	witeNum6X8(82,7,(year%10));
	OLED_P6x8Str(88,7,"-");
	witeNum6X8(92,7,month/10);
	witeNum6X8(98,7,month%10);
	OLED_P6x8Str(104,7,"-");
	witeNum6X8(110,7,day/10);
	witeNum6X8(116,7,day%10);
}
void OLED_ChangeRECNum(long num)
{
	if (num>99999)
	{
		witeNum6X8(0,7,num/100000);
		witeNum6X8(6,7,(num%100000)/10000);
		witeNum6X8(12,7,(num%10000)/1000);
		witeNum6X8(18,7,(num%1000)/100);
		witeNum6X8(24,7,(num%100)/10);
		witeNum6X8(30,7,num%10);
	}
	if (num>9999 & num<100000 )
	{
		witeNum6X8(0,7,(num%100000)/10000);
		witeNum6X8(6,7,(num%10000)/1000);
		witeNum6X8(12,7,(num%1000)/100);
		witeNum6X8(18,7,(num%100)/10);
		witeNum6X8(24,7,num%10);
	}
	if (num>999 & num<10000)
	{
		witeNum6X8(0,7,(num%10000)/1000);
		witeNum6X8(6,7,(num%1000)/100);
		witeNum6X8(12,7,(num%100)/10);
		witeNum6X8(18,7,num%10);
	}
	if (num>99& num<1000)
	{
		witeNum6X8(0,7,(num%1000)/100);
		witeNum6X8(6,7,(num%100)/10);
		witeNum6X8(12,7,num%10);
	}
	if (num>9& num<100)
	{
		witeNum6X8(0,7,(num%100)/10);
		witeNum6X8(6,7,num%10);
	}
		if (num<10)
	{
		witeNum6X8(0,7,num);
	}
}
void OLED_ChangeRightSatus(u8 right)
{
	if (right) OLED_P16x16Ch(0,0,13);
	else OLED_P16x16Ch(0,0,11);
}
void OLED_ChangeBLESatus(u8 on)
{
	if (on) OLED_P16x16Ch(16,0,21);
	else OLED_P16x16Ch(16,0,0);
}
void OLED_ChangeRECSatus(u8 on)
{
	if (on) OLED_P16x16Ch(32,0,12);
	else OLED_P16x16Ch(32,0,0);
}
void OLED_ChangePowerSatus(u8 percent)
{
	switch (percent)
	{
	case 100:
		OLED_P16x16Ch(112,0,14);
		break;
	case 80:
		OLED_P16x16Ch(112,0,15);
		break;
	case 60:
		OLED_P16x16Ch(112,0,16);
		break;
	case 40:
		OLED_P16x16Ch(112,0,17);
		break;
	case 20:
		OLED_P16x16Ch(112,0,18);
		break;
	case 10:
		OLED_P16x16Ch(112,0,19);
		break;
	case 0:
		OLED_P16x16Ch(112,0,20);
		break;
	default:
		break;
	}
}
void witeNum8X16(u8 x,u8 y,u8 num)
{
	OLED_SetPos(x,y);
	for (u8 i = 0; i < 8; i++)
	{
		OLED_Wdata(F8X16[(num+16)*16+i]);
	}
	OLED_SetPos(x,y+1);
	for (u8 i = 8; i < 16; i++)
	{
		OLED_Wdata(F8X16[(num+16)*16+i]);
	}
}
void witeNum6X8(u8 x,u8 y,u8 num)
{
	OLED_SetPos(x,y);
	for (u8 i = 0; i < 6; i++)
	{
		OLED_Wdata(F6x8[16+num][i]);
	}
}