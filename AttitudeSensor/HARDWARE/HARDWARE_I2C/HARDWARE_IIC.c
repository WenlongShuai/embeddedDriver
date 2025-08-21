#include "HARDWARE_IIC.h"

/**
  * @brief  I2C I/O����
  * @param  ��
  * @retval ��
  */
static void I2C_GPIO_Config(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure; 

	/* ʹ���� I2C �йص�ʱ�� */
	IIC_I2C_APBxClock_FUN(IIC_I2C_CLK, ENABLE );
	IIC_I2C_GPIO_APBxClock_FUN(IIC_I2C_GPIO_CLK, ENABLE );
	
  /*!< GPIO configuration */
	/* Connect PXx to I2C_SCL*/
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);
  /* Connect PXx to I2C_SDA*/
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);
	
  /* I2C_SCL��I2C_SDA*/
  GPIO_InitStructure.GPIO_Pin = IIC_I2C_SCL_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	       // ��©���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(IIC_I2C_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = IIC_I2C_SDA_PIN;
  GPIO_Init(IIC_I2C_PORT, &GPIO_InitStructure);
}


/**
  * @brief  I2C ����ģʽ����
  * @param  ��
  * @retval ��
  */
void I2C_Mode_Config(void)
{
  I2C_InitTypeDef  I2C_InitStructure; 
	
	/* ��ʼ��GPIO */
	I2C_GPIO_Config();

  /* I2C ���� */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	
	/* �ߵ�ƽ�����ȶ����͵�ƽ���ݱ仯 SCL ʱ���ߵ�ռ�ձ� */
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	
  I2C_InitStructure.I2C_OwnAddress1 = I2Cx_OWN_ADDRESS7; 
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable ;
	 
	/* I2C��Ѱַģʽ */
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	
	/* ͨ������ */
  I2C_InitStructure.I2C_ClockSpeed = I2C_Speed;
  
	/* I2C ��ʼ�� */
  I2C_Init(IIC_I2Cx, &I2C_InitStructure);
  
	/* ʹ�� I2C */
  I2C_Cmd(IIC_I2Cx, ENABLE);  

	/* ���� I2C ȷ������ */
	I2C_AcknowledgeConfig(IIC_I2Cx, ENABLE);
}





