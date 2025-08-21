/**
 ****************************************************************************************************
 * @file        atk_ms6050_iic.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MS6050ģ��IIC�ӿ���������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� MiniSTM32 V4������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "IMU_IIC_Software.h"
#include "delay.h"

/**
 * @brief       IIC�ӿ���ʱ���������ڿ���IIC��д�ٶ�
 * @param       ��
 * @retval      ��
 */
static __INLINE void Software_iic_delay(void)
{
	delay_us(2);
}

/**
 * @brief       ����IIC��ʼ�ź�
 * @param       ��
 * @retval      ��
 */
void Software_iic_start(void)
{
	SOFTWARE_IIC_SDA(1);
	SOFTWARE_IIC_SCL(1);
	Software_iic_delay();
	SOFTWARE_IIC_SDA(0);
	Software_iic_delay();
	SOFTWARE_IIC_SCL(0);
	Software_iic_delay();
}

/**
 * @brief       ����IICֹͣ�ź�
 * @param       ��
 * @retval      ��
 */
void Software_iic_stop(void)
{
	SOFTWARE_IIC_SDA(0);
	Software_iic_delay();
	SOFTWARE_IIC_SCL(1);
	Software_iic_delay();
	SOFTWARE_IIC_SDA(1);
	Software_iic_delay();
}

/**
 * @brief       �ȴ�IICӦ���ź�
 * @param       ��
 * @retval      0: Ӧ���źŽ��ճɹ�
 *              1: Ӧ���źŽ���ʧ��
 */
uint8_t Software_iic_wait_ack(void)
{
	uint8_t waittime = 0;
	uint8_t rack = 0;

	SOFTWARE_IIC_SDA(1);
	Software_iic_delay();
	SOFTWARE_IIC_SCL(1);
	Software_iic_delay();

	while (SOFTWARE_IIC_READ_SDA())
	{
		waittime++;
		if (waittime > 250)
		{
			Software_iic_stop();
			rack = 1;
			break;
		}
	}

	SOFTWARE_IIC_SCL(0);
	Software_iic_delay();

	return rack;
}

/**
 * @brief       ����ACKӦ���ź�
 * @param       ��
 * @retval      ��
 */
void Software_iic_ack(void)
{
	SOFTWARE_IIC_SDA(0);
	Software_iic_delay();
	SOFTWARE_IIC_SCL(1);
	Software_iic_delay();
	SOFTWARE_IIC_SCL(0);
	Software_iic_delay();
	SOFTWARE_IIC_SDA(1);
	Software_iic_delay();
}

/**
 * @brief       ������ACKӦ���ź�
 * @param       ��
 * @retval      ��
 */
void Software_iic_nack(void)
{
	SOFTWARE_IIC_SDA(1);
	Software_iic_delay();
	SOFTWARE_IIC_SCL(1);
	Software_iic_delay();
	SOFTWARE_IIC_SCL(0);
	Software_iic_delay();
}

/**
 * @brief       IIC����һ���ֽ�
 * @param       dat: Ҫ���͵�����
 * @retval      ��
 */
void Software_iic_send_byte(uint8_t dat)
{
	uint8_t t;

	for (t=0; t<8; t++)
	{
		SOFTWARE_IIC_SDA((dat & 0x80) >> 7);
		Software_iic_delay();
		SOFTWARE_IIC_SCL(1);
		Software_iic_delay();
		SOFTWARE_IIC_SCL(0);
		dat <<= 1;
	}
	SOFTWARE_IIC_SDA(1);
}

/**
 * @brief       IIC����һ���ֽ�
 * @param       ack: ack=1ʱ������ack; ack=0ʱ������nack
 * @retval      ���յ�������
 */
uint8_t Software_iic_read_byte(uint8_t ack)
{
	uint8_t i;
	uint8_t dat = 0;

	for (i = 0; i < 8; i++ )
	{
		dat <<= 1;
		SOFTWARE_IIC_SCL(1);
		Software_iic_delay();

		if (SOFTWARE_IIC_READ_SDA())
		{
				dat++;
		}

		SOFTWARE_IIC_SCL(0);
		Software_iic_delay();
	}

	if (ack == 0)
	{
		Software_iic_nack();
	}
	else
	{
		Software_iic_ack();
	}

	return dat;
}


u8 Software_Read_Byte(u8 ADDR,u8 reg,u8 len,u8 *buf)
{
	Software_iic_start();
	Software_iic_send_byte((ADDR<<1)|0);
	if (Software_iic_wait_ack() == 1)
	{
		Software_iic_stop();
		return SOFTWARE_EACK;
	}
	Software_iic_send_byte(reg);
	if (Software_iic_wait_ack() == 1)
	{
		Software_iic_stop();
		return SOFTWARE_EACK;
	}
	Software_iic_start();
	Software_iic_send_byte((ADDR<<1)|1);
	if (Software_iic_wait_ack() == 1)
	{
		Software_iic_stop();
		return SOFTWARE_EACK;
	}
	while (len)
	{
		*buf = Software_iic_read_byte((len > 1) ? 1 : 0);
		len--;
		buf++;
	}
	Software_iic_stop();
	return SOFTWARE_EOK;
}
	
	
u8 Software_Write_longByte(u8 ADDR,u8 reg,u8 len,u8 *buf)
{
	uint8_t i;
	Software_iic_start();
	Software_iic_send_byte((ADDR<<1)|0);
	if (Software_iic_wait_ack() == 1)
	{
		Software_iic_stop();
		return SOFTWARE_EACK;
	}
	Software_iic_send_byte(reg);
	if (Software_iic_wait_ack() == 1)
	{
		Software_iic_stop();
		return SOFTWARE_EACK;
	}
	for (i=0; i<len; i++)
	{
		Software_iic_send_byte(buf[i]);
		if (Software_iic_wait_ack() == 1)
		{
			Software_iic_stop();
			return SOFTWARE_EACK;
		}
	}
	Software_iic_stop();
	return SOFTWARE_EOK;

}

	
/**************************IIC���ֽڵ�λ�ı�********************************************
*����ԭ��:		u8 IICwriteBit(u8 dev, u8 reg, u8 bitNum, u8 data)
*��������:	     �� �޸� д ָ���豸 ָ���Ĵ���һ���ֽ� �е�1��λ
���룺	ADDR     ������ַ
      reg	     �Ĵ�����ַ
		  bitNum   Ҫ�޸�Ŀ���ֽڵ�bitNumλ
		  data     Ϊ0 ʱ��Ŀ��λ������0 ���򽫱���λ
		   
����   �ɹ�Ϊ1  
 		   ʧ��Ϊ0 
*******************************************************************************/ 
void Software_bitchange( u8 ADDR, u8 reg, u8 bitNum, u8 data)
{
	u8 buffer[1],b;
	Software_iic_start();
	Software_iic_send_byte((ADDR<<1)|0);
	if (Software_iic_wait_ack() == 1)
	{
			Software_iic_stop();
			return;
	}
	Software_iic_send_byte(reg);
	if (Software_iic_wait_ack() == 1)
	{
			Software_iic_stop();
			return;
	}
	Software_iic_start();
	Software_iic_send_byte((ADDR<<1)|1);
	if (Software_iic_wait_ack() == 1)
	{
			Software_iic_stop();
			return;
	}
	*buffer = Software_iic_read_byte(0);
	Software_iic_stop();
		
	b=buffer[0];
	b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
	
	Software_iic_start();
	Software_iic_send_byte((ADDR<<1)|0);
	if (Software_iic_wait_ack() == 1)
	{
			Software_iic_stop();
			return;
	}
	Software_iic_send_byte(reg);
	if (Software_iic_wait_ack() == 1)
	{
			Software_iic_stop();
			return;
	}
	Software_iic_send_byte(b);
	if (Software_iic_wait_ack() == 1)
	{
			Software_iic_stop();
			return;
	}
	Software_iic_stop();
}

/**
 * @brief       ��ʼ��IIC�ӿ�
 * @param       ��
 * @retval      ��
 */
void Software_iic_init(void)
{
	GPIO_InitTypeDef gpio_init_struct;

	/* ʹ��SCL��SDA����GPIO��ʱ�� */
	SOFTWARE_IIC_SCL_GPIO_CLK_ENABLE();
	SOFTWARE_IIC_SDA_GPIO_CLK_ENABLE();

	/* ��ʼ��SCL���� */
	gpio_init_struct.GPIO_Pin = SOFTWARE_IIC_SCL_GPIO_PIN;  /* SCL���� */
	gpio_init_struct.GPIO_Mode = GPIO_Mode_Out_OD;          /* ������� */
	gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;                  /* ���� */
	GPIO_Init(SOFTWARE_IIC_SCL_GPIO_PORT, &gpio_init_struct);
	/* ��ʼ��SDA���� */
	gpio_init_struct.GPIO_Pin = SOFTWARE_IIC_SDA_GPIO_PIN;  /* SDA���� */
	GPIO_Init(SOFTWARE_IIC_SDA_GPIO_PORT, &gpio_init_struct);

	Software_iic_stop();
}
