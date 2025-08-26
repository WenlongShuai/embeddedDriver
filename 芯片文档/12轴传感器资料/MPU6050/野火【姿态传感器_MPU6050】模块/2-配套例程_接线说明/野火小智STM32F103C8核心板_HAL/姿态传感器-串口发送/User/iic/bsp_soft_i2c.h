#ifndef _BSP_I2C_H
#define _BSP_I2C_H

#include <inttypes.h>

#define I2C_WR	0		/* д����bit */
#define I2C_RD	1		/* ������bit */

/* ����I2C�������ӵ�GPIO�˿�, �û�ֻ��Ҫ�޸�����4�д��뼴������ı�SCL��SDA������ */
#define GPIO_PORT_SOFT_I2C	GPIOB			/* GPIO�˿� */
#define SOFT_I2C_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()
#define SOFT_I2C_SCL_PIN		GPIO_PIN_10			/* ���ӵ�SCLʱ���ߵ�GPIO */
#define SOFT_I2C_SDA_PIN		GPIO_PIN_11			/* ���ӵ�SDA�����ߵ�GPIO */

void i2c_Start(void);
void i2c_Stop(void);
void i2c_SendByte(uint8_t _ucByte);
uint8_t i2c_ReadByte(uint8_t ack);
uint8_t i2c_WaitAck(void);
void i2c_Ack(void);
void i2c_NAck(void);
uint8_t i2c_CheckDevice(uint8_t _Address);
void i2c_GPIO_Config(void);

#endif
