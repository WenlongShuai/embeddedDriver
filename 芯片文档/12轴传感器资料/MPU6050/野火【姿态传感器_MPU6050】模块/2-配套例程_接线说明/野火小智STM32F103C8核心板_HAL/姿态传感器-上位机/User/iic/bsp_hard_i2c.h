#ifndef __BSP_IIC_H
#define	__BSP_IIC_H

#include "stm32f1xx.h"

#define  MPU_I2C    I2C1

#define  I2C_CLK_ENABLE()        __HAL_RCC_I2C1_CLK_ENABLE()
#define  I2C_CLK_DISABLE()       __HAL_RCC_I2C1_CLK_DISABLE()

    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */
//�궨��IIC��GPIO
#define  I2C_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()
#define  I2C_SCL_PIN             GPIO_PIN_6
#define  I2C_SDA_PIN             GPIO_PIN_7
#define  I2C_GPIO_PORT           GPIOB

/* �����ַֻҪ��STM32��ҵ�I2C������ַ��һ������ */
#define I2Cx_OWN_ADDRESS7      0X0A 




void MPU_I2C_Config(void);
uint32_t I2C_ByteWrite(uint8_t* pBuffer, uint8_t WriteAddr);                                          
uint32_t I2C_BufferRead(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead);
#endif /* __BSP_IIC_H */
