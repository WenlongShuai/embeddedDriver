#include "oled.h"
#include "oled_codetab.h"
#include <string.h>
#include "delay.h"

OLED_MENU_STRUCT oled_menu_struct;
extern uint8_t keyStateFlag;

uint8_t OLED_Buffer[OLED_PAGES][OLED_WIDTH];  // 128��8�ֽڻ�����

static __IO uint32_t  I2CTimeout = I2CT_LONG_TIMEOUT;

static void IIC_Write_Byte(uint8_t addr, uint8_t data)
{
//	while (I2C_GetFlagStatus(IIC2_I2Cx, I2C_FLAG_BUSY));

//	I2C_GenerateSTART(IIC2_I2Cx, ENABLE); /* IIC_Start�ź� */
//	while (!I2C_CheckEvent(IIC2_I2Cx, I2C_EVENT_MASTER_MODE_SELECT)); /*EV5,��ģʽ*/

//	I2C_Send7bitAddress(IIC2_I2Cx, OLED_ID, I2C_Direction_Transmitter);  /* ���дӻ� */
//	delay_ms(20);
//	while (!I2C_CheckEvent(IIC2_I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

//	I2C_SendData(IIC2_I2Cx, addr);  /* ��������д���ַ */
//	while (!I2C_CheckEvent(IIC2_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

//	I2C_SendData(IIC2_I2Cx, data);  /* �������� */
//	while (!I2C_CheckEvent(IIC2_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

//	I2C_GenerateSTOP(IIC2_I2Cx, ENABLE); /* IIC_Stop�ź� */
	
	
	/* Send STRAT condition */
  I2C_GenerateSTART(IIC2_I2Cx, ENABLE);
	I2CTimeout = I2CT_FLAG_TIMEOUT; 
	
	/* Test on EV5 and clear it */
  while(!I2C_CheckEvent(IIC2_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))  
  {
    if((I2CTimeout--) == 0) return;
  } 
	
	I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Send address for write */
  I2C_Send7bitAddress(IIC2_I2Cx, OLED_ID, I2C_Direction_Transmitter);
	/* Test on EV6 and clear it */
  while(!I2C_CheckEvent(IIC2_I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((I2CTimeout--) == 0) return;
  }  
	
	/* Send internal address to write to */
	I2C_SendData(IIC2_I2Cx, addr);
	
	I2CTimeout = I2CT_FLAG_TIMEOUT;
	/* Test on EV8 and clear it */
	while(!I2C_CheckEvent(IIC2_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
		if((I2CTimeout--) == 0) return;
	} 
	
	/* Send the byte to be written */
	I2C_SendData(IIC2_I2Cx, data); 
	
	I2CTimeout = I2CT_FLAG_TIMEOUT;  
	/* Test on EV8 and clear it */
	while(!I2C_CheckEvent(IIC2_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
		if((I2CTimeout--) == 0) return;
	} 
  
  /* Send STOP condition */
  I2C_GenerateSTOP(IIC2_I2Cx, ENABLE);
}

/* oledд���� */
void Oled_Write_Data(uint8_t data)
{
#if IIC_SELECT
	IIC_Write_Byte(OLED_WR_DATA,data);
#else
	IIC2_Start();
	IIC2_Send_Byte(OLED_ID);
	/* �ȴ�Ӧ�� */
	while (IIC2_Wait_Ack());

	IIC2_Send_Byte(OLED_WR_DATA);
	/* �ȴ�Ӧ�� */
	while (IIC2_Wait_Ack());

	IIC2_Send_Byte(data);
	/* �ȴ�Ӧ�� */
	while (IIC2_Wait_Ack());
	
	IIC2_Stop();
#endif
}

/* oledд���� */
void Oled_Write_Cmd(uint8_t cmd)
{
	#if IIC_SELECT
	IIC_Write_Byte(OLED_WR_CMD,cmd);
	#else
	IIC2_Start();
	IIC2_Send_Byte(OLED_ID);
	/* �ȴ�Ӧ�� */
	while (IIC2_Wait_Ack());

	IIC2_Send_Byte(OLED_WR_CMD);
	/* �ȴ�Ӧ�� */
	while (IIC2_Wait_Ack());

	IIC2_Send_Byte(cmd);
	/* �ȴ�Ӧ�� */
	while (IIC2_Wait_Ack());

	IIC2_Stop();
#endif
}

void OLED_Init(void)
{
	/* ������ʾ��/�ر�
	 * AE--->��ʾ��
	 * AF--->��ʾ�ر�(����ģʽ)
	 */
	Oled_Write_Cmd(0xAE);

	/* ================== ��������� ===================*/
	/* ���öԱȶ�
	 * 0~255����ֵԽ�� ����Խ��
	 */
	Oled_Write_Cmd(0x81);
	Oled_Write_Cmd(0xFF);

	/* ʹ��ȫ����ʾ
	 * A4--->�ָ���RAM������ʾ
	 * A5--->����RAM������ʾ
	 */
	Oled_Write_Cmd(0xA4);

	/* ������ʾģʽ
	 * A6--->������ʾ��0��1��
	 * A7--->����ʾ��1��0��
	 */
	Oled_Write_Cmd(0xA6);

	/* ================== ��������� ===================*/
	/* ����ʹ��/ʧ��
	 * 2E--->ʧ��
	 * 2F--->ʹ��
	 */
	Oled_Write_Cmd(0x2E);

	/* ���ֽ���� ����ˮƽ�������� */

	/* ��/��ˮƽ��������
	 * 26--->��ˮƽ����
	 * 27--->��ˮƽ����
	 */
	Oled_Write_Cmd(0x26);
	/* �����ֽ� */
	Oled_Write_Cmd(0x00);
	/* ���ù�����ʼҳ��ַ */
	Oled_Write_Cmd(0x00);
	/* ���ù������ */
	Oled_Write_Cmd(0x03);
	/* ���ù���������ַ */
	Oled_Write_Cmd(0x07);
	/* �����ֽ� */
	Oled_Write_Cmd(0x00);
	Oled_Write_Cmd(0xFF);

	/* =============== Ѱַ��������� ==================*/

	/* ˫�ֽ�����:�Ĵ���Ѱַģʽ */
	Oled_Write_Cmd(0x20);

	/* 10:ҳѰַģʽ
	 * 01:��ֱѰַģʽ
	 * 00:ˮƽѰַģʽ
	 */
	Oled_Write_Cmd(0x10);
	/* ���ֽ�����:����ҳѰַ����ʼҳ��ַ */
	Oled_Write_Cmd(0xB0);
	/* ���ֽ�����:����ҳѰַ����ʼ�е�ַ��λ */
	Oled_Write_Cmd(0x00);
	/* ���ֽ�����:����ҳѰַ����ʼ�е�ַ��λ */
	Oled_Write_Cmd(0x10);

	/*=============== Ӳ����������� ==================*/

	/* ������ʾ��ʼ��
	 * 0x40~0x7F��Ӧ0~63
	 */
	Oled_Write_Cmd(0x40);

	/* ��������ӳ��
	 * A0:addressX--->segX
	 * A1:addressX--->seg(127-X)
	 */
	Oled_Write_Cmd(0xA1);

	/* ���ö�·���ñ� */
	Oled_Write_Cmd(0xA8);
	Oled_Write_Cmd(0x3F);

	/* ����COM���ɨ�跽��
	 * C0��COM0--->COM63(��������ɨ��)
	 * C8��COM63--->COM0(��������ɨ��)
	 */
	Oled_Write_Cmd(0xC8);

	/* ˫�ֽ��������COM��ʾƫ���� */
	Oled_Write_Cmd(0xD3);
	Oled_Write_Cmd(0x00); /* COM��ƫ�� */

	/* ˫�ֽ��������COM��ӳ�� */
	Oled_Write_Cmd(0xDA);
	Oled_Write_Cmd(0x12);

	/* ˫�ֽ��������Ԥ���� */
	Oled_Write_Cmd(0xD9);
	Oled_Write_Cmd(0x22); /* �׶�һ2����ЧDCLKʱ��/�׶ζ�2����ЧDCLKʱ�� */

	/* ����VCOMHȡ��ѡ���ƽ
	 * 00:0.65xVcc
	 * 20:0.77xVcc
	 * 30:0.83xVcc
	 */
	Oled_Write_Cmd(0xDB);
	Oled_Write_Cmd(0x20);

	/* ˫�ֽ�������õ�ɱ� */
	Oled_Write_Cmd(0x8d);
	Oled_Write_Cmd(0x14);

	Oled_Write_Cmd(0xAF);
	
	OLED_CLS();                                              // ����
	
	for(int i = 0;i < 3;i++)
	{
		for(int j = 0;j < 3;j++)
			oled_menu_struct.chooseNum[i][j] = 0;
	}
	oled_menu_struct.chooseNum[0][0] = 1;
}

/**
 * @brief  ���ù��
 * @param  x,���xλ��
 *		   y,���yλ��
 * @retval ��
 */
void OLED_SetPos(unsigned char x, unsigned char y) // ������ʼ������
{
	Oled_Write_Cmd(0xb0 + y);
	Oled_Write_Cmd(((x & 0xf0) >> 4) | 0x10);
	Oled_Write_Cmd((x & 0x0f) | 0x01);
}

/**
 * @brief  ���������Ļ
 * @param  fill_data:Ҫ��������
 * @retval ��
 */
void OLED_Fill(unsigned char fill_data) // ȫ�����
{
	unsigned char m, n;
	for (m = 0; m < 8; m++)
	{
		Oled_Write_Cmd(0xb0 + m); // page0-page1
		Oled_Write_Cmd(0x00);     // low column start address
		Oled_Write_Cmd(0x10);     // high column start address
		for (n = 0; n < 128; n++)
		{
			Oled_Write_Data(fill_data);
		}
	}
}

/**
 * @brief  ����ָ������
 * @param  startX, startY : ��ʼ������(startX:0~127, startY:0~63)
						width, height : ���Ŀ�Ⱥ͸߶�(width:0~127, height:0~63)
 * @retval ��
 */
void OLED_Appoint(unsigned char startX, unsigned char startY,unsigned char width, unsigned char height) // ָ��λ�����
{
	for (uint16_t i = startX; i < startX + width && i < OLED_WIDTH; i++) 
	{
		for (uint16_t j = startY; j < startY + height && j < OLED_HEIGHT; j++) 
		{
			uint8_t page = j / 8;
			uint8_t bit = j % 8;
			OLED_Buffer[page][i] ^= (1 << bit);  // ��������תλ
		}
	}
	OLED_UpdateScreen(startX, startY, width, height);
}

// ������ʾ�������������ݷ��͵�OLED��
void OLED_UpdateScreen(uint16_t x, uint16_t y, uint16_t w, uint16_t h) 
{
//	for (uint8_t page = 0; page < OLED_PAGES; page++) 
//	{
//		// ����ҳ��ַ
//		Oled_Write_Cmd(0xB0 + page);  // ҳ0-7
//		Oled_Write_Cmd(0x00);         // �е͵�ַ
//		Oled_Write_Cmd(0x10);         // �иߵ�ַ
//		
//		// ����һ������
//		uint8_t data[OLED_WIDTH];
//		memcpy(&data[0], OLED_Buffer[page], OLED_WIDTH);
//		
//		
//		for(uint8_t i = 0;i < OLED_WIDTH;i++)
//		{
//			Oled_Write_Data(data[i]);
//		}
//	}
	
	uint8_t start_page = y / 8;
	uint8_t end_page = (y + h - 1) / 8;
	
	for (uint8_t page = start_page; page <= end_page; page++) 
	{
		// ����ҳ��ַ
		Oled_Write_Cmd(0xB0 + page);
		Oled_Write_Cmd(0x00 + (x & 0x0F));     // �е͵�ַ
		Oled_Write_Cmd(0x10 + ((x >> 4) & 0x0F)); // �иߵ�ַ
		
		// ���͸�ҳָ�����������
		uint8_t data[w];
		memcpy(&data[0], &OLED_Buffer[page][x], w);
		
		for(uint8_t i = 0;i < w;i++)
		{
			Oled_Write_Data(data[i]);
		}
	}
}

/**
 * @brief  ����
 * @param  ��
 * @retval ��
 */
void OLED_CLS(void) // ����
{
	OLED_Fill(0x00);
}

/**
 * @brief  ��OLED�������л���
 * @param  ��
 * @retval ��
 */
void OLED_ON(void)
{
	Oled_Write_Cmd(0x8D); // ���õ�ɱ�
	Oled_Write_Cmd(0x14); // ������ɱ�
	Oled_Write_Cmd(0xAF); // OLED����
}

/**
 * @brief  ��OLED���� -- ����ģʽ��,OLED���Ĳ���10uA
 * @param  ��
 * @retval ��
 */
void OLED_OFF(void)
{
	Oled_Write_Cmd(0x8D); // ���õ�ɱ�
	Oled_Write_Cmd(0x10); // �رյ�ɱ�
	Oled_Write_Cmd(0xAE); // OLED����
}


/**
 * @brief ��ʾcodetab.h�е�ASCII�ַ�,��6*8��8*16��ѡ��
 * @param  x,y : ��ʼ������(x:0~127, y:0~7);
 *					ch[] :- Ҫ��ʾ���ַ���;
 *					textsize : �ַ���С(1:6*8 ; 2:8*16)
 * @retval ��
 */
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char textsize)
{
	unsigned char c = 0, i = 0, j = 0;
	switch (textsize)
	{
	case 1:
	{
		while (ch[j] != '\0')
		{
				c = ch[j] - 32;
				if (x > 126)
				{
						x = 0;
						y++;
				}
				OLED_SetPos(x, y);
				for (i = 0; i < 6; i++)
						Oled_Write_Data(F6x8[c][i]);
				x += 6;
				j++;
		}
	}
	break;
	case 2:
	{
		while (ch[j] != '\0')
		{
				c = ch[j] - 32;
				if (x > 120)
				{
						x = 0;
						y++;
				}
				OLED_SetPos(x, y);
				for (i = 0; i < 8; i++)
						Oled_Write_Data(F8X16[c * 16 + i]);
				OLED_SetPos(x, y + 1);
				for (i = 0; i < 8; i++)
						Oled_Write_Data(F8X16[c * 16 + i + 8]);
				x += 8;
				j++;
		}
	}
	break;
	}
}

/**
 * @brief  OLED_ShowCN����ʾcodetab.h�еĺ���,16*16����
 * @param  x,y: ��ʼ������(x:0~127, y:0~7); N:������codetab.h�е�����
 *
 * @retval ��
 */
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char n)
{
	unsigned char wm = 0;
	unsigned int adder = 32 * n;
	OLED_SetPos(x, y);
	for (wm = 0; wm < 16; wm++)
	{
			Oled_Write_Data(F16x16[adder]);
			adder += 1;
	}
	OLED_SetPos(x, y + 1);
	for (wm = 0; wm < 16; wm++)
	{
			Oled_Write_Data(F16x16[adder]);
			adder += 1;
	}
}

/**
 * @brief  OLED_DrawBMP����ʾBMPλͼ
 * @param  x0,y0 :��ʼ������(x0:0~127, y0:0~7);x1,y1 : ���Խ���(������)������(x1:1~128,y1:1~8)
 * @retval ��
 */
void OLED_DrawBMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[])
{
	unsigned int j = 0;
	unsigned char x, y;

	if (y1 % 8 == 0)
	{
			y = y1 / 8;
	}
	else
	{
			y = y1 / 8 + 1;
	}
	for (y = y0; y < y1; y++)
	{
			OLED_SetPos(x0, y);
			for (x = x0; x < x1; x++)
			{
					Oled_Write_Data(BMP[j++]);
			}
	}
}




void setOLEDDisplay(KeyEvent_t keyEvent)
{
	switch(keyEvent)
	{
		case KEY_EVENT_CLICK:  //��������
			
			break;
		case KEY_EVENT_DOUBLE_CLICK:  //����˫��
			
			break;
	
	}
	
	
	/* ������ */
	#if 1
	OLED_DrawBMP(0, 0, 16, 2, (unsigned char *)bluetoothIcon);   //����ͼ��
	OLED_DrawBMP(20, 0, 36, 2, (unsigned char *)wifiIcon);		//WiFiͼ��
	OLED_DrawBMP(111, 0, 127, 2, (unsigned char *)electricQuantityIco3);       //��ص���ͼ��
	OLED_ShowStr(44,2,(unsigned char *)"17:58",2);
	OLED_ShowStr(50,4,(unsigned char *)"2025",1);
	
//	OLED_Appoint(96,48,32,16);
	
	for (uint8_t i = 0; i < 2; i++)
	{
			OLED_ShowCN(0+ i * 16, 6, i); // ������ʾ����
	}
	
	for (uint8_t i = 0; i < 2; i++)
	{
			OLED_ShowCN(96+ i * 16, 6, i+2); // ������ʾ����
	}
	
	
	#endif
	
	#if 0
	OLED_ShowStr(0,0,(unsigned char *)"L:+ 100",1);
	OLED_ShowStr(64,0,(unsigned char *)"L:+ 100",1);
	OLED_ShowStr(0,1,(unsigned char *)"R:+ 100",1);
	OLED_ShowStr(64,1,(unsigned char *)"R:+ 100",1);
	OLED_ShowStr(0,2,(unsigned char *)"GPS:",1);
	OLED_ShowStr(32,2,(unsigned char *)"119.00000001",1);
	OLED_ShowStr(32,3,(unsigned char *)"39.00000001",1);
	OLED_ShowStr(32,4,(unsigned char *)"1000",1);
	OLED_ShowStr(0,5,(unsigned char *)"IMU:",1);
	OLED_ShowStr(32,5,(unsigned char *)"111",1);
	OLED_ShowStr(32,6,(unsigned char *)"222",1);
	OLED_ShowStr(32,7,(unsigned char *)"333",1);
	#endif
	
//	delay_ms(5000);
}

void oled_Main_Menu_Display(void)
{
	/* ������ */
	OLED_CLS();
	OLED_DrawBMP(0, 0, 16, 2, (unsigned char *)bluetoothIcon);   //����ͼ��
	OLED_DrawBMP(20, 0, 36, 2, (unsigned char *)wifiIcon);		//WiFiͼ��
	OLED_DrawBMP(111, 0, 127, 2, (unsigned char *)electricQuantityIco3);   //��ص���ͼ��
	OLED_ShowStr(44,2,(unsigned char *)"17:58",2);
	OLED_ShowStr(50,4,(unsigned char *)"2025",1);
		
	for (uint8_t i = 0; i < 2; i++)
	{
		OLED_ShowCN(0+ i * 16, 6, 6+i); //�����á�
	}
	
	for (uint8_t i = 0; i < 2; i++)
	{
		OLED_ShowCN(96+ i * 16, 6, 18+i); //����ʾ��
	}
}

void oled_Set_Menu_Display(void)
{
	uint8_t i;
	OLED_CLS();
	
	for (i = 0; i < 4; i++)
	{
		if(i < 2)
			OLED_ShowCN(0+ i * 16, 0, 0+i); //��ʱ�䡱
		else
			OLED_ShowCN(0+ i * 16, 0, 6+(i-2)); //�����á�
	}
	OLED_ShowStr(i * 16, 0, (unsigned char *)" >",2);   //8*16�ַ�
	
	for (i = 0; i < 4; i++)
	{
		if(i < 2)
			OLED_ShowCN(0+ i * 16, 2, 2+i); //��������
		else
			OLED_ShowCN(0+ i * 16, 2, 6+(i-2)); //�����á�
	}
	OLED_ShowStr(i * 16, 2, (unsigned char *)" >",2);   //8*16�ַ�
	
	for (i = 0; i < 4; i++)
	{
		if(i < 2)
			OLED_ShowCN(0+ i * 16, 4, 4+i); //���������á�
		else
			OLED_ShowCN(0+ i * 16, 4, 6+(i-2)); //���������á�
	}
	OLED_ShowStr(i * 16, 4, (unsigned char *)" >",2);   //8*16�ַ�

	for (i = 0; i < 2; i++)
	{
		OLED_ShowCN(96+ i * 16, 6, 24+i); //���˳���
	}
}

void oled_Dis_Menu_Display(void)
{
	uint8_t i;
	OLED_CLS();
	
	for (i = 0; i < 5; i++)
	{
		if(i < 3)
			OLED_ShowCN(0+ i * 16, 0, 8+i); //�������
		else
			OLED_ShowCN(0+ i * 16, 0, 18+(i-3)); //����ʾ��
	}
	OLED_ShowStr(i * 16, 0, (unsigned char *)" >",2);
	
	for (i = 0; i < 5; i++)
	{
		if(i < 2)
			OLED_ShowCN(0+ i * 16, 2, 8+i); //�������
		else if(i == 2)
			OLED_ShowCN(0+ i * 16, 2, 11); //���ҡ�
		else
			OLED_ShowCN(0+ i * 16, 2, 18+(i-3)); //����ʾ��
	}
	OLED_ShowStr(i * 16, 2, (unsigned char *)" >",2);
	
	OLED_ShowStr(0, 4, (unsigned char *)"GPS",2);
	for (i = 0; i < 2; i++)
	{
		OLED_ShowCN(24+ i * 16, 4, 18+i); //����ʾ��
	}
	OLED_ShowStr(24+i * 16, 4, (unsigned char *)" >",2);
	
//	OLED_ShowStr(0, 6, (unsigned char *)"IMU",2);
//	for (i = 0; i < 4; i++)
//	{
//		OLED_ShowCN(24+ i * 16, 6, 18+(i-3)); //����ʾ��
//	}
//	OLED_ShowStr(24 + i * 16, 6, (unsigned char *)" >",2);
//	
//	for (i = 0; i < 6; i++)
//	{
//		if(i < 4)
//			OLED_ShowCN(0+ i * 16, 6, 12+i); //������ģʽ��
//		else
//			OLED_ShowCN(0+ i * 16, 6, 18+(i-4)); //����ʾ��
//	}
//	OLED_ShowStr(i * 16, 2, (unsigned char *)" >",2);
//	
//	for (i = 0; i < 4; i++)
//	{
//		if(i < 2)
//			OLED_ShowCN(0+ i * 16, 6, i); //��ȫ����
//		else
//			OLED_ShowCN(0+ i * 16, 6, 18+(i-3)); //����ʾ��
//	}
//	OLED_ShowStr(i * 16, 2, (unsigned char *)" >",2);

	for (i = 0; i < 2; i++)
	{
		OLED_ShowCN(96+ i * 16, 6, 24+i); //���˳���
	}
}

void main_Menu_Operate(void)
{
	keyStateFlag = 0;
	switch(oled_menu_struct.chooseNum[0][1])
	{
		case 0:
			while(1)
			{
				delay_ms(100);
				if(keyStateFlag)
					break;
			}
			break;
		case 1:
			while(1)
			{
				for (uint8_t i = 0; i < 2; i++)
				{
					OLED_ShowCN(0+ i * 16, 6, 6+i); //�����á�
				}
				delay_ms(100);
				OLED_ShowStr(0, 6, (unsigned char *)"    ",2);
				if(keyStateFlag)
					break;
			}
			for (uint8_t i = 0; i < 2; i++)
			{
				OLED_ShowCN(0+ i * 16, 6, 6+i); //�����á�
			}
			break;
		case 2:
			while(1)
			{
				for (uint8_t i = 0; i < 2; i++)
				{
					OLED_ShowCN(96+ i * 16, 6, 18+i); //����ʾ��
				}
				delay_ms(100);
				OLED_ShowStr(96, 6, (unsigned char *)"    ",2);
				if(keyStateFlag)
					break;
			}
			for (uint8_t i = 0; i < 2; i++)
			{
				OLED_ShowCN(96+ i * 16, 6, 18+i); //����ʾ��
			}
			break;
	}
}

void set_Menu_Operate(void)
{
	keyStateFlag = 0;
	int i = 0;
	switch(oled_menu_struct.chooseNum[1][1])
	{
		case 0:
			while(1)
			{
				delay_ms(100);
				if(keyStateFlag)
					break;
			}
			break;
		case 1:
			
			while(1)
			{
				for (i = 0; i < 4; i++)
				{
					if(i < 2)
						OLED_ShowCN(0+ i * 16, 0, 0+i); //��ʱ�䡱
					else
						OLED_ShowCN(0+ i * 16, 0, 6+(i-2)); //�����á�
				}
				OLED_ShowStr(i * 16, 0, (unsigned char *)" >",2);   //8*16�ַ�
				
				delay_ms(100);
				OLED_ShowStr(0, 0, (unsigned char *)"          ",2);
				if(keyStateFlag)
					break;
			}
			for (i = 0; i < 4; i++)
			{
				if(i < 2)
					OLED_ShowCN(0+ i * 16, 0, 0+i); //��ʱ�䡱
				else
					OLED_ShowCN(0+ i * 16, 0, 6+(i-2)); //�����á�
			}
			OLED_ShowStr(i * 16, 0, (unsigned char *)" >",2);   //8*16�ַ�
			break;
		case 2:
			while(1)
			{
				for (i = 0; i < 4; i++)
				{
					if(i < 2)
						OLED_ShowCN(0+ i * 16, 2, 2+i); //��������
					else
						OLED_ShowCN(0+ i * 16, 2, 6+(i-2)); //�����á�
				}
				OLED_ShowStr(i * 16, 2, (unsigned char *)" >",2);   //8*16�ַ�
				delay_ms(100);
				OLED_ShowStr(0, 2, (unsigned char *)"          ",2);
				if(keyStateFlag)
					break;
			}
			for (i = 0; i < 4; i++)
			{
				if(i < 2)
					OLED_ShowCN(0+ i * 16, 2, 2+i); //��������
				else
					OLED_ShowCN(0+ i * 16, 2, 6+(i-2)); //�����á�
			}
			OLED_ShowStr(i * 16, 2, (unsigned char *)" >",2);   //8*16�ַ�
			break;
		case 3:
			while(1)
			{
				for (i = 0; i < 4; i++)
				{
					if(i < 2)
						OLED_ShowCN(0+ i * 16, 4, 4+i); //���������á�
					else
						OLED_ShowCN(0+ i * 16, 4, 6+(i-2)); //���������á�
				}
				OLED_ShowStr(i * 16, 4, (unsigned char *)" >",2);   //8*16�ַ�
				delay_ms(100);
				
				OLED_ShowStr(0, 4, (unsigned char *)"          ",2);
				if(keyStateFlag)
					break;
			}
			for (i = 0; i < 4; i++)
			{
				if(i < 2)
					OLED_ShowCN(0+ i * 16, 4, 4+i); //���������á�
				else
					OLED_ShowCN(0+ i * 16, 4, 6+(i-2)); //���������á�
			}
			OLED_ShowStr(i * 16, 4, (unsigned char *)" >",2);   //8*16�ַ�
			break;
		case 4:
			while(1)
			{
				for (i = 0; i < 2; i++)
				{
					OLED_ShowCN(96+ i * 16, 6, 24+i); //���˳���
				}
				delay_ms(100);
				OLED_ShowStr(96, 6, (unsigned char *)"    ",2);
				if(keyStateFlag)
					break;
			}
			for (i = 0; i < 2; i++)
			{
				OLED_ShowCN(96+ i * 16, 6, 24+i); //���˳���
			}
			break;
	}
}

void set_Time_Menu_Operate(void)
{
	keyStateFlag = 0;
	int i = 0;
	switch(oled_menu_struct.chooseNum[2][2])
	{
		case 0:
			while(1)
			{
				delay_ms(100);
				if(keyStateFlag)
					break;
			}
			break;
		case 1:
			while(1)
			{
				for (i = 0; i < 2; i++)
				{
					OLED_ShowCN(96+ i * 16, 6, 24+i); //���˳���
				}
				delay_ms(100);
				OLED_ShowStr(96, 6, (unsigned char *)"    ",2);
				if(keyStateFlag)
					break;
			}
			for (i = 0; i < 2; i++)
			{
				OLED_ShowCN(96+ i * 16, 6, 24+i); //���˳���
			}
			break;
	}
}

void set_Bluetooth_Menu_Operate(void)
{
	keyStateFlag = 0;
	int i = 0;
	switch(oled_menu_struct.chooseNum[2][2])
	{
		case 0:
			while(1)
			{
				delay_ms(100);
				if(keyStateFlag)
					break;
			}
			break;
		case 1:
			while(1)
			{
				for (i = 0; i < 2; i++)
				{
					OLED_ShowCN(96+ i * 16, 6, 24+i); //���˳���
				}
				delay_ms(100);
				OLED_ShowStr(96, 6, (unsigned char *)"    ",2);
				if(keyStateFlag)
					break;
			}
			for (i = 0; i < 2; i++)
			{
				OLED_ShowCN(96+ i * 16, 6, 24+i); //���˳���
			}
			break;
	}
}

void set_WIFI_Menu_Operate(void)
{
	keyStateFlag = 0;
	int i = 0;
	switch(oled_menu_struct.chooseNum[2][2])
	{
		case 0:
			while(1)
			{
				delay_ms(100);
				if(keyStateFlag)
					break;
			}
			break;
		case 1:
			while(1)
			{
				for (i = 0; i < 2; i++)
				{
					OLED_ShowCN(96+ i * 16, 6, 24+i); //���˳���
				}
				delay_ms(100);
				OLED_ShowStr(96, 6, (unsigned char *)"    ",2);
				if(keyStateFlag)
					break;
			}
			for (i = 0; i < 2; i++)
			{
				OLED_ShowCN(96+ i * 16, 6, 24+i); //���˳���
			}
			break;
	}
}

void dis_Menu_Operate(void)
{
	keyStateFlag = 0;
	int i = 0;
	switch(oled_menu_struct.chooseNum[2][1])
	{
		case 0:
			while(1)
			{
				delay_ms(100);
				if(keyStateFlag)
					break;
			}
			break;
		case 1:
			OLED_ShowStr(0, 4, (unsigned char *)"GPS",2);
			for (i = 0; i < 2; i++)
			{
				OLED_ShowCN(24+ i * 16, 4, 18+i); //����ʾ��
			}
			OLED_ShowStr(24+i * 16, 4, (unsigned char *)" >",2);
			
			for (i = 0; i < 5; i++)
			{
				if(i < 2)
					OLED_ShowCN(0+ i * 16, 2, 8+i); //�������
				else if(i == 2)
					OLED_ShowCN(0+ i * 16, 2, 11); //���ҡ�
				else
					OLED_ShowCN(0+ i * 16, 2, 18+(i-3)); //����ʾ��
			}
			OLED_ShowStr(i * 16, 2, (unsigned char *)" >",2);
			while(1)
			{
				for (i = 0; i < 5; i++)
				{
					if(i < 3)
						OLED_ShowCN(0+ i * 16, 0, 8+i); //�������
					else
						OLED_ShowCN(0+ i * 16, 0, 18+(i-3)); //����ʾ��
				}
				OLED_ShowStr(i * 16, 0, (unsigned char *)" >",2);
				delay_ms(100);
				OLED_ShowStr(0, 0, (unsigned char *)"            ",2);
				if(keyStateFlag)
					break;
			}
			break;
		case 2:
			for (i = 0; i < 5; i++)
			{
				if(i < 3)
					OLED_ShowCN(0+ i * 16, 0, 8+i); //�������
				else
					OLED_ShowCN(0+ i * 16, 0, 18+(i-3)); //����ʾ��
			}
			OLED_ShowStr(i * 16, 0, (unsigned char *)" >",2);
			
			OLED_ShowStr(0, 4, (unsigned char *)"GPS",2);
			for (i = 0; i < 2; i++)
			{
				OLED_ShowCN(24+ i * 16, 4, 18+i); //����ʾ��
			}
			OLED_ShowStr(24+i * 16, 4, (unsigned char *)" >",2);
			while(1)
			{
				for (i = 0; i < 5; i++)
				{
					if(i < 2)
						OLED_ShowCN(0+ i * 16, 2, 8+i); //�������
					else if(i == 2)
						OLED_ShowCN(0+ i * 16, 2, 11); //���ҡ�
					else
						OLED_ShowCN(0+ i * 16, 2, 18+(i-3)); //����ʾ��
				}
				OLED_ShowStr(i * 16, 2, (unsigned char *)" >",2);
				delay_ms(100);
				OLED_ShowStr(0, 0, (unsigned char *)"            ",2);
				if(keyStateFlag)
					break;
			}
			break;
		case 3:
			for (i = 0; i < 5; i++)
			{
				if(i < 3)
					OLED_ShowCN(0+ i * 16, 0, 8+i); //�������
				else
					OLED_ShowCN(0+ i * 16, 0, 18+(i-3)); //����ʾ��
			}
			OLED_ShowStr(i * 16, 0, (unsigned char *)" >",2);
				
			for (i = 0; i < 5; i++)
			{
				if(i < 2)
					OLED_ShowCN(0+ i * 16, 2, 8+i); //�������
				else if(i == 2)
					OLED_ShowCN(0+ i * 16, 2, 11); //���ҡ�
				else
					OLED_ShowCN(0+ i * 16, 2, 18+(i-3)); //����ʾ��
			}
			OLED_ShowStr(i * 16, 2, (unsigned char *)" >",2);
			while(1)
			{
				OLED_ShowStr(0, 4, (unsigned char *)"GPS",2);
				for (i = 0; i < 2; i++)
				{
					OLED_ShowCN(24+ i * 16, 4, 18+i); //����ʾ��
				}
				OLED_ShowStr(24+i * 16, 4, (unsigned char *)" >",2);
				delay_ms(100);
				OLED_ShowStr(0, 0, (unsigned char *)"            ",2);
				if(keyStateFlag)
					break;
			}
			break;
		case 4:
			for (i = 0; i < 5; i++)
			{
				if(i < 2)
					OLED_ShowCN(0+ i * 16, 0, 8+i); //�������
				else if(i == 2)
					OLED_ShowCN(0+ i * 16, 0, 11); //���ҡ�
				else
					OLED_ShowCN(0+ i * 16, 0, 18+(i-3)); //����ʾ��
			}
			OLED_ShowStr(i * 16, 0, (unsigned char *)" >",2);
			
			OLED_ShowStr(0, 2, (unsigned char *)"GPS",2);
			for (i = 0; i < 2; i++)
			{
				OLED_ShowCN(24+ i * 16, 2, 18+i); //����ʾ��
			}
			OLED_ShowStr(24+i * 16, 2, (unsigned char *)" >",2);
		
			while(1)
			{
				OLED_ShowStr(0, 4, (unsigned char *)"IMU",2);
				for (i = 0; i < 4; i++)
				{
					OLED_ShowCN(24+ i * 16, 4, 18+(i-3)); //����ʾ��
				}
				OLED_ShowStr(24 + i * 16, 4, (unsigned char *)" >",2);
				delay_ms(100);
				OLED_ShowStr(0, 4, (unsigned char *)"            ",2);
				if(keyStateFlag)
					break;
			}
			break;
		case 5:
			OLED_ShowStr(0, 0, (unsigned char *)"GPS",2);
			for (i = 0; i < 2; i++)
			{
				OLED_ShowCN(24+ i * 16, 0, 18+i); //����ʾ��
			}
			OLED_ShowStr(24+i * 16, 0, (unsigned char *)" >",2);
			
			OLED_ShowStr(0, 2, (unsigned char *)"IMU",2);
			for (i = 0; i < 4; i++)
			{
				OLED_ShowCN(24+ i * 16, 2, 18+(i-3)); //����ʾ��
			}
			OLED_ShowStr(24 + i * 16, 2, (unsigned char *)" >",2);
			
		
			while(1)
			{
				for (i = 0; i < 6; i++)
				{
					if(i < 4)
						OLED_ShowCN(0+ i * 16, 4, 12+i); //������ģʽ��
					else
						OLED_ShowCN(0+ i * 16, 4, 18+(i-4)); //����ʾ��
				}
				OLED_ShowStr(i * 16, 4, (unsigned char *)" >",2);
				delay_ms(100);
				OLED_ShowStr(0, 4, (unsigned char *)"            ",2);
				if(keyStateFlag)
					break;
			}
			break;
		case 6:
			OLED_ShowStr(0, 0, (unsigned char *)"IMU",2);
			for (i = 0; i < 4; i++)
			{
				OLED_ShowCN(24+ i * 16, 0, 18+(i-3)); //����ʾ��
			}
			OLED_ShowStr(24 + i * 16, 0, (unsigned char *)" >",2);
			
			for (i = 0; i < 6; i++)
			{
				if(i < 4)
					OLED_ShowCN(0+ i * 16, 2, 12+i); //������ģʽ��
				else
					OLED_ShowCN(0+ i * 16, 2, 18+(i-4)); //����ʾ��
			}
			OLED_ShowStr(i * 16, 2, (unsigned char *)" >",2);
			while(1)
			{
				for (i = 0; i < 4; i++)
				{
					if(i < 2)
						OLED_ShowCN(0+ i * 16, 4, i); //��ȫ����
					else
						OLED_ShowCN(0+ i * 16, 4, 18+(i-3)); //����ʾ��
				}
				OLED_ShowStr(i * 16, 4, (unsigned char *)" >",2);
				delay_ms(100);
				OLED_ShowStr(0, 4, (unsigned char *)"            ",2);
				if(keyStateFlag)
					break;
			}
			break;
		case 7:
			while(1)
			{
				for (i = 0; i < 2; i++)
				{
					OLED_ShowCN(96+ i * 16, 6, 24+i); //���˳���
				}
				delay_ms(100);
				OLED_ShowStr(96, 6, (unsigned char *)"    ",2);
				if(keyStateFlag)
					break;
			}
			for (i = 0; i < 2; i++)
			{
				OLED_ShowCN(96+ i * 16, 6, 24+i); //���˳���
			}
			break;
	}

}

void dis_MotorLeft_Menu_Operate(void)
{
	int i = 0;
	switch(oled_menu_struct.chooseNum[2][1])
	{
		case 0:
			while(1)
			{
				delay_ms(100);
				if(keyStateFlag)
					break;
			}
			break;
		case 1:
			while(1)
			{
				for (i = 0; i < 2; i++)
				{
					OLED_ShowCN(96+ i * 16, 6, 24+i); //���˳���
				}
				delay_ms(100);
				OLED_ShowStr(96, 6, (unsigned char *)"    ",2);
				if(keyStateFlag)
					break;
			}
			for (i = 0; i < 2; i++)
			{
				OLED_ShowCN(96+ i * 16, 6, 24+i); //���˳���
			}
			break;
	}
}

void dis_MotorRight_Menu_Operate(void)
{
	int i = 0;
	switch(oled_menu_struct.chooseNum[2][1])
	{
		case 0:
			while(1)
			{
				delay_ms(100);
				if(keyStateFlag)
					break;
			}
			break;
		case 1:
			while(1)
			{
				for (i = 0; i < 2; i++)
				{
					OLED_ShowCN(96+ i * 16, 6, 24+i); //���˳���
				}
				delay_ms(100);
				OLED_ShowStr(96, 6, (unsigned char *)"    ",2);
				if(keyStateFlag)
					break;
			}
			for (i = 0; i < 2; i++)
			{
				OLED_ShowCN(96+ i * 16, 6, 24+i); //���˳���
			}
			break;
	}

}

void dis_GPS_Menu_Operate(void)
{
	int i = 0;
	switch(oled_menu_struct.chooseNum[2][1])
	{
		case 0:
			while(1)
			{
				delay_ms(100);
				if(keyStateFlag)
					break;
			}
			break;
		case 1:
			while(1)
			{
				for (i = 0; i < 2; i++)
				{
					OLED_ShowCN(96+ i * 16, 6, 24+i); //���˳���
				}
				delay_ms(100);
				OLED_ShowStr(96, 6, (unsigned char *)"    ",2);
				if(keyStateFlag)
					break;
			}
			for (i = 0; i < 2; i++)
			{
				OLED_ShowCN(96+ i * 16, 6, 24+i); //���˳���
			}
			break;
	}
}

void dis_IMU_Menu_Operate(void)
{
	int i = 0;
	switch(oled_menu_struct.chooseNum[2][1])
	{
		case 0:
			while(1)
			{
				delay_ms(100);
				if(keyStateFlag)
					break;
			}
			break;
		case 1:
			while(1)
			{
				for (i = 0; i < 2; i++)
				{
					OLED_ShowCN(96+ i * 16, 6, 24+i); //���˳���
				}
				delay_ms(100);
				OLED_ShowStr(96, 6, (unsigned char *)"    ",2);
				if(keyStateFlag)
					break;
			}
			for (i = 0; i < 2; i++)
			{
				OLED_ShowCN(96+ i * 16, 6, 24+i); //���˳���
			}
			break;
	}

}

void dis_ControlMode_Menu_Operate(void)
{
	int i = 0;
	switch(oled_menu_struct.chooseNum[2][1])
	{
		case 0:
			while(1)
			{
				delay_ms(100);
				if(keyStateFlag)
					break;
			}
			break;
		case 1:
			while(1)
			{
				for (i = 0; i < 2; i++)
				{
					OLED_ShowCN(96+ i * 16, 6, 24+i); //���˳���
				}
				delay_ms(100);
				OLED_ShowStr(96, 6, (unsigned char *)"    ",2);
				if(keyStateFlag)
					break;
			}
			for (i = 0; i < 2; i++)
			{
				OLED_ShowCN(96+ i * 16, 6, 24+i); //���˳���
			}
			break;
	}

}

void dis_All_Menu_Operate(void)
{
	int i = 0;
	switch(oled_menu_struct.chooseNum[2][1])
	{
		case 0:
			while(1)
			{
				delay_ms(100);
				if(keyStateFlag)
					break;
			}
			break;
		case 1:
			while(1)
			{
				for (i = 0; i < 2; i++)
				{
					OLED_ShowCN(96+ i * 16, 6, 24+i); //���˳���
				}
				delay_ms(100);
				OLED_ShowStr(96, 6, (unsigned char *)"    ",2);
				if(keyStateFlag)
					break;
			}
			for (i = 0; i < 2; i++)
			{
				OLED_ShowCN(96+ i * 16, 6, 24+i); //���˳���
			}
			break;
	}

}

void keyClickOLEDDis(void (*clickOperate)(void))
{
	clickOperate();
	
	
	
	
	
	
//	switch(oled_menu_struct.menuNum)
//	{
//		case 0:  //������
//			
//			break;
//		case 1:	 //���ý���
//			break; 
//		case 2:	 //��ʾ����
//			break;
//	}
	

}

void keyDoubleClickOLEDDis(void (*doubleClickOperate)(void))
{
	doubleClickOperate();
}


void setTimeMenuDisplay(void)
{
	OLED_CLS();
	OLED_ShowStr(48, 4, (unsigned char *)"TIME",2);   //8*16�ַ�
	
	for (int i = 0; i < 2; i++)
	{
		OLED_ShowCN(96+ i * 16, 6, 24+i); //���˳���
	}
}

void setBluetoothMenuDisplay(void)
{
	OLED_CLS();
	OLED_ShowStr(48, 4, (unsigned char *)"BLUETOOTH",2);   //8*16�ַ�

	for (int i = 0; i < 2; i++)
	{
		OLED_ShowCN(96+ i * 16, 6, 24+i); //���˳���
	}
}

void setWIFIMenuDisplay(void)
{
	OLED_CLS();
	OLED_ShowStr(48, 4, (unsigned char *)"WIFI",2);   //8*16�ַ�

	for (int i = 0; i < 2; i++)
	{
		OLED_ShowCN(96+ i * 16, 6, 24+i); //���˳���
	}
}

void disMotorLeftDisplay(void)
{
	OLED_ShowStr(0,0,(unsigned char *)"L:+ 100",1);
	OLED_ShowStr(64,0,(unsigned char *)"+ 100",1);
	for (int i = 0; i < 2; i++)
	{
		OLED_ShowCN(96+ i * 16, 6, 24+i); //���˳���
	}
}

void disMotorRightDisplay(void)
{
	OLED_ShowStr(0,1,(unsigned char *)"R:+ 100",1);
	OLED_ShowStr(64,1,(unsigned char *)"+ 100",1);
	for (int i = 0; i < 2; i++)
	{
		OLED_ShowCN(96+ i * 16, 6, 24+i); //���˳���
	}
}

void disGPSDisplay(void)
{
	OLED_ShowStr(0,2,(unsigned char *)"GPS:",1);
	OLED_ShowStr(32,2,(unsigned char *)"119.00000001",1);
	OLED_ShowStr(32,3,(unsigned char *)"39.00000001",1);
	OLED_ShowStr(32,4,(unsigned char *)"1000",1);
	for (int i = 0; i < 2; i++)
	{
		OLED_ShowCN(96+ i * 16, 6, 24+i); //���˳���
	}
}

void disIMUDisplay(void)
{
	OLED_ShowStr(0,5,(unsigned char *)"IMU:",1);
	OLED_ShowStr(32,5,(unsigned char *)"111",1);
	OLED_ShowStr(32,6,(unsigned char *)"222",1);
	OLED_ShowStr(32,7,(unsigned char *)"333",1);
	
	for (int i = 0; i < 2; i++)
	{
		OLED_ShowCN(96+ i * 16, 6, 24+i); //���˳���
	}
}

void disControlModeDisplay(void)
{

}

void disALLDisplay(void)
{
	OLED_ShowStr(0,0,(unsigned char *)"L:+ 100",1);
	OLED_ShowStr(64,0,(unsigned char *)"+ 100",1);
	OLED_ShowStr(0,1,(unsigned char *)"R:+ 100",1);
	OLED_ShowStr(64,1,(unsigned char *)"+ 100",1);
	OLED_ShowStr(0,2,(unsigned char *)"GPS:",1);
	OLED_ShowStr(32,2,(unsigned char *)"119.00000001",1);
	OLED_ShowStr(32,3,(unsigned char *)"39.00000001",1);
	OLED_ShowStr(32,4,(unsigned char *)"1000",1);
	OLED_ShowStr(0,5,(unsigned char *)"IMU:",1);
	OLED_ShowStr(32,5,(unsigned char *)"111",1);
	OLED_ShowStr(32,6,(unsigned char *)"222",1);
	OLED_ShowStr(32,7,(unsigned char *)"333",1);
	
	for (int i = 0; i < 2; i++)
	{
		OLED_ShowCN(96+ i * 16, 6, 24+i); //���˳���
	}
}
