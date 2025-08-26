/**
  ******************************************************************************
  * @file    bsp_iic.c
  * @author  fire
  * @version V1.0
  * @date    2024-xx-xx
  * @brief   IICӦ�ú����ӿ�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32 F103 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./iic/bsp_hard_i2c.h"
#include "./mpu6050/mpu6050.h"
#include "./usart/bsp_debug_usart.h"

I2C_HandleTypeDef mpu_i2c;

 /**
  * @brief  ��ʼ��I2C��IO
  * @param  ��
  * @retval ��
  */
void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(i2cHandle->Instance == MPU_I2C)
    {
        //����I2Cʱ��
        I2C_CLK_ENABLE();
        //����I2C��GPIOʱ��
        I2C_GPIO_CLK_ENABLE();
        
        //��ʼ��GPIO
        GPIO_InitStruct.Pin   = I2C_SCL_PIN | I2C_SDA_PIN;
        GPIO_InitStruct.Mode  = GPIO_MODE_AF_OD;//���ÿ�©���
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Pull  = GPIO_NOPULL;
        HAL_GPIO_Init(I2C_GPIO_PORT,&GPIO_InitStruct);
        
        //������ӳ��
//        __HAL_AFIO_REMAP_I2C1_ENABLE();
        __HAL_RCC_I2C1_FORCE_RESET();
        __HAL_RCC_I2C1_RELEASE_RESET();
        
    }   
}

 /**
  * @brief  ʧ��I2C��IO
  * @param  ��
  * @retval ��
  */
void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{

  if(i2cHandle->Instance == MPU_I2C)
  {
    //ʧ��I2Cʱ��
    I2C_CLK_DISABLE();

    //ʧ��SCL��SDA
    HAL_GPIO_DeInit(I2C_GPIO_PORT, I2C_SCL_PIN);
    HAL_GPIO_DeInit(I2C_GPIO_PORT, I2C_SDA_PIN);

  }
}

 /**
  * @brief  ����MPU6050��I2C
  * @param  ��
  * @retval ��
  */
void MPU_I2C_Config(void)
{
    mpu_i2c.Instance             = MPU_I2C;                    //ѡ��I2C1
    mpu_i2c.Init.ClockSpeed      = 400000;                     //ʱ��Ƶ��Ϊ400000
    mpu_i2c.Init.DutyCycle       = I2C_DUTYCYCLE_2;            //ʱ��ռ�ձ�Ϊlow/high = 2:1
    mpu_i2c.Init.OwnAddress1     = 0;                          //�豸�����ַ1ֻҪ��������ַ��һ������
    mpu_i2c.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;    //ѰַģʽΪ7λ
    mpu_i2c.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;    //������˫��ַģʽ
    mpu_i2c.Init.OwnAddress2     = 0;                          //�豸�����ַ2Ϊ0
    mpu_i2c.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;    //�������㲥ģʽ
    mpu_i2c.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;      //��ֹʱ���ӳ�����ģʽ�±����ֹ
    
    //��ʼ��I2C
    HAL_I2C_Init(&mpu_i2c);
    
    //I2Cʹ��
    __HAL_I2C_ENABLE(&mpu_i2c);
}

/**
  * @brief   ����I2Cдһ���ֽڵ� MPU6050��
  * @param   
  *		@arg pBuffer:������ָ��
  *		@arg WriteAddr:д��ַ 
  * @retval  ��
  */
uint32_t I2C_ByteWrite(uint8_t* pBuffer, uint8_t WriteAddr)
{
	HAL_StatusTypeDef status = HAL_OK;

	status = HAL_I2C_Mem_Write(&mpu_i2c, MPU6050_SLAVE_ADDRESS, (uint16_t)WriteAddr, I2C_MEMADD_SIZE_8BIT, pBuffer, 1, 1000); 

	/* Check the communication status */
	if(status != HAL_OK)
	{
        printf("д��ʧ��");
	/* Execute user timeout callback */
	//I2Cx_Error(Addr);
	}
	while (HAL_I2C_GetState(&mpu_i2c) != HAL_I2C_STATE_READY)
	{
		
	}


//	while (HAL_I2C_IsDeviceReady(&mpu_i2c, MPU6050_SLAVE_ADDRESS, 300, 300) == HAL_TIMEOUT);

	/* Wait for the end of the transfer */
	while (HAL_I2C_GetState(&mpu_i2c) != HAL_I2C_STATE_READY)
	{
		
	}
	return status;
}


/**
  * @brief   ��MPU6050�����ȡһ������ 
  * @param   
  *		@arg pBuffer:��Ŵ�EEPROM��ȡ�����ݵĻ�����ָ��
  *		@arg WriteAddr:�������ݵ�EEPROM�ĵ�ַ
  *     @arg NumByteToWrite:Ҫ��EEPROM��ȡ���ֽ���
  * @retval  ��
  */
uint32_t I2C_BufferRead(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead)
{
	HAL_StatusTypeDef status = HAL_OK;
	
	status=HAL_I2C_Mem_Read(&mpu_i2c,MPU6050_SLAVE_ADDRESS,ReadAddr, I2C_MEMADD_SIZE_8BIT, (uint8_t *)pBuffer, NumByteToRead,0xffff);

	return status;
}











