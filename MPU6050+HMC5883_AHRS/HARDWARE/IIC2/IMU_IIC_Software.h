/**
 ****************************************************************************************************
 * @file        atk_ms6050_iic.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MS6050ģ��IIC�ӿ���������
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

#ifndef __IMU_IIC_SOFTWARE_H
#define __IMU_IIC_SOFTWARE_H

#include "sys.h"

/* ���Ŷ��� */
#define SOFTWARE_IIC_SCL_GPIO_PORT            GPIOB
#define SOFTWARE_IIC_SCL_GPIO_PIN             GPIO_Pin_10
#define SOFTWARE_IIC_SCL_GPIO_CLK_ENABLE()    do{ RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); }while(0)
#define SOFTWARE_IIC_SDA_GPIO_PORT            GPIOB
#define SOFTWARE_IIC_SDA_GPIO_PIN             GPIO_Pin_11
#define SOFTWARE_IIC_SDA_GPIO_CLK_ENABLE()    do{ RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); }while(0)

/* IO���� */
#define SOFTWARE_IIC_SCL(x)                   do{ x ?                                                                                             \
                                                    GPIO_SetBits(SOFTWARE_IIC_SCL_GPIO_PORT, SOFTWARE_IIC_SCL_GPIO_PIN):    \
                                                    GPIO_ResetBits(SOFTWARE_IIC_SCL_GPIO_PORT, SOFTWARE_IIC_SCL_GPIO_PIN);   \
                                                }while(0)

#define SOFTWARE_IIC_SDA(x)                   do{ x ?                                                                                             \
                                                    GPIO_SetBits(SOFTWARE_IIC_SDA_GPIO_PORT, SOFTWARE_IIC_SDA_GPIO_PIN) :    \
                                                    GPIO_ResetBits(SOFTWARE_IIC_SDA_GPIO_PORT, SOFTWARE_IIC_SDA_GPIO_PIN);   \
                                                }while(0)

#define SOFTWARE_IIC_READ_SDA()               GPIO_ReadInputDataBit(SOFTWARE_IIC_SDA_GPIO_PORT, SOFTWARE_IIC_SDA_GPIO_PIN)

																								
																								/* ����������� */
#define SOFTWARE_EOK      0   /* û�д��� */
#define SOFTWARE_EID      1   /* ID���� */
#define SOFTWARE_EACK     2   /* IICͨѶACK���� */
																								
/* �������� */
void Software_iic_start(void);                /* ����IIC��ʼ�ź� */
void Software_iic_stop(void);                 /* ����IICֹͣ�ź� */
uint8_t Software_iic_wait_ack(void);          /* �ȴ�IICӦ���ź� */
void Software_iic_ack(void);                  /* ����ACKӦ���ź� */
void Software_iic_nack(void);                 /* ������ACKӦ���ź� */
void Software_iic_send_byte(uint8_t dat);     /* IIC����һ���ֽ� */
uint8_t Software_iic_read_byte(uint8_t ack);  /* IIC����һ���ֽ� */
void Software_iic_init(void);                 /* ��ʼ��IIC�ӿ� */

void Software_bitchange( u8 ADDR, u8 reg, u8 bitNum, u8 data);
u8 Software_Read_Byte(u8 ADDR,u8 reg,u8 len,u8 *buf);
u8 Software_Write_longByte(u8 ADDR,u8 reg,u8 len,u8 *buf);

#endif /* __IMU_IIC_SOFTWARE_H */
