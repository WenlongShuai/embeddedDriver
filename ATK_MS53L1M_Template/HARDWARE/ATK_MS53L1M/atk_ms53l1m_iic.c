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
void atk_ms53l1m_iic_start(void)
{
    ATK_MS53L1M_IIC_SDA(1);
    ATK_MS53L1M_IIC_SCL(1);
    atk_ms53l1m_iic_delay();
    ATK_MS53L1M_IIC_SDA(0);
    atk_ms53l1m_iic_delay();
    ATK_MS53L1M_IIC_SCL(0);
    atk_ms53l1m_iic_delay();
}

/**
 * @brief       ����IICֹͣ�ź�
 * @param       ��
 * @retval      ��
 */
void atk_ms53l1m_iic_stop(void)
{
    ATK_MS53L1M_IIC_SCL(0);
    ATK_MS53L1M_IIC_SDA(0);
    atk_ms53l1m_iic_delay();
    ATK_MS53L1M_IIC_SCL(1);
    ATK_MS53L1M_IIC_SDA(1);
    atk_ms53l1m_iic_delay();
}

/**
 * @brief       �ȴ�IICӦ���ź�
 * @param       ��
 * @retval      0: Ӧ���źŽ��ճɹ�
 *              1: Ӧ���źŽ���ʧ��
 */
uint8_t atk_ms53l1m_iic_wait_ack(void)
{
    uint8_t waittime = 0;
    uint8_t rack = 0;
    
    ATK_MS53L1M_IIC_SDA(1);
    atk_ms53l1m_iic_delay();
    ATK_MS53L1M_IIC_SCL(1);
    atk_ms53l1m_iic_delay();
    
    while (ATK_MS53L1M_IIC_READ_SDA())
    {
        waittime++;
        
        if (waittime > 250)
        {
            atk_ms53l1m_iic_stop();
            rack = 1;
            break;
        }
    }
    
    ATK_MS53L1M_IIC_SCL(0);
    atk_ms53l1m_iic_delay();
    
    return rack;
}

/**
 * @brief       ����ACKӦ���ź�
 * @param       ��
 * @retval      ��
 */
void atk_ms53l1m_iic_ack(void)
{
    ATK_MS53L1M_IIC_SDA(0);
    atk_ms53l1m_iic_delay();
    ATK_MS53L1M_IIC_SCL(1);
    atk_ms53l1m_iic_delay();
    ATK_MS53L1M_IIC_SCL(0);
    atk_ms53l1m_iic_delay();
    ATK_MS53L1M_IIC_SDA(1);
    atk_ms53l1m_iic_delay();
}

/**
 * @brief       ������ACKӦ���ź�
 * @param       ��
 * @retval      ��
 */
void atk_ms53l1m_iic_nack(void)
{
    ATK_MS53L1M_IIC_SDA(1);
    atk_ms53l1m_iic_delay();
    ATK_MS53L1M_IIC_SCL(1);
    atk_ms53l1m_iic_delay();
    ATK_MS53L1M_IIC_SCL(0);
    atk_ms53l1m_iic_delay();
}

/**
 * @brief       IIC����һ���ֽ�
 * @param       dat: Ҫ���͵�����
 * @retval      ��
 */
void atk_ms53l1m_iic_send_byte(uint8_t dat)
{
    uint8_t t;
    
    for (t=0; t<8; t++)
    {
        ATK_MS53L1M_IIC_SDA((dat & 0x80) >> 7);
        atk_ms53l1m_iic_delay();
        ATK_MS53L1M_IIC_SCL(1);
        atk_ms53l1m_iic_delay();
        ATK_MS53L1M_IIC_SCL(0);
        dat <<= 1;
    }
    ATK_MS53L1M_IIC_SDA(1);
}

/**
 * @brief       IIC����һ���ֽ�
 * @param       ack: ack=1ʱ������ack; ack=0ʱ������nack
 * @retval      ���յ�������
 */
uint8_t atk_ms53l1m_iic_read_byte(uint8_t ack)
{
    uint8_t i;
    uint8_t dat = 0;
    
    for (i = 0; i < 8; i++ )
    {
        dat <<= 1;
        ATK_MS53L1M_IIC_SCL(1);
        atk_ms53l1m_iic_delay();
        
        if (ATK_MS53L1M_IIC_READ_SDA())
        {
            dat++;
        }
        
        ATK_MS53L1M_IIC_SCL(0);
        atk_ms53l1m_iic_delay();
    }
    
    if (ack == 0)
    {
        atk_ms53l1m_iic_nack();
    }
    else
    {
        atk_ms53l1m_iic_ack();
    }

    return dat;
}

/**
 * @brief       ��ʼ��IIC�ӿ�
 * @param       ��
 * @retval      ��
 */
void atk_ms53l1m_iic_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    
    /* ʹ��SCL��SDA����GPIO��ʱ�� */
    ATK_MS53L1M_IIC_SCL_GPIO_CLK_ENABLE();
    ATK_MS53L1M_IIC_SDA_GPIO_CLK_ENABLE();
    
    /* ��ʼ��SCL���� */
    gpio_init_struct.GPIO_Pin    = ATK_MS53L1M_IIC_SCL_GPIO_PIN; /* SCL���� */
    gpio_init_struct.GPIO_Mode   = GPIO_Mode_Out_PP;          /* ������� */
    gpio_init_struct.GPIO_Speed  = GPIO_Speed_50MHz;         /* ���� */
    GPIO_Init(ATK_MS53L1M_IIC_SCL_GPIO_PORT, &gpio_init_struct);
    
    /* ��ʼ��SDA���� */
    gpio_init_struct.GPIO_Pin    = ATK_MS53L1M_IIC_SDA_GPIO_PIN; /* SDA���� */
    gpio_init_struct.GPIO_Mode   = GPIO_Mode_Out_OD;          /* ��©��� */
    GPIO_Init(ATK_MS53L1M_IIC_SDA_GPIO_PORT, &gpio_init_struct);
    
    atk_ms53l1m_iic_stop();
}
