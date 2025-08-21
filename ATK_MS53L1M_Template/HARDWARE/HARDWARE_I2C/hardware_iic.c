#include "hardware_iic.h"

static __IO uint32_t  I2CTimeout = HARDWARE_IICx_LONG_TIMEOUT;

void Hardware_IIC_Init(void)
{ 
	GPIO_InitTypeDef GPIO_InitStructure;                                              //GPIO �ṹ�嶨��		
	I2C_InitTypeDef I2C_InitStructure;                                                //I2C �ṹ�嶨��

	//*I2C-IO ������*//
	HARDWARE_IICx_GPIO_APBxClock_FUN(HARDWARE_IICx_SCL_GPIO_CLK | HARDWARE_IICx_SDA_GPIO_CLK, ENABLE);                              //ʹ��GPIOBʱ��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;                                   //���ù��ܵĿ�©���
	GPIO_InitStructure.GPIO_Pin = HARDWARE_IICx_SCL_PIN;                          
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;                                 //�ٶ����100MHz
	GPIO_Init(HARDWARE_IICx_SCL_PORT, &GPIO_InitStructure);                                            //��ʼ���ṹ������ 
	
	GPIO_InitStructure.GPIO_Pin = HARDWARE_IICx_SDA_PIN;                                 //�ٶ����50MHz
	GPIO_Init(HARDWARE_IICx_SDA_PORT, &GPIO_InitStructure);                                            //��ʼ���ṹ������ 
			
	//*I2C-ģʽ ����*//
	HARDWARE_IICx_APBxClock_FUN(HARDWARE_IICx_CLK,ENABLE);	                              //I2C2 ʱ��ʹ��		
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;                                        //ѡ��I2C����
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;                                       //I2CӦ��ʹ��
	I2C_InitStructure.I2C_ClockSpeed = HARDWARE_IICx_Speed;                                        //ʱ�����ʣ��� HZ Ϊ��λ�ģ����Ϊ 400khz
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;                                //�ò���ֻ���� I2C �����ڿ���ģʽ��ʱ�ӹ���Ƶ�ʸ��� 100KHz���²�������
	I2C_InitStructure.I2C_OwnAddress1 = HARDWARE_IICx_OWN_ADDRESS7;                                          //���õ�һ���豸�����ַ
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;         
	I2C_Init(HARDWARE_IICx_I2Cx, &I2C_InitStructure);                                               //��ʼ���ṹ������
	I2C_Cmd(HARDWARE_IICx_I2Cx, ENABLE);               
}
	
	
//************IIC���ֽ�д����******************************************//	
uint8_t Hardware_Write_shortByte(uint8_t ADDR, uint8_t reg, uint8_t REG_data)
{
	I2C_GenerateSTART(HARDWARE_IICx_I2Cx, ENABLE);   
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;	//������ʼλ

	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))                      //��� EV5
	{
		if((I2CTimeout--) == 0) return 1;
	}
		
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;	
	I2C_Send7bitAddress(HARDWARE_IICx_I2Cx,ADDR,I2C_Direction_Transmitter);                        //����������ַ 
	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))         //ADDR=1����� EV6
	{
		if((I2CTimeout--) == 0) return 2;
	}
	
	I2C_SendData(HARDWARE_IICx_I2Cx,reg);                                                          //�Ĵ��������ַ
	
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;
	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))                 //��λ�Ĵ����ǿգ����ݼĴ����Ѿ��գ����� EV8���������ݵ� DR �ȿ�������¼�
	{
		if((I2CTimeout--) == 0) return 3;
	}
		
		
	I2C_SendData(HARDWARE_IICx_I2Cx, REG_data);                                                    //��������
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;
	
	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))                 //���EV8
	{
		if((I2CTimeout--) == 0) return 4;
	}
		
	I2C_GenerateSTOP(HARDWARE_IICx_I2Cx, ENABLE);                                                  //����ֹͣ�ź�
	
	return 0;
}

//************IIC���ֽ�д����******************************************//	
uint8_t Hardware_Write_longByte(uint8_t ADDR, uint8_t reg, uint8_t len, uint8_t *buf)
{   
	I2C_GenerateSTART(HARDWARE_IICx_I2Cx, ENABLE);                                                  //������ʼλ
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;
	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))                       //��� EV5
	{
		if((I2CTimeout--) == 0) return 1;
	}

	I2C_Send7bitAddress(HARDWARE_IICx_I2Cx, ADDR, I2C_Direction_Transmitter);                       //����������ַ
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;

	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))          //ADDR=1����� EV6
	{
		if((I2CTimeout--) == 0) return 2;
	}
		
	I2C_SendData(HARDWARE_IICx_I2Cx, reg);                                                          //�Ĵ��������ַ
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;

	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))                  //��λ�Ĵ����ǿգ����ݼĴ����Ѿ��գ����� EV8���������ݵ� DR �ȿ�������¼�
	{
		if((I2CTimeout--) == 0) return 3;
	}
		
	while(len--)                                                                      //���� while ѭ�� ��������
	{
		I2C_SendData(HARDWARE_IICx_I2Cx, *buf);                                                     //��������
		buf++;                                                                        //����ָ����λ
		I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;
		while (!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))             //���EV8
		{
			if((I2CTimeout--) == 0) return 4;
		}
	}
	I2C_GenerateSTOP(HARDWARE_IICx_I2Cx, ENABLE);                                                   //����ֹͣ�ź�

	return 0;
}

uint8_t Hardware_Write_nbyte(uint8_t dev_address, uint16_t address, uint8_t len, uint8_t *wbuf)
{		
	I2C_GenerateSTART(HARDWARE_IICx_I2Cx, ENABLE);                                                  //������ʼλ
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;
	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))                       //��� EV5
	{
		if((I2CTimeout--) == 0) return 1;
	}

	I2C_Send7bitAddress(HARDWARE_IICx_I2Cx, dev_address, I2C_Direction_Transmitter);                       //����������ַ
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;

	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))          //ADDR=1����� EV6
	{
		if((I2CTimeout--) == 0) return 2;
	}
		
	I2C_SendData(HARDWARE_IICx_I2Cx, address >> 8);                                                        
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;
	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))                  //��λ�Ĵ����ǿգ����ݼĴ����Ѿ��գ����� EV8���������ݵ� DR �ȿ�������¼�
	{
		if((I2CTimeout--) == 0) return 3;
	}
	
	I2C_SendData(HARDWARE_IICx_I2Cx, address & 0x00FF);                                                        
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;
	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))                  //��λ�Ĵ����ǿգ����ݼĴ����Ѿ��գ����� EV8���������ݵ� DR �ȿ�������¼�
	{
		if((I2CTimeout--) == 0) return 4;
	}
		
	while(len--)                                                                      //���� while ѭ�� ��������
	{
		I2C_SendData(HARDWARE_IICx_I2Cx, *wbuf);                                                     //��������
		wbuf++;                                                                        //����ָ����λ
		I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;
		while (!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))             //���EV8
		{
			if((I2CTimeout--) == 0) return 5;
		}
	}
	I2C_GenerateSTOP(HARDWARE_IICx_I2Cx, ENABLE);                                                   //����ֹͣ�ź�

	return 0;
		
		
}

//***********IIC���ֽڶ�����********************************************//
uint8_t Hardware_Read_Byte(uint8_t ADDR, uint8_t reg, uint8_t len, uint8_t *buf)                                //��Ҫ������ʼ�ź�
{  
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;
	
	while(I2C_GetFlagStatus(HARDWARE_IICx_I2Cx, I2C_FLAG_BUSY))                                    //���ÿ⺯����� I2C �����Ƿ��� BUSY ״̬
	{
		if((I2CTimeout--) == 0) return 1;
	}
	I2C_GenerateSTART(HARDWARE_IICx_I2Cx, ENABLE);                                                  //�����ź�
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;

	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))                       //��� EV5
	{
		if((I2CTimeout--) == 0) return 2;
	}
		
	I2C_Send7bitAddress(HARDWARE_IICx_I2Cx, ADDR, I2C_Direction_Transmitter);                       //д��������ַ
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;

	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))        //��� EV6
	{
		if((I2CTimeout--) == 0) return 3;
	}
		
		
	I2C_SendData(HARDWARE_IICx_I2Cx, reg);                                                          //���Ͷ��ĵ�ַ
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;

	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))                  //��� EV8
	{
		if((I2CTimeout--) == 0) return 4;
	}
		
	I2C_GenerateSTART(HARDWARE_IICx_I2Cx, ENABLE);                                                  //�����ź�
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;

	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))                       //��� EV5
	{
		if((I2CTimeout--) == 0) return 5;
	}
		
	I2C_Send7bitAddress(HARDWARE_IICx_I2Cx, ADDR|1, I2C_Direction_Receiver);                          //��������ַ����������Ϊ��
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;

	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))            //���EV6
	{
		if((I2CTimeout--) == 0) return 6;
	}
		
	while(len)
	{
		if(len == 1)//ֻʣ�����һ������ʱ���� if ���
		{
			I2C_AcknowledgeConfig(HARDWARE_IICx_I2Cx, DISABLE);//�����һ������ʱ�ر�Ӧ��λ
			I2C_GenerateSTOP(HARDWARE_IICx_I2Cx, ENABLE);//���һ������ʱʹ��ֹͣλ
		}
		if(I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)) //��ȡ����
		{
			*buf = I2C_ReceiveData(HARDWARE_IICx_I2Cx);//���ÿ⺯��������ȡ���� pBuffer
			buf++; //ָ����λ
			len--;//�ֽ����� 1
			//delay_ms(1);
		}
	}				
	I2C_AcknowledgeConfig(HARDWARE_IICx_I2Cx, ENABLE);                                              //��Ӧ��λʹ�ܻ�ȥ���ȴ��´�ͨ��	
			
	return 0;				
}

uint8_t Hardware_Read_nbyte(uint8_t dev_address, uint16_t data, uint8_t len, uint8_t *rbuf)
{
	delay_ms(2);
	
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;
	
	while(I2C_GetFlagStatus(HARDWARE_IICx_I2Cx, I2C_FLAG_BUSY))                                    //���ÿ⺯����� I2C �����Ƿ��� BUSY ״̬
	{
		if((I2CTimeout--) == 0) return 1;
	}
	I2C_GenerateSTART(HARDWARE_IICx_I2Cx, ENABLE);                                                  //�����ź�
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;

	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))                       //��� EV5
	{
		if((I2CTimeout--) == 0) return 2;
	}
		
	I2C_Send7bitAddress(HARDWARE_IICx_I2Cx, dev_address, I2C_Direction_Transmitter);                       //д��������ַ
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;

	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))        //��� EV6
	{
		if((I2CTimeout--) == 0) return 3;
	}
		
		
	I2C_SendData(HARDWARE_IICx_I2Cx, data >> 8);                                                     
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;

	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))                  //��� EV8
	{
		if((I2CTimeout--) == 0) return 4;
	}
	
	I2C_SendData(HARDWARE_IICx_I2Cx, data & 0x00FF);                                                     
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;

	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))                  //��� EV8
	{
		if((I2CTimeout--) == 0) return 5;
	}
		
	I2C_GenerateSTART(HARDWARE_IICx_I2Cx, ENABLE);                                                  //�����ź�
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;

	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))                       //��� EV5
	{
		if((I2CTimeout--) == 0) return 6;
	}
		
	I2C_Send7bitAddress(HARDWARE_IICx_I2Cx, dev_address|1, I2C_Direction_Receiver);                          //��������ַ����������Ϊ��
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;

	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))            //���EV6
	{
		if((I2CTimeout--) == 0) return 7;
	}
		
	while(len)
	{
		if(len == 1)//ֻʣ�����һ������ʱ���� if ���
		{
			I2C_AcknowledgeConfig(HARDWARE_IICx_I2Cx, DISABLE);//�����һ������ʱ�ر�Ӧ��λ
			I2C_GenerateSTOP(HARDWARE_IICx_I2Cx, ENABLE);//���һ������ʱʹ��ֹͣλ
		}
		if(I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)) //��ȡ����
		{
			*rbuf = I2C_ReceiveData(HARDWARE_IICx_I2Cx);//���ÿ⺯��������ȡ���� pBuffer
			rbuf++; //ָ����λ
			len--;//�ֽ����� 1
			//delay_ms(1);
		}
	}				
	I2C_AcknowledgeConfig(HARDWARE_IICx_I2Cx, ENABLE);                                              //��Ӧ��λʹ�ܻ�ȥ���ȴ��´�ͨ��	
			
	return 0;				

}

//************IIC����д����******************************************//	
uint8_t Hardware_Write_commendByte(uint8_t ADDR, uint8_t reg)
{   
	I2C_GenerateSTART(HARDWARE_IICx_I2Cx, ENABLE);                                                 //������ʼλ
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;

	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))                      //��� EV5
	{
		if((I2CTimeout--) == 0) return 1;
	}
		
	I2C_Send7bitAddress(HARDWARE_IICx_I2Cx, ADDR, I2C_Direction_Transmitter);                        //����������ַ 
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;

	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))         //ADDR=1����� EV6
	{
		if((I2CTimeout--) == 0) return 2;
	}
		
	I2C_SendData(HARDWARE_IICx_I2Cx,reg);                                                          //�Ĵ��������ַ
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;

	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))                 //��λ�Ĵ����ǿգ����ݼĴ����Ѿ��գ����� EV8���������ݵ� DR �ȿ�������¼�
	{
		if((I2CTimeout--) == 0) return 3;
	}
		
	I2C_GenerateSTOP(HARDWARE_IICx_I2Cx, ENABLE);                                                  //����ֹͣ�ź�
			
	return 0;
}

/**************************IIC���ֽڵ�λ�ı�********************************************
*����ԭ��:		u8 IICwriteBit(u8 dev, u8 reg, u8 bitNum, u8 data)
*��������:	     �� �޸� д ָ���豸 ָ���Ĵ���һ���ֽ� �е�1��λ
���룺	ADDR     ������ַ
      reg	     �Ĵ�����ַ
		  bitNum   Ҫ�޸�Ŀ���ֽڵ�bitNumλ
		  data     Ϊ0 ʱ��Ŀ��λ������0 ���򽫱���λ
		   
����   �ɹ�Ϊ1  
 		   ʧ��Ϊ0 
*******************************************************************************/ 
void Hardware_bitchange(uint8_t ADDR, uint8_t reg, uint8_t bitNum, uint8_t data)
{
	u8 buffer[1],b;

	Hardware_Read_Byte(ADDR,reg,1,buffer);
	b=buffer[0];
	b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
	Hardware_Write_shortByte(ADDR, reg, b);
}

/**************************IIC���ֽڶ�λ�ı�********************************************
*����ԭ��:		u8 IICwriteBits(u8 dev,u8 reg,u8 bitStart,u8 length,u8 data)
*��������:	    �� �޸� д ָ���豸 ָ���Ĵ���һ���ֽ� �еĶ��λ
����;  ADDR       ������ַ
       reg	      �Ĵ�����ַ
		   bitStart   Ŀ���ֽڵ���ʼλ
	     length     λ����
		   data       ��Ÿı�Ŀ���ֽ�λ��ֵ
		
����   �ɹ� Ϊ1 
 		   ʧ��Ϊ0
*******************************************************************************/ 
uint8_t Hardware_bitschange(uint8_t ADDR, uint8_t reg, uint8_t bitStart, uint8_t length, uint8_t data)
{ 
	u8 buffer[1],b;

	if (Hardware_Read_Byte(ADDR, reg, 1, buffer) == 0) 
	{
		u8 mask = (0xFF << (bitStart + 1)) | 0xFF >> ((8 - bitStart) + length - 1);
		data <<= (8 - length);
		data >>= (7 - bitStart);
		b &= mask;
		b |= data;
		return Hardware_Write_shortByte(ADDR, reg, b);
	} 
			
	return 1;
}

