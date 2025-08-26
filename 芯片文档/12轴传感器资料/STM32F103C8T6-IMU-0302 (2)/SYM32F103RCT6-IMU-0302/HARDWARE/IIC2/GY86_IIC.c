#include "GY86_IIC.h"
#include "delay.h"
#include "stm32f10x_i2c.h"
//********�ӿڶ���*********************************************************//
//���ÿ⺯���� I2C2�˿����úã�����ʹ�õ��� PB10��PB11 �˿ڣ���
//�����������:
//************************************************************************//

static __IO uint32_t  I2CTimeout = I2CT_LONG_TIMEOUT;

void GY86_IIC_Init(void)
	{ 
		
	GPIO_InitTypeDef GPIO_InitStructure;                                              //GPIO �ṹ�嶨��		
	I2C_InitTypeDef I2C_InitStructure;                                                //I2C �ṹ�嶨��
	
  //*I2C2-ģʽ ����*//
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);	                              //I2C2 ʱ��ʹ��		
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;                                        //ѡ��I2C����
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;                                       //I2CӦ��ʹ��
	I2C_InitStructure.I2C_ClockSpeed = 400000;                                        //ʱ�����ʣ��� HZ Ϊ��λ�ģ����Ϊ 400khz
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;                                //�ò���ֻ���� I2C �����ڿ���ģʽ��ʱ�ӹ���Ƶ�ʸ��� 100KHz���²�������
	I2C_InitStructure.I2C_OwnAddress1 =0x0a;                                          //���õ�һ���豸�����ַ
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;         //MPU6050 ��ַΪ 7 λ�������� 7 λ������
	I2C_Init(I2C2, &I2C_InitStructure);                                               //��ʼ���ṹ������
	I2C_Cmd(I2C2, ENABLE);                                                            //ʹ�� I2C2
			
			//*I2C2-IO ������*//
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);                              //ʹ��GPIOBʱ��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;                                   //���ù��ܵĿ�©���
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;                          //ʹ��B.10��B.11
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                 //�ٶ����50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);                                            //��ʼ���ṹ������  
	}
	
	
//************IIC���ֽ�д����******************************************//	
u8 GY86_Write_shortByte(u8 ADDR,u8 reg,u8 REG_data)
{   
	  delay_ms(2); 	
		I2C_GenerateSTART(I2C2, ENABLE);   
		I2CTimeout = I2CT_FLAG_TIMEOUT;	//������ʼλ
	
		while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))                      //��� EV5
		{
			if((I2CTimeout--) == 0) return 1;
		}
			
		I2CTimeout = I2CT_FLAG_TIMEOUT;	
		I2C_Send7bitAddress(I2C2,ADDR,I2C_Direction_Transmitter);                        //����������ַ 
		while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))         //ADDR=1����� EV6
		{
			if((I2CTimeout--) == 0) return 2;
		}
		
		I2C_SendData(I2C2,reg);                                                          //�Ĵ��������ַ
		
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED))                 //��λ�Ĵ����ǿգ����ݼĴ����Ѿ��գ����� EV8���������ݵ� DR �ȿ�������¼�
		{
			if((I2CTimeout--) == 0) return 3;
		}
			
			
		I2C_SendData(I2C2, REG_data);                                                    //��������
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		
		while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED))                 //���EV8
		{
			if((I2CTimeout--) == 0) return 4;
		}
			
		I2C_GenerateSTOP(I2C2, ENABLE);                                                  //����ֹͣ�ź�
	return 0;
}

//************IIC���ֽ�д����******************************************//	
u8 G86_Write_longByte(u8 ADDR,u8 reg,u8 len,u8 *buf)
{   
	  delay_ms(2); 
		I2C_GenerateSTART(I2C2, ENABLE);                                                  //������ʼλ
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))                       //��� EV5
		{
			if((I2CTimeout--) == 0) return 1;
		}
		
		I2C_Send7bitAddress(I2C2, ADDR, I2C_Direction_Transmitter);                       //����������ַ
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		
		while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))          //ADDR=1����� EV6
		{
			if((I2CTimeout--) == 0) return 2;
		}
			
		I2C_SendData(I2C2, reg);                                                          //�Ĵ��������ַ
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		
		while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED))                  //��λ�Ĵ����ǿգ����ݼĴ����Ѿ��գ����� EV8���������ݵ� DR �ȿ�������¼�
		{
			if((I2CTimeout--) == 0) return 3;
		}
			
		while(len--)                                                                      //���� while ѭ�� ��������
		{
			I2C_SendData(I2C2, *buf);                                                     //��������
			buf++;                                                                        //����ָ����λ
			I2CTimeout = I2CT_FLAG_TIMEOUT;
			while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED))             //���EV8
			{
				if((I2CTimeout--) == 0) return 4;
			}
		}
		I2C_GenerateSTOP(I2C2, ENABLE);                                                   //����ֹͣ�ź�
	return 0;
}



//***********IIC���ֽڶ�����********************************************//
u8 GY86_Read_Byte(u8 ADDR,u8 reg,u8 len,u8 *buf)                                //��Ҫ������ʼ�ź�

{  
   	delay_ms(2);
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY))                                    //���ÿ⺯����� I2C �����Ƿ��� BUSY ״̬
		{
			if((I2CTimeout--) == 0) return 1;
		}
		I2C_GenerateSTART(I2C2, ENABLE);                                                  //�����ź�
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		
		while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))                       //��� EV5
		{
			if((I2CTimeout--) == 0) return 2;
		}
			
		I2C_Send7bitAddress(I2C2, ADDR, I2C_Direction_Transmitter);                       //д��������ַ
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		
		while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))        //��� EV6
		{
			if((I2CTimeout--) == 0) return 3;
		}
			
			
		I2C_SendData(I2C2, reg);                                                          //���Ͷ��ĵ�ַ
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		
		while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED))                  //��� EV8
		{
			if((I2CTimeout--) == 0) return 4;
		}
			
		I2C_GenerateSTART(I2C2, ENABLE);                                                  //�����ź�
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		
		while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))                       //��� EV5
		{
			if((I2CTimeout--) == 0) return 5;
		}
			
		I2C_Send7bitAddress(I2C2, ADDR, I2C_Direction_Receiver);                          //��������ַ����������Ϊ��
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		
		while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))            //���EV6
		{
			if((I2CTimeout--) == 0) return 6;
		}
			
		while(len)
		{
						if(len == 1)//ֻʣ�����һ������ʱ���� if ���
							{
									I2C_AcknowledgeConfig(I2C2, DISABLE);//�����һ������ʱ�ر�Ӧ��λ
									I2C_GenerateSTOP(I2C2, ENABLE);//���һ������ʱʹ��ֹͣλ
							}
						if(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED)) //��ȡ����
							{
									*buf = I2C_ReceiveData(I2C2);//���ÿ⺯��������ȡ���� pBuffer
									buf++; //ָ����λ
									len--;//�ֽ����� 1
						 // 	delay_ms(1);
							}
				}				
		I2C_AcknowledgeConfig(I2C2, ENABLE);                                              //��Ӧ��λʹ�ܻ�ȥ���ȴ��´�ͨ��	
return 0;				
}


//************IIC����д����******************************************//	
u8 GY86_Write_commendByte(u8 ADDR,u8 reg)
{   
	  delay_ms(2);                                                                     //д�ֽڼ��
		I2C_GenerateSTART(I2C2, ENABLE);                                                 //������ʼλ
		I2CTimeout = I2CT_FLAG_TIMEOUT;
	
		while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));                      //��� EV5
		{
			if((I2CTimeout--) == 0) return 1;
		}
			
		I2C_Send7bitAddress(I2C2,ADDR,I2C_Direction_Transmitter);                        //����������ַ 
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		
		while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))         //ADDR=1����� EV6
		{
			if((I2CTimeout--) == 0) return 2;
		}
			
		I2C_SendData(I2C2,reg);                                                          //�Ĵ��������ַ
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		
		while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED))                 //��λ�Ĵ����ǿգ����ݼĴ����Ѿ��գ����� EV8���������ݵ� DR �ȿ�������¼�
		{
			if((I2CTimeout--) == 0) return 3;
		}
			
		I2C_GenerateSTOP(I2C2, ENABLE);                                                  //����ֹͣ�ź�
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
void GY86_bitchange( u8 ADDR, u8 reg, u8 bitNum, u8 data)
{
	  u8 buffer[1],b;
    GY86_Read_Byte(ADDR,reg,1,buffer);
	  b=buffer[0];
    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
		GY86_Write_shortByte(ADDR, reg, b);
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
u8 GY86_bitschange(u8 ADDR, u8 reg,u8 bitStart,u8 length,u8 data)
{

    u8 buffer[1],b;
    if (GY86_Read_Byte(ADDR,reg,1,buffer) == 0) {
        u8 mask = (0xFF << (bitStart + 1)) | 0xFF >> ((8 - bitStart) + length - 1);
        data <<= (8 - length);
        data >>= (7 - bitStart);
        b &= mask;
        b |= data;
        return GY86_Write_shortByte(ADDR, reg, b);
    } else {
        return 1;
    }
}
