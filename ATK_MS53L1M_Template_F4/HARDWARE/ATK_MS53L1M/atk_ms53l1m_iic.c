/**
 ****************************************************************************************************
 * @file        atk_ms53l1m_iic.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MS53L0ģ��IIC�ӿ���������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32F103������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "atk_ms53l1m_iic.h"
#include "delay.h"

/**
 * @brief       IIC�ӿ���ʱ���������ڿ���IIC��д�ٶ�
 * @param       ��
 * @retval      ��
 */
static inline void atk_ms53l1m_iic_delay(void)
{
    delay_us(4);
}

/**
 * @brief       ����IIC��ʼ�ź�
 * @param       ��
 * @retval      ��
 */
void atk_ms53l1m_iic_start(uint8_t dir)
{
	switch(dir)
	{
		case 1:
			#if ATK_MS53L1M_LEFT_ON
			ATK_MS53L1M_IIC_SDA_LEFT(1);
			ATK_MS53L1M_IIC_SCL_LEFT(1);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SDA_LEFT(0);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_LEFT(0);
			atk_ms53l1m_iic_delay();
			#endif
			break;
		case 2:
			#if ATK_MS53L1M_MID_ON
			ATK_MS53L1M_IIC_SDA_MID(1);
			ATK_MS53L1M_IIC_SCL_MID(1);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SDA_MID(0);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_MID(0);
			atk_ms53l1m_iic_delay();
			#endif
			break;
		case 3:
			#if ATK_MS53L1M_RIGHT_ON
			ATK_MS53L1M_IIC_SDA_RIGHT(1);
			ATK_MS53L1M_IIC_SCL_RIGHT(1);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SDA_RIGHT(0);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_RIGHT(0);
			atk_ms53l1m_iic_delay();
			#endif
			break;
	}
    
}

/**
 * @brief       ����IICֹͣ�ź�
 * @param       ��
 * @retval      ��
 */
void atk_ms53l1m_iic_stop(uint8_t dir)
{
	switch(dir)
	{
		case 1:
			#if ATK_MS53L1M_LEFT_ON
			ATK_MS53L1M_IIC_SCL_LEFT(0);
			ATK_MS53L1M_IIC_SDA_LEFT(0);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_LEFT(1);
			ATK_MS53L1M_IIC_SDA_LEFT(1);
			atk_ms53l1m_iic_delay();
			#endif
			break;
		case 2:
			#if ATK_MS53L1M_MID_ON
			ATK_MS53L1M_IIC_SCL_MID(0);
			ATK_MS53L1M_IIC_SDA_MID(0);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_MID(1);
			ATK_MS53L1M_IIC_SDA_MID(1);
			atk_ms53l1m_iic_delay();
			break;
			#endif
		case 3:
			#if ATK_MS53L1M_RIGHT_ON
			ATK_MS53L1M_IIC_SCL_RIGHT(0);
			ATK_MS53L1M_IIC_SDA_RIGHT(0);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_RIGHT(1);
			ATK_MS53L1M_IIC_SDA_RIGHT(1);
			atk_ms53l1m_iic_delay();
			#endif
			break;
	}
}

/**
 * @brief       �ȴ�IICӦ���ź�
 * @param       ��
 * @retval      0: Ӧ���źŽ��ճɹ�
 *              1: Ӧ���źŽ���ʧ��
 */
uint8_t atk_ms53l1m_iic_wait_ack(uint8_t dir)
{
	uint8_t waittime = 0;
	uint8_t rack = 0;
	
	switch(dir)
	{
		case 1:
			#if ATK_MS53L1M_LEFT_ON
			ATK_MS53L1M_IIC_SDA_LEFT(1);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_LEFT(1);
			atk_ms53l1m_iic_delay();
			
			while (ATK_MS53L1M_IIC_READ_SDA_LEFT())
			{
					waittime++;
					
					if (waittime > 250)
					{
							atk_ms53l1m_iic_stop(DIR_LEFT);
							rack = 1;
							break;
					}
			}
			
			ATK_MS53L1M_IIC_SCL_LEFT(0);
			atk_ms53l1m_iic_delay();
			#endif
			break;
		case 2:
			#if ATK_MS53L1M_MID_ON
			ATK_MS53L1M_IIC_SDA_MID(1);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_MID(1);
			atk_ms53l1m_iic_delay();
			
			while (ATK_MS53L1M_IIC_READ_SDA_MID())
			{
					waittime++;
					
					if (waittime > 250)
					{
							atk_ms53l1m_iic_stop(DIR_MID);
							rack = 1;
							break;
					}
			}
			
			ATK_MS53L1M_IIC_SCL_MID(0);
			atk_ms53l1m_iic_delay();
			#endif
			break;
		case 3:
			#if ATK_MS53L1M_RIGHT_ON
			ATK_MS53L1M_IIC_SDA_RIGHT(1);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_RIGHT(1);
			atk_ms53l1m_iic_delay();
			
			while (ATK_MS53L1M_IIC_READ_SDA_RIGHT())
			{
					waittime++;
					
					if (waittime > 250)
					{
							atk_ms53l1m_iic_stop(DIR_RIGHT);
							rack = 1;
							break;
					}
			}
			
			ATK_MS53L1M_IIC_SCL_RIGHT(0);
			atk_ms53l1m_iic_delay();
			#endif
			break;
	}
	return rack;
}

/**
 * @brief       ����ACKӦ���ź�
 * @param       ��
 * @retval      ��
 */
void atk_ms53l1m_iic_ack(uint8_t dir)
{
	switch(dir)
	{
		case 1:
			#if ATK_MS53L1M_LEFT_ON
			ATK_MS53L1M_IIC_SDA_LEFT(0);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_LEFT(1);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_LEFT(0);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SDA_LEFT(1);
			atk_ms53l1m_iic_delay();
			#endif
			break;
		case 2:
			#if ATK_MS53L1M_MID_ON
			ATK_MS53L1M_IIC_SDA_MID(0);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_MID(1);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_MID(0);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SDA_MID(1);
			atk_ms53l1m_iic_delay();
			#endif
			break;
		case 3:
			#if ATK_MS53L1M_RIGHT_ON
			ATK_MS53L1M_IIC_SDA_RIGHT(0);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_RIGHT(1);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_RIGHT(0);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SDA_RIGHT(1);
			atk_ms53l1m_iic_delay();
			#endif
			break;
	}
}

/**
 * @brief       ������ACKӦ���ź�
 * @param       ��
 * @retval      ��
 */
void atk_ms53l1m_iic_nack(uint8_t dir)
{
	switch(dir)
	{
		case 1:
			#if ATK_MS53L1M_LEFT_ON
			ATK_MS53L1M_IIC_SDA_LEFT(1);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_LEFT(1);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_LEFT(0);
			atk_ms53l1m_iic_delay();
			#endif
			break;
		case 2:
			#if ATK_MS53L1M_MID_ON
			ATK_MS53L1M_IIC_SDA_MID(1);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_MID(1);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_MID(0);
			atk_ms53l1m_iic_delay();
			#endif
			break;
		case 3:
			#if ATK_MS53L1M_RIGHT_ON
			ATK_MS53L1M_IIC_SDA_RIGHT(1);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_RIGHT(1);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_RIGHT(0);
			atk_ms53l1m_iic_delay();
		#endif
			break;
	}
}

/**
 * @brief       IIC����һ���ֽ�
 * @param       dat: Ҫ���͵�����
 * @retval      ��
 */
void atk_ms53l1m_iic_send_byte(uint8_t dir, uint8_t dat)
{
    uint8_t t;
	
	switch(dir)
	{
		case 1:
			#if ATK_MS53L1M_LEFT_ON
			for (t=0; t<8; t++)
			{
					ATK_MS53L1M_IIC_SDA_LEFT((dat & 0x80) >> 7);
					atk_ms53l1m_iic_delay();
					ATK_MS53L1M_IIC_SCL_LEFT(1);
					atk_ms53l1m_iic_delay();
					ATK_MS53L1M_IIC_SCL_LEFT(0);
					dat <<= 1;
			}
			ATK_MS53L1M_IIC_SDA_LEFT(1);
			#endif
			break;
		case 2:
			#if ATK_MS53L1M_MID_ON
			for (t=0; t<8; t++)
			{
					ATK_MS53L1M_IIC_SDA_MID((dat & 0x80) >> 7);
					atk_ms53l1m_iic_delay();
					ATK_MS53L1M_IIC_SCL_MID(1);
					atk_ms53l1m_iic_delay();
					ATK_MS53L1M_IIC_SCL_MID(0);
					dat <<= 1;
			}
			ATK_MS53L1M_IIC_SDA_MID(1);
			#endif
			break;
		case 3:
			#if ATK_MS53L1M_RIGHT_ON
			for (t=0; t<8; t++)
			{
					ATK_MS53L1M_IIC_SDA_RIGHT((dat & 0x80) >> 7);
					atk_ms53l1m_iic_delay();
					ATK_MS53L1M_IIC_SCL_RIGHT(1);
					atk_ms53l1m_iic_delay();
					ATK_MS53L1M_IIC_SCL_RIGHT(0);
					dat <<= 1;
			}
			ATK_MS53L1M_IIC_SDA_RIGHT(1);
			#endif
			break;
	}
}

/**
 * @brief       IIC����һ���ֽ�
 * @param       ack: ack=1ʱ������ack; ack=0ʱ������nack
 * @retval      ���յ�������
 */
uint8_t atk_ms53l1m_iic_read_byte(uint8_t dir, uint8_t ack)
{
    uint8_t i;
    uint8_t dat = 0;
	
	switch(dir)
	{
		case 1:
			#if ATK_MS53L1M_LEFT_ON
			for (i = 0; i < 8; i++ )
			{
					dat <<= 1;
					ATK_MS53L1M_IIC_SCL_LEFT(1);
					atk_ms53l1m_iic_delay();
					
					if (ATK_MS53L1M_IIC_READ_SDA_LEFT())
					{
							dat++;
					}
					
					ATK_MS53L1M_IIC_SCL_LEFT(0);
					atk_ms53l1m_iic_delay();
			}
			
			if (ack == 0)
			{
					atk_ms53l1m_iic_nack(DIR_LEFT);
			}
			else
			{
					atk_ms53l1m_iic_ack(DIR_LEFT);
			}
			#endif
			break;
		case 2:
			#if ATK_MS53L1M_MID_ON
			for (i = 0; i < 8; i++ )
			{
					dat <<= 1;
					ATK_MS53L1M_IIC_SCL_MID(1);
					atk_ms53l1m_iic_delay();
					
					if (ATK_MS53L1M_IIC_READ_SDA_MID())
					{
							dat++;
					}
					
					ATK_MS53L1M_IIC_SCL_MID(0);
					atk_ms53l1m_iic_delay();
			}
			
			if (ack == 0)
			{
					atk_ms53l1m_iic_nack(DIR_MID);
			}
			else
			{
					atk_ms53l1m_iic_ack(DIR_MID);
			}
			#endif
			break;
		case 3:
			#if ATK_MS53L1M_RIGHT_ON
			for (i = 0; i < 8; i++ )
			{
					dat <<= 1;
					ATK_MS53L1M_IIC_SCL_RIGHT(1);
					atk_ms53l1m_iic_delay();
					
					if (ATK_MS53L1M_IIC_READ_SDA_RIGHT())
					{
							dat++;
					}
					
					ATK_MS53L1M_IIC_SCL_RIGHT(0);
					atk_ms53l1m_iic_delay();
			}
			
			if (ack == 0)
			{
					atk_ms53l1m_iic_nack(DIR_RIGHT);
			}
			else
			{
					atk_ms53l1m_iic_ack(DIR_RIGHT);
			}
			#endif
			break;
	}
    
    return dat;
}

/**
 * @brief       ��ʼ��IIC�ӿ�
 * @param       ��
 * @retval      ��
 */
void atk_ms53l1m_iic_init(uint8_t dir)
{
	GPIO_InitTypeDef gpio_init_struct = {0};
	
	switch(dir)
	{
		case 1:
			#if ATK_MS53L1M_LEFT_ON
			/* ʹ��SCL��SDA����GPIO��ʱ�� */
			ATK_MS53L1M_IIC_SCL_GPIO_CLK_ENABLE_LEFT();
			ATK_MS53L1M_IIC_SDA_GPIO_CLK_ENABLE_LEFT();
			
			/* ��ʼ��SCL���� */
			gpio_init_struct.GPIO_Pin    = ATK_MS53L1M_IIC_SCL_GPIO_PIN_LEFT; /* SCL���� */
			gpio_init_struct.GPIO_Mode   = GPIO_Mode_OUT;          /* ������� */
			gpio_init_struct.GPIO_Speed  = GPIO_Speed_100MHz;         /* ���� */
			gpio_init_struct.GPIO_OType = GPIO_OType_PP;
			gpio_init_struct.GPIO_PuPd = GPIO_PuPd_UP;
			GPIO_Init(ATK_MS53L1M_IIC_SCL_GPIO_PORT_LEFT, &gpio_init_struct);
			
			/* ��ʼ��SDA���� */
			gpio_init_struct.GPIO_Pin = ATK_MS53L1M_IIC_SDA_GPIO_PIN_LEFT; /* SDA���� */
			gpio_init_struct.GPIO_OType = GPIO_OType_OD;          /* ��©��� */
			GPIO_Init(ATK_MS53L1M_IIC_SDA_GPIO_PORT_LEFT, &gpio_init_struct);
			
			atk_ms53l1m_iic_stop(DIR_LEFT);
			#endif
			break;
		
		
		case 2:
			#if ATK_MS53L1M_MID_ON
	
			/* ʹ��SCL��SDA����GPIO��ʱ�� */
			ATK_MS53L1M_IIC_SCL_GPIO_CLK_ENABLE_MID();
			ATK_MS53L1M_IIC_SDA_GPIO_CLK_ENABLE_MID();
			
			/* ��ʼ��SCL���� */
			gpio_init_struct.GPIO_Pin    = ATK_MS53L1M_IIC_SCL_GPIO_PIN_MID; /* SCL���� */
			gpio_init_struct.GPIO_Mode   = GPIO_Mode_OUT;          /* ������� */
			gpio_init_struct.GPIO_Speed  = GPIO_Speed_100MHz;         /* ���� */
			gpio_init_struct.GPIO_OType = GPIO_OType_PP;
			gpio_init_struct.GPIO_PuPd = GPIO_PuPd_UP;
			GPIO_Init(ATK_MS53L1M_IIC_SCL_GPIO_PORT_MID, &gpio_init_struct);
			
			/* ��ʼ��SDA���� */
			gpio_init_struct.GPIO_Pin = ATK_MS53L1M_IIC_SDA_GPIO_PIN_MID; /* SDA���� */
			gpio_init_struct.GPIO_OType = GPIO_OType_OD;          /* ��©��� */
			GPIO_Init(ATK_MS53L1M_IIC_SDA_GPIO_PORT_MID, &gpio_init_struct);
			
			atk_ms53l1m_iic_stop(DIR_MID);
			#endif
			break;
		
		case 3:
			#if ATK_MS53L1M_RIGHT_ON
	
			/* ʹ��SCL��SDA����GPIO��ʱ�� */
			ATK_MS53L1M_IIC_SCL_GPIO_CLK_ENABLE_RIGHT();
			ATK_MS53L1M_IIC_SDA_GPIO_CLK_ENABLE_RIGHT();
			
			/* ��ʼ��SCL���� */
			gpio_init_struct.GPIO_Pin    = ATK_MS53L1M_IIC_SCL_GPIO_PIN_RIGHT; /* SCL���� */
			gpio_init_struct.GPIO_Mode   = GPIO_Mode_OUT;          /* ������� */
			gpio_init_struct.GPIO_Speed  = GPIO_Speed_100MHz;         /* ���� */
			gpio_init_struct.GPIO_OType = GPIO_OType_PP;
			gpio_init_struct.GPIO_PuPd = GPIO_PuPd_UP;
			GPIO_Init(ATK_MS53L1M_IIC_SCL_GPIO_PORT_RIGHT, &gpio_init_struct);
			
			/* ��ʼ��SDA���� */
			gpio_init_struct.GPIO_Pin = ATK_MS53L1M_IIC_SDA_GPIO_PIN_RIGHT; /* SDA���� */
			gpio_init_struct.GPIO_OType = GPIO_OType_OD;          /* ��©��� */
			GPIO_Init(ATK_MS53L1M_IIC_SDA_GPIO_PORT_RIGHT, &gpio_init_struct);
			
			atk_ms53l1m_iic_stop(DIR_RIGHT);
			#endif
			break;
	}
}
