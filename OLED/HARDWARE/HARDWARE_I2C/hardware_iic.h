#ifndef __HARDWARE_IIC_H__
#define __HARDWARE_IIC_H__

#include "sys.h"


/**************************I2C�������壬I2C1��I2C2********************************/
#define             IIC2_I2Cx                                I2C3
#define             IIC2_I2C_APBxClock_FUN                   RCC_APB1PeriphClockCmd
#define             IIC2_I2C_CLK                             RCC_APB1Periph_I2C3
#define             IIC2_I2C_GPIO_APBxClock_FUN              RCC_AHB1PeriphClockCmd
#define             IIC2_I2C_SCL_GPIO_CLK                    RCC_AHB1Periph_GPIOA     
#define             IIC2_I2C_SCL_PORT                        GPIOA  
#define             IIC2_I2C_SCL_PIN                         GPIO_Pin_8 
#define             IIC2_I2C_SDA_GPIO_CLK                    RCC_AHB1Periph_GPIOC 
#define             IIC2_I2C_SDA_PORT                        GPIOC
#define             IIC2_I2C_SDA_PIN                         GPIO_Pin_9

/* STM32 I2C ����ģʽ */
#define I2C_Speed              100000  //*

/* �����ַֻҪ��STM32��ҵ�I2C������ַ��һ������ */
#define I2Cx_OWN_ADDRESS7      0X0B


/*�ȴ���ʱʱ��*/
#define I2CT_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT         ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))


void I2C_Mode_Config(void);

#endif /* __IIC_H__ */
