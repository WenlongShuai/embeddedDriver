#ifndef __SOFTWARE_IIC_H__
#define __SOFTWARE_IIC_H__

#include "SysTick.h"

/* I2C1 ���Ŷ��� */
#define SOFTWARE_IIC1_SCL_GPIO_RCC   					 RCC_APB2Periph_GPIOB
#define SOFTWARE_IIC1_SCL_GPIO_PORT            GPIOB
#define SOFTWARE_IIC1_SCL_GPIO_PIN             GPIO_Pin_6
#define SOFTWARE_IIC1_SDA_GPIO_RCC   					 RCC_APB2Periph_GPIOB
#define SOFTWARE_IIC1_SDA_GPIO_PORT            GPIOB
#define SOFTWARE_IIC1_SDA_GPIO_PIN             GPIO_Pin_7

//IO��������
#define SOFTWARE_IIC1_SDA_IN()  {GPIOB->CRL&=~(0xF<<(7*4));GPIOB->CRL|=0<<7*4;}	//����ģʽ
#define SOFTWARE_IIC1_SDA_OUT() {GPIOB->CRL&=~(0xF<<(7*4));GPIOB->CRL|=3<<7*4;} 	//���ģʽ

/* I2C1 IO���� */
#define SOFTWARE_IIC1_SCL(x)                   do{ x ?                                                                                             \
                                                    GPIO_SetBits(SOFTWARE_IIC1_SCL_GPIO_PORT, SOFTWARE_IIC1_SCL_GPIO_PIN):    \
                                                    GPIO_ResetBits(SOFTWARE_IIC1_SCL_GPIO_PORT, SOFTWARE_IIC1_SCL_GPIO_PIN);   \
                                                }while(0)

#define SOFTWARE_IIC1_SDA(x)                   do{ x ?                                                                                             \
                                                    GPIO_SetBits(SOFTWARE_IIC1_SDA_GPIO_PORT, SOFTWARE_IIC1_SDA_GPIO_PIN) :    \
                                                    GPIO_ResetBits(SOFTWARE_IIC1_SDA_GPIO_PORT, SOFTWARE_IIC1_SDA_GPIO_PIN);   \
                                                }while(0)

#define SOFTWARE_IIC1_READ_SDA()               GPIO_ReadInputDataBit(SOFTWARE_IIC1_SDA_GPIO_PORT, SOFTWARE_IIC1_SDA_GPIO_PIN)

							

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

void Software_bitchange(uint8_t ADDR, uint8_t reg, uint8_t bitNum, uint8_t data);
uint8_t Software_Read_Byte(uint8_t ADDR,uint8_t reg,uint8_t len,uint8_t *buf);
uint8_t Software_Write_longByte(uint8_t ADDR,uint8_t reg,uint8_t len,uint8_t *buf);

#endif /* __IIC_H__ */
