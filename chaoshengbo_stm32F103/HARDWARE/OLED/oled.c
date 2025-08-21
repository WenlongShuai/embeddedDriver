#include "oled.h"
#include "oled_codetab.h"
#include <string.h>


uint8_t OLED_Buffer[OLED_PAGES][OLED_WIDTH];  // 128��8�ֽڻ�����

/* oledд���� */
static void Oled_Write_Data(uint8_t data)
{
	Software_Write_longByte(OLED_ID, OLED_WR_DATA, 1, &data);
}

/* oledд���� */
static void Oled_Write_Cmd(uint8_t cmd)
{
	Software_Write_longByte(OLED_ID, OLED_WR_CMD, 1, &cmd);
}

void OLED_IICTypeInit(void)
{
	Software_iic_init();
}

void OLED_Init(void)
{
	OLED_IICTypeInit();		//OLED IICͨ�ŷ�ʽ��ʼ��

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
