/**
  ******************************************************************************
  * @file    bsp_mpu6050.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2024-xx-xx
  * @brief    mpu6050����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103 STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "./mpu6050/mpu6050.h"
#include "./usart/bsp_debug_usart.h"

#ifdef i2c_soft
    #include "./iic/bsp_soft_i2c.h"
#else
    #include "./iic/bsp_hard_i2c.h"
#endif


/**
 * @brief  Manages error callback by re-initializing I2C.
 * @param  Addr: I2C Address
 * @retval None
 */
static void I2Cx_Error(uint8_t Addr)
{

    /* ���³�ʼ��I2C���� */
    #ifdef i2c_soft
        i2c_GPIO_Config();
        i2c_Stop();
    #else
        MPU_I2C_Config();
        /* �ָ�I2C�Ĵ���ΪĬ��ֵ */
        HAL_I2C_DeInit(&MPU6050_I2C_Handle);
    #endif
    
    MPU6050_Init();
}


#ifdef i2c_hard
/**
 * @brief  ���Ĵ����������ṩ���ϲ�Ľӿ�
 * @param  slave_addr: �ӻ���ַ
 * @param 	reg_addr:�Ĵ�����ַ
 * @param len��Ҫ��ȡ�ĳ���
 *	@param data_ptr:ָ��Ҫ�洢���ݵ�ָ��
 * @retval ����Ϊ0��������Ϊ��0
 */
int MPU6050_ReadData(unsigned char slave_addr, unsigned char reg_addr, unsigned short len,
                             unsigned char *data_ptr)
{

    HAL_StatusTypeDef status = HAL_OK;
    slave_addr <<= 1;
    status = HAL_I2C_Mem_Read(&MPU6050_I2C_Handle, slave_addr, reg_addr, I2C_MEMADD_SIZE_8BIT, data_ptr, len,
                              I2Cx_FLAG_TIMEOUT);
    /* ���ͨѶ״̬ */
    if (status != HAL_OK)
    {
        /* ���߳����� */
        I2Cx_Error(slave_addr);
    }
    while (HAL_I2C_GetState(&MPU6050_I2C_Handle) != HAL_I2C_STATE_READY)
    {
    }
    /* ���SENSOR�Ƿ����������һ�ζ�д���� */
    while (HAL_I2C_IsDeviceReady(&MPU6050_I2C_Handle, slave_addr, I2Cx_FLAG_TIMEOUT, I2Cx_FLAG_TIMEOUT) == HAL_TIMEOUT)
        ;
    /* �ȴ�������� */
    while (HAL_I2C_GetState(&MPU6050_I2C_Handle) != HAL_I2C_STATE_READY)
    {
    }
    return status;
}

/**
 * @brief  д�Ĵ����������ṩ���ϲ�Ľӿ�
 * @param  slave_addr: �ӻ���ַ
 * @param 	reg_addr:�Ĵ�����ַ
 * @param len��д��ĳ���
 *	@param data_ptr:ָ��Ҫд�������
 * @retval ����Ϊ0��������Ϊ��0
 */
int MPU6050_WriteReg(unsigned char slave_addr, unsigned char reg_addr, unsigned short len,
                              unsigned char *data_ptr)
{
    HAL_StatusTypeDef status = HAL_OK;
    slave_addr <<= 1;
    status = HAL_I2C_Mem_Write(&MPU6050_I2C_Handle, slave_addr, reg_addr, I2C_MEMADD_SIZE_8BIT, data_ptr, len,
                               I2Cx_FLAG_TIMEOUT);
    /* ���ͨѶ״̬ */
    if (status != HAL_OK)
    {
        /* ���߳����� */
        I2Cx_Error(slave_addr);
    }
    while (HAL_I2C_GetState(&MPU6050_I2C_Handle) != HAL_I2C_STATE_READY)
    {
    }
    /* ���SENSOR�Ƿ����������һ�ζ�д���� */
    while (HAL_I2C_IsDeviceReady(&MPU6050_I2C_Handle, slave_addr, I2Cx_FLAG_TIMEOUT, I2Cx_FLAG_TIMEOUT) == HAL_TIMEOUT)
        ;
    /* �ȴ�������� */
    while (HAL_I2C_GetState(&MPU6050_I2C_Handle) != HAL_I2C_STATE_READY)
    {
    }
    return status;
}

#else

/**
 * @brief  ���Ĵ����������ṩ���ϲ�Ľӿ�
 * @param  slave_addr: �ӻ���ַ
 * @param 	reg_addr:�Ĵ�����ַ
 * @param len��Ҫ��ȡ�ĳ���
 *	@param data_ptr:ָ��Ҫ�洢���ݵ�ָ��
 * @retval ����Ϊ0��������Ϊ��0
 */
int MPU6050_ReadData(unsigned char slave_addr, unsigned char reg_addr, unsigned short len,
                             unsigned char *data_ptr)
{
	unsigned char i;

    slave_addr <<= 1;
	
	i2c_Start();
	i2c_SendByte(slave_addr);
    
	if(i2c_WaitAck())
        I2Cx_Error(slave_addr);
    
	i2c_SendByte(reg_addr);
	
    if(i2c_WaitAck())
        I2Cx_Error(slave_addr);
	
	i2c_Start();
	i2c_SendByte(MPU6050_SLAVE_ADDRESS+1);
	
    if(i2c_WaitAck())
        I2Cx_Error(slave_addr);
	
	for(i=0;i<(len-1);i++){
		*data_ptr=i2c_ReadByte(1);
		data_ptr++;
	}
	*data_ptr=i2c_ReadByte(0);
	i2c_Stop();
    return 0;
}

/**
 * @brief  д�Ĵ����������ṩ���ϲ�Ľӿ�
 * @param  slave_addr: �ӻ���ַ
 * @param 	reg_addr:�Ĵ�����ַ
 * @param len��д��ĳ���
 *	@param data_ptr:ָ��Ҫд�������
 * @retval ����Ϊ0��������Ϊ��0
 */
int MPU6050_WriteReg(unsigned char slave_addr, unsigned char reg_addr, unsigned short len,
                              unsigned char *data_ptr)
{
    unsigned short i = 0;
    slave_addr <<= 1;
    i2c_Start();
    i2c_SendByte(slave_addr);
    
    if(i2c_WaitAck())
        I2Cx_Error(slave_addr);
    
    i2c_SendByte(reg_addr);
    
    if(i2c_WaitAck())
        I2Cx_Error(slave_addr);
    
    for(i=0;i<len;i++)
    {
        i2c_SendByte(data_ptr[i]);
        if(i2c_WaitAck())
            I2Cx_Error(slave_addr);
    }
    i2c_Stop();
    return 0;
}

#endif

inline void get_ms_user(unsigned long *count) //���Ǻ���  inline���Σ����������ٳ���ջ����
{
    *count = HAL_GetTick();
}


/**
  * @brief   ��ʼ��MPU6050��INT����
  * @param   
  * @retval  
  */
void MPU_INT_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = MPU6050_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(MPU6050_INT_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}


/**
  * @brief   ��ʼ��MPU6050оƬ
  * @param   
  * @retval  
  */
void MPU6050_Init(void)
{
  int i=0,j=0;
  uint8_t reg[5]={0x00,0x07,0x06,0x01,0x18};
  //�ڳ�ʼ��֮ǰҪ��ʱһ��ʱ�䣬��û����ʱ����ϵ�����ϵ����ݿ��ܻ����
  for(i=0;i<1000;i++)
  {
    for(j=0;j<1000;j++)
    {
      ;
    }
  }
	MPU6050_WriteReg(MPU6050_SLAVE_ADDRESS,MPU6050_RA_PWR_MGMT_1,1, &reg[0]);	     //�������״̬
	MPU6050_WriteReg(MPU6050_SLAVE_ADDRESS,MPU6050_RA_SMPLRT_DIV ,1, &reg[1]);	    //�����ǲ�����
	MPU6050_WriteReg(MPU6050_SLAVE_ADDRESS,MPU6050_RA_CONFIG , 1,&reg[2]);	
	MPU6050_WriteReg(MPU6050_SLAVE_ADDRESS,MPU6050_RA_ACCEL_CONFIG ,1, &reg[3]);	  //���ü��ٶȴ�����������4Gģʽ
	MPU6050_WriteReg(MPU6050_SLAVE_ADDRESS,MPU6050_RA_GYRO_CONFIG,1, &reg[4]);     //�������Լ켰������Χ������ֵ��0x18(���Լ죬2000deg/s)
}


