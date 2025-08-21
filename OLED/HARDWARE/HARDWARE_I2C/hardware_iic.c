#include "hardware_iic.h"
#include "delay.h"


/**
  * @brief  I2C1 I/O����
  * @param  ��
  * @retval ��
  */
static void I2C2_GPIO_Config(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure; 

	/* ʹ���� I2C �йص�ʱ�� */
	IIC2_I2C_APBxClock_FUN(IIC2_I2C_CLK, ENABLE );
	IIC2_I2C_GPIO_APBxClock_FUN(IIC2_I2C_SCL_GPIO_CLK | IIC2_I2C_SDA_GPIO_CLK, ENABLE );
	
  /*!< GPIO configuration */
	/* Connect PXx to I2C_SCL*/
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_I2C3);
  /* Connect PXx to I2C_SDA*/
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_I2C3);
	
  /* I2C_SCL��I2C_SDA*/
  GPIO_InitStructure.GPIO_Pin = IIC2_I2C_SCL_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	       // ��©���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(IIC2_I2C_SCL_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = IIC2_I2C_SDA_PIN;
  GPIO_Init(IIC2_I2C_SDA_PORT, &GPIO_InitStructure);
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
	I2C2_GPIO_Config();

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
  I2C_Init(IIC2_I2Cx, &I2C_InitStructure);
	/* ʹ�� I2C */
  I2C_Cmd(IIC2_I2Cx, ENABLE);  
	/* ���� I2C ȷ������ */
//	I2C_AcknowledgeConfig(IIC2_I2Cx, ENABLE);
}


