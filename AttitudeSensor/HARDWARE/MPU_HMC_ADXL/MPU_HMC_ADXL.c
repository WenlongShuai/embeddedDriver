#include "MPU_HMC_ADXL.h"

extern __IO uint32_t FlashID;

uint8_t RecvBuff[RECVBUFF_SIZE];

/* HMC5883L */
/*------------------------- У׼���� --------------------------*/
short x_offset = 0;       // X�����ƫ�Ʋ���
short y_offset = 0;       // Y�����ƫ�Ʋ���
float x_scale = 1.0;      // X�����ϵ������ԲУ׼�ã�
float y_scale = 1.0;      // Y�����ϵ������ԲУ׼�ã�
unsigned char HMC_BUF[8] = {0};     // ���ݻ��������洢6������+2Ԥ����

/* MPU3050 */
unsigned char TX_DATA[4];           //��ʾ�ݻ�����
unsigned char MPU_BUF[10] = {0};       //�������ݻ�����


/* ADXL345 */
unsigned char ADXL_BUF[6] = {0};


#ifndef SOFTWARE_IIC  //Ӳ��I2C
static __IO uint32_t  I2CTimeout = I2CT_LONG_TIMEOUT;
#endif


void MPU_HMC_ADXL_Init(void)
{
	#ifdef SOFTWARE_IIC  //���I2C
	IIC_Init();
	#else  //Ӳ��I2C
	I2C_Mode_Config();
	#endif
	
//	I2C1_DMA_Init();
	
	Init_HMC5883();
	Init_MPU3050();
	Init_ADXL345();
	SPI_FLASH_Init();
}

/*------------------- IIC ���ֽ�д���� -------------------*/
void IIC_Single_Write(unsigned char Slave_Address, unsigned char REG_Address, unsigned short Len,unsigned char *Data_Buf)
{
	// д�Ĵ�������
	
	#ifdef SOFTWARE_IIC  //���I2C
	IIC_Start();
	IIC_Send_Byte(Slave_Address);
	IIC_Wait_Ack();
	
	for(int i = 0;i < Len;i++)
	{
		IIC_Send_Byte(REG_Address+i);
		IIC_Wait_Ack();
		IIC_Send_Byte(Data_Buf[i]);
		IIC_Wait_Ack();
	}
	IIC_Stop();
	#else   //Ӳ��I2C
	/* Send STRAT condition */
  I2C_GenerateSTART(IIC_I2Cx, ENABLE);
	I2CTimeout = I2CT_FLAG_TIMEOUT; 
	
	/* Test on EV5 and clear it */
  while(!I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))  
  {
    if((I2CTimeout--) == 0) return;
  } 
	
	I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Send address for write */
  I2C_Send7bitAddress(IIC_I2Cx, Slave_Address, I2C_Direction_Transmitter);
	
	/* Test on EV6 and clear it */
  while(!I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((I2CTimeout--) == 0) return;
  }  
	
	
	for(int i = 0;i < Len;i++)
	{
		/* Send internal address to write to */
		I2C_SendData(IIC_I2Cx, REG_Address+i);
		
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		/* Test on EV8 and clear it */
		while(!I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
		{
			if((I2CTimeout--) == 0) return;
		} 
		
		/* Send the byte to be written */
		I2C_SendData(IIC_I2Cx, Data_Buf[i]); 
		
		I2CTimeout = I2CT_FLAG_TIMEOUT;  
		/* Test on EV8 and clear it */
		while(!I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
		{
			if((I2CTimeout--) == 0) return;
		} 
	}
  
  /* Send STOP condition */
  I2C_GenerateSTOP(IIC_I2Cx, ENABLE);
	
	#endif
}

unsigned char IIC_Single_Read(unsigned char Slave_Address, unsigned char REG_Address)
{
	unsigned char REG_data = 0;
	
	// ���Ĵ�������
	#ifdef SOFTWARE_IIC  //���I2C
	IIC_Start();
	IIC_Send_Byte(Slave_Address);
	IIC_Wait_Ack();
	IIC_Send_Byte(REG_Address);
	IIC_Wait_Ack();
	IIC_Start();
	IIC_Send_Byte(Slave_Address+1);
	IIC_Wait_Ack();
	REG_data = IIC_Read_Byte(0);
	IIC_Stop();
	#else   //Ӳ��I2C
	I2CTimeout = I2CT_LONG_TIMEOUT;
  
  //*((u8 *)0x4001080c) |=0x80; 
  while(I2C_GetFlagStatus(IIC_I2Cx, I2C_FLAG_BUSY))
  {
    if((I2CTimeout--) == 0) return 0;
  }
  
  /* Send START condition */
  I2C_GenerateSTART(IIC_I2Cx, ENABLE);
  //*((u8 *)0x4001080c) &=~0x80;
  
  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((I2CTimeout--) == 0) return 0;
  }
  
  /* Send address for write */
  I2C_Send7bitAddress(IIC_I2Cx, Slave_Address, I2C_Direction_Transmitter);

  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((I2CTimeout--) == 0) return 0;
  }
    
  /* Clear EV6 by setting again the PE bit */
  I2C_Cmd(IIC_I2Cx, ENABLE);

  /* Send internal address to write to */
  I2C_SendData(IIC_I2Cx, REG_Address);  
   
  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if((I2CTimeout--) == 0) return 0;
  }
    
  /* Send STRAT condition a second time */  
  I2C_GenerateSTART(IIC_I2Cx, ENABLE);
  
  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((I2CTimeout--) == 0) return 0;
  }
    
  /* Send address for read */
  I2C_Send7bitAddress(IIC_I2Cx, Slave_Address+1, I2C_Direction_Receiver);
  
  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
  {
    if((I2CTimeout--) == 0) return 0;
   }
  
	 /* Disable Acknowledgement */
	I2C_AcknowledgeConfig(IIC_I2Cx, DISABLE);
	 
	/* Test on EV7 and clear it */    
	I2CTimeout = I2CT_LONG_TIMEOUT;
	while(I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)==0)  
	{
		if((I2CTimeout--) == 0) return 0;
	}     
	
	/* Read a byte */
	REG_data = I2C_ReceiveData(IIC_I2Cx);
	
	/* Send STOP Condition */
	I2C_GenerateSTOP(IIC_I2Cx, ENABLE);

  /* Enable Acknowledgement to be ready for another reception */
  I2C_AcknowledgeConfig(IIC_I2Cx, ENABLE);
	#endif
	
  return REG_data;
}

void IIC_Multiple_Read(unsigned char Slave_Address, unsigned char REG_Address, unsigned short size ,unsigned char *readBuf)
{
	// ���Ĵ�������
	#ifdef SOFTWARE_IIC  //���I2C
	IIC_Start();
	IIC_Send_Byte(Slave_Address);
	IIC_Wait_Ack();
	IIC_Send_Byte(REG_Address);
	IIC_Wait_Ack();
	IIC_Start();
	IIC_Send_Byte(Slave_Address+1);
	IIC_Wait_Ack();
	
	for(int i = 0;i < 6;i++)
	{
		if(i == 5)	
			readBuf[i] = IIC_Read_Byte(0);
		else
			readBuf[i] = IIC_Read_Byte(1);	
	}
	IIC_Stop();
	#else  //Ӳ��I2C
	I2CTimeout = I2CT_LONG_TIMEOUT;

	//*((u8 *)0x4001080c) |=0x80; 
	while(I2C_GetFlagStatus(IIC_I2Cx, I2C_FLAG_BUSY))
	{
		if((I2CTimeout--) == 0) return;
	}

	/* Send START condition */
	I2C_GenerateSTART(IIC_I2Cx, ENABLE);
	//*((u8 *)0x4001080c) &=~0x80;

	I2CTimeout = I2CT_FLAG_TIMEOUT;
	/* Test on EV5 and clear it */
	while(!I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
	{
		if((I2CTimeout--) == 0) return;
	}

	/* Send address for write */
	I2C_Send7bitAddress(IIC_I2Cx, Slave_Address, I2C_Direction_Transmitter);

	I2CTimeout = I2CT_FLAG_TIMEOUT;
	/* Test on EV6 and clear it */
	while(!I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
		if((I2CTimeout--) == 0) return;
	}

	/* Clear EV6 by setting again the PE bit */
	I2C_Cmd(IIC_I2Cx, ENABLE);

	/* Send internal address to write to */
	I2C_SendData(IIC_I2Cx, REG_Address);  

	I2CTimeout = I2CT_FLAG_TIMEOUT;
	/* Test on EV8 and clear it */
	while(!I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
		if((I2CTimeout--) == 0) return;
	}

	/* Send STRAT condition a second time */  
	I2C_GenerateSTART(IIC_I2Cx, ENABLE);

	I2CTimeout = I2CT_FLAG_TIMEOUT;
	/* Test on EV5 and clear it */
	while(!I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
	{
		if((I2CTimeout--) == 0) return;
	}

	/* Send address for read */
	I2C_Send7bitAddress(IIC_I2Cx, Slave_Address+1, I2C_Direction_Receiver);

	I2CTimeout = I2CT_FLAG_TIMEOUT;
	/* Test on EV6 and clear it */
	while(!I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
	{
		if((I2CTimeout--) == 0) return;
	}

	/* While there is data to be read */
	for(int i = 0;i < size;i++)
	{
		if(i == 5)
		{
			/* Disable Acknowledgement */
			I2C_AcknowledgeConfig(IIC_I2Cx, DISABLE);
		}

		/* Test on EV7 and clear it */    
		I2CTimeout = I2CT_LONG_TIMEOUT;

		while(I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)==0)  
		{
			if((I2CTimeout--) == 0) return;
		}      
		/* Read a byte */
		readBuf[i] = I2C_ReceiveData(IIC_I2Cx);  
	}

	/* Send STOP Condition */
	I2C_GenerateSTOP(IIC_I2Cx, ENABLE);
	
	/* Enable Acknowledgement to be ready for another reception */
	I2C_AcknowledgeConfig(IIC_I2Cx, ENABLE);
	#endif
}

void IIC_Read_DMA(unsigned char Slave_Address, unsigned char REG_Address, unsigned char *data, uint16_t size)
{
	#if 0
	// ���Ĵ�������
	#ifdef SOFTWARE_IIC  //���I2C
	IIC_Start();
	/* ����DMA���ջ�������С */
  DMA1_Stream0->NDTR = RECVBUFF_SIZE;
	IIC_Send_Byte(Slave_Address);
	IIC_Wait_Ack();
	IIC_Send_Byte(REG_Address);
	IIC_Wait_Ack();
	IIC_Start();
	IIC_Send_Byte(Slave_Address+1);
	IIC_Wait_Ack();
	
	/* ���������ɱ�־ */
	I2C1_RxComplete = 0;
	
	/* ʹ��DMA���� */
	DMA_Cmd(DMA1_Stream0, ENABLE);
		
	IIC_Stop();
	#else  //Ӳ��I2C
	I2CTimeout = I2CT_LONG_TIMEOUT;
  
  //*((u8 *)0x4001080c) |=0x80; 
  while(I2C_GetFlagStatus(IIC_I2Cx, I2C_FLAG_BUSY))
  {
    if((I2CTimeout--) == 0) return;
  }

  /* Send START condition */
  I2C_GenerateSTART(IIC_I2Cx, ENABLE);
  //*((u8 *)0x4001080c) &=~0x80;
  
  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((I2CTimeout--) == 0) return;
  }
  
  /* Send address for write */
  I2C_Send7bitAddress(IIC_I2Cx, Slave_Address, I2C_Direction_Transmitter);

  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((I2CTimeout--) == 0) return;
  }
    
  /* Clear EV6 by setting again the PE bit */
  I2C_Cmd(IIC_I2Cx, ENABLE);

  /* Send internal address to write to */
  I2C_SendData(IIC_I2Cx, REG_Address);  
   
  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if((I2CTimeout--) == 0) return;
  }
    
  /* Send STRAT condition a second time */  
  I2C_GenerateSTART(IIC_I2Cx, ENABLE);
  
  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((I2CTimeout--) == 0) return;
  }
    
  /* Send address for read */
  I2C_Send7bitAddress(IIC_I2Cx, Slave_Address+1, I2C_Direction_Receiver);
  
  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
  {
    if((I2CTimeout--) == 0) return;
  }
	
	/* ���������ɱ�־ */
	I2C1_RxComplete = 0;
	
	/* ʹ��DMA���� */
	DMA_Cmd(DMA1_Stream0, ENABLE);

	/* Send STOP Condition */
//	I2C_GenerateSTOP(IIC_I2Cx, ENABLE);
	
	/* Enable Acknowledgement to be ready for another reception */
	//I2C_AcknowledgeConfig(IIC_I2Cx, ENABLE);
	
	#endif
	
	#endif
	
// /* ����豸����״̬ */
//	uint8_t status;
//	status = I2C1_CheckConnection(Slave_Address);
//	if(status != 0)
//	{
//			/* �������Ӵ��� */
//			//while(1)
//				printf("I2C Read Connection ERROR\r\n");
//				return;
//	}

	/* ������ȡ10���Ĵ��������� */

	I2C1_DMA_ReceiveData(Slave_Address, REG_Address, data, size);
}

void IIC_Write_DMA(unsigned char Slave_Address, unsigned char REG_Address, unsigned char *data, uint16_t size)
{
 /* ����豸����״̬ */
	uint8_t status;
 /* ����豸����״̬ */
	status = I2C1_CheckConnection(Slave_Address);
	if(status != 0)
	{
			/* �������Ӵ��� */
			//while(1)
			{
				printf("I2C Write Connection ERROR %d\r\n",status);
				return;
				
			}
	}
	
	/* �������� */
	status = I2C1_DMA_SendData(Slave_Address, &REG_Address, 1);
	if(status != 0)
	{
			/* �����ʹ��� */
			//while(1)
			{
				printf("I2C DMA Write EEROR  %d\r\n",status);
				return;
			}
	}
	
	/* �������� */
	status = I2C1_DMA_SendData(Slave_Address, data, 1);
	if(status != 0)
	{
			/* �����ʹ��� */
			//while(1)
			{
				printf("I2C DMA Write EEROR  %d\r\n",status);
				return;
			}
	}
}

/*------------------- HMC5883 ��������ʼ������ ----------------------*/
/*
�Ĵ������ý�����
- ���üĴ���A��0x00��: 
  (3<<5)=0b1100000 �� 8�β���ƽ���������ֲὨ��ֵ��
  (1<<4)=0b010000 �� 15Hz�������
  0 �� ��������ģʽ�����Լ죩
- ���üĴ���B��0x01��: 
  (1<<5)=0b100000 �� ��1.3Ga���̣�Ĭ��������1090 LSB/Gauss��
- ģʽ�Ĵ�����0x02��: 
  0x00 �� ��������ģʽ
*/
void Init_HMC5883()
{
	unsigned char HMC_InitBuf[] = {0x70, 0xE0, 0x00};
	Delay_us(100);
	IIC_Single_Write(HMC_SlaveAddress, HMC5883_CONFIG_A, 1, &HMC_InitBuf[0]); // ���üĴ���A
	IIC_Single_Write(HMC_SlaveAddress, HMC5883_CONFIG_B, 1, &HMC_InitBuf[1]);       // ���üĴ���B
	IIC_Single_Write(HMC_SlaveAddress, HMC5883_MODE, 1, &HMC_InitBuf[2]);           // ģʽ�Ĵ���
	
	
//	IIC_Write_DMA(HMC_SlaveAddress, HMC5883_CONFIG_A, &HMC_InitBuf[0], 2); // ���üĴ���A
//	IIC_Write_DMA(HMC_SlaveAddress, HMC5883_CONFIG_B, &HMC_InitBuf[1], 2);       // ���üĴ���B
//	IIC_Write_DMA(HMC_SlaveAddress, HMC5883_MODE, &HMC_InitBuf[2], 2);           // ģʽ�Ĵ���
	
	// ��ʼ���������˲���
	float q = 0.1;  // ��������Э����
	float r = 0;    // ��������Э����
	float initial_x = 0;  // ��ʼ״̬����ֵ
	float initial_p = 0;  // ��ʼ�������Э����
}

/*------------------- �Լ�ģʽʵ�� -------------------------*/
/*
�Լ����̣�
1. ����Ϊ�ڲ�������ģʽ��������֪�ų���
2. �л�Ϊ���β���ģʽ
3. ��ȡ����ֵ��������ΧX/Y��951��Z��886��
4. �ָ���������ģʽ
*/
unsigned char Init_HMC5883_SelfTestMode() 
{
	unsigned char HMC_SelfTestBuf[] = {0x70|0x01, 0x01, 0x70|0x00, 0x00};
	
	IIC_Single_Write(HMC_SlaveAddress, HMC5883_CONFIG_A, 1, &HMC_SelfTestBuf[0]); // ���üĴ���A��0b01110001��
	Delay_ms(10);
	IIC_Single_Write(HMC_SlaveAddress, HMC5883_MODE, 1, &HMC_SelfTestBuf[1]);     // ���β���ģʽ                     
	Delay_ms(2000); // �ȴ������ȶ�
	IIC_Multiple_Read(HMC_SlaveAddress, HMC5883_DATA_X_H, 6, HMC_BUF);              // ��ȡ�Լ�����

	// �ָ�����ģʽ
	IIC_Single_Write(HMC_SlaveAddress, HMC5883_CONFIG_A, 1, &HMC_SelfTestBuf[2]); 
	IIC_Single_Write(HMC_SlaveAddress, HMC5883_MODE, 1, &HMC_SelfTestBuf[3]);
	return 0;
}

/*------------------- ��ԲУ׼�㷨 ------------------------*/
/*
У׼���裺
1. 360����ת�豸�ɼ�X/Y��ֵ
2. �������ƫ�ƣ�offset = (max + min)/2
3. ����������ӣ�scale = avg_radius / axis_radius
*/
void CalibrateHMC5883() 
{
	short x_min = 0x7FFF, x_max = 0x8000;
	short y_min = 0x7FFF, y_max = 0x8000;
	uint8_t cal_flag = 0;
	float HMC_CalFactor[5] = {0};  //��[0]����ʾ�Ƿ�д��У׼ϵ��
	
	if(FlashID == sFLASH_ID)
	{
		/*��ȡ����*/
		SPI_FLASH_BufferRead((void*)HMC_CalFactor, SPI_FLASH_PageSize*0, sizeof(HMC_CalFactor));
		
		if(HMC_CalFactor[0] == 0xBB)  /*����־����0xcd����ʾ֮ǰ����д������*/
		{
			x_offset = HMC_CalFactor[1];
			y_offset = HMC_CalFactor[3];
			x_scale = HMC_CalFactor[2];
			y_scale = HMC_CalFactor[4];
			SPI_Flash_PowerDown(); 
			printf("�ѱ�У׼��\n");
			return;
		}

		printf("�뻺��360����ת\n");
		for (int i = 0; i < 120; i++)
		{
			Delay_ms(100);
			IIC_Multiple_Read(HMC_SlaveAddress, HMC5883_DATA_X_H, 6, HMC_BUF);
//			IIC_Read_DMA(HMC_SlaveAddress, HMC5883_DATA_X_H, HMC_BUF, 6); 	// ��ȡԭʼ����
			
			short x = (short)(HMC_BUF[0]  << 8) | HMC_BUF[1];
			short y = (short)(HMC_BUF[4]  << 8) | HMC_BUF[5];
			
			printf("x --> %d\r\n",x);
			printf("y --> %d\r\n",y);
			
			// ���¼�ֵ
			x_min = (x < x_min) ? x : x_min;
			x_max = (x > x_max) ? x : x_max;
			y_min = (y < y_min) ? y : y_min;
			y_max = (y > y_max) ? y : y_max;
		}

		// ����У׼����
		x_offset = (x_max + x_min) / 2;
		y_offset = (y_max + y_min) / 2;
		float avg_radius = ((x_max - x_min) + (y_max - y_min)) / 4.0;
		x_scale = avg_radius / (x_max - x_min);
		y_scale = avg_radius / (y_max - y_min);

		HMC_CalFactor[0] = 0xBB;  //0xCD�Ƿ�д��У׼ϵ��
		HMC_CalFactor[1] = x_offset*1.0;
		HMC_CalFactor[2] = x_scale;
		HMC_CalFactor[3] = y_offset*1.0;
		HMC_CalFactor[4] = y_scale;
		
		printf("У׼��ɣ�\n");
		
		/* ��У׼ϵ��д��Flash */
		//��������
		SPI_FLASH_SectorErase(0);
		SPI_FLASH_BufferWrite((void*)HMC_CalFactor, SPI_FLASH_PageSize*0, sizeof(HMC_CalFactor));
		SPI_Flash_PowerDown(); 
	}
}

/*------------------- �����ݲɼ����� ----------------------*/
void ReadData_HMC5883(float *x_raw, float *y_raw, float *z_raw)
{
	IIC_Multiple_Read(HMC_SlaveAddress, HMC5883_DATA_X_H, 6, HMC_BUF); // ��ȡԭʼ����
	// ԭʼ���ݽ�����ע��X/Y�Ĵ���˳��
	*x_raw = (HMC_BUF[0]  << 8) | HMC_BUF[1];		//��ȡ����X������
	//printf("%f\t",*x_raw);
	*y_raw = (HMC_BUF[4]  << 8) | HMC_BUF[5];		//��ȡ����X������
	//printf("%f\t",*y_raw);
	*z_raw = (HMC_BUF[2]  << 8) | HMC_BUF[3];		//��ȡ����X������
	//printf("%f\r\n",*z_raw);

	//�������
	//printf("x = %d y = %d\r\n", x, y);
}

void ReadData_HMC5883_DMA(float *x_raw, float *y_raw, float *z_raw)
{
	IIC_Read_DMA(HMC_SlaveAddress, HMC5883_DATA_X_H, RecvBuff, 6); // ��ȡԭʼ����
	// ԭʼ���ݽ�����ע��X/Y�Ĵ���˳��
	*x_raw = (RecvBuff[0]  << 8) | RecvBuff[1];		//��ȡ����X������
	//printf("%f\t",*x_raw);
	*y_raw = (RecvBuff[4]  << 8) | RecvBuff[5];		//��ȡ����X������
	//printf("%f\t",*y_raw);
	*z_raw = (RecvBuff[2]  << 8) | RecvBuff[3];		//��ȡ����X������
	//printf("%f\r\n",*z_raw);

	//�������
	//printf("x = %d y = %d\r\n", x, y);
}

/*------------------- ��λ�Ǽ��� --------------------------*/
// ��-180��~180��ת��Ϊ0��~360�㣬��ӳ�䵽8��������
double AngleToDirection(short x_raw, short y_raw, short z_raw) 
{
	// Ӧ��У׼����
	short x = (x_raw - x_offset) * x_scale;
	short y = (y_raw - y_offset) * y_scale;

	// ��λ�Ǽ��㣨ע��atan2����˳��Ϊy,x��
	double angle = atan2((double)y, (double)x) * (180.0 / 3.14159265358979);
	if (angle < 0) angle += 360;         // ת��Ϊ0-360��
	
//	const float sector = 45.0;            // ÿ������45�㷶Χ
//	const char* directions[] = {"��", "����", "��", "����","��", "����", "��", "����"};
//	int index = (int)((angle + sector/2) / sector) % 8;								 
//	return directions[index];
	
	return angle;
}


//��ʼ��MPU3050��������Ҫ��ο�pdf�����޸�
static void Init_MPU3050(void)
{
	unsigned char MPU_InitBuf[] = {0x80, 0x03, 0x07, 0x1E};
	IIC_Single_Write(MPU_SlaveAddress, MPU3050_RA_PWR_M, 1, &MPU_InitBuf[0]);	     //�������״̬  ��λ
	Delay_us(100);   //�ȴ���λ�ȶ�
	IIC_Single_Write(MPU_SlaveAddress, MPU3050_RA_PWR_M, 1, &MPU_InitBuf[1]);	     // ����ʱ��ԴΪPLL_Z
	IIC_Single_Write(MPU_SlaveAddress, MPU3050_RA_SMPL, 1, &MPU_InitBuf[2]);    //�����ǲ�����
	IIC_Single_Write(MPU_SlaveAddress, MPU3050_RA_DLPF, 1, &MPU_InitBuf[3]);    // ���õ�ͨ�˲��������̣�42Hz������2000��/s��
	
//	IIC_Write_DMA(MPU_SlaveAddress, MPU3050_RA_PWR_M, &MPU_InitBuf[0], 2);	     //�������״̬  ��λ
//	Delay_us(100);   //�ȴ���λ�ȶ�
//	IIC_Write_DMA(MPU_SlaveAddress, MPU3050_RA_PWR_M, &MPU_InitBuf[1], 2);	     // ����ʱ��ԴΪPLL_Z
//	IIC_Write_DMA(MPU_SlaveAddress, MPU3050_RA_SMPL, &MPU_InitBuf[2], 2);    //�����ǲ�����
//	IIC_Write_DMA(MPU_SlaveAddress, MPU3050_RA_DLPF, &MPU_InitBuf[3], 2);    // ���õ�ͨ�˲��������̣�42Hz������2000��/s��
}
        
//******��ȡMPU3050����  I2C��ʽ****************************************
void ReadData_MPU3050(float *x_raw, float *y_raw, float *z_raw)
{
	IIC_Multiple_Read(MPU_SlaveAddress, MPU3050_RA_GX_H, 6, MPU_BUF);
	*x_raw = (MPU_BUF[0]<<8)|MPU_BUF[1];		//��ȡ����X������
	//printf("%f\t",*x_raw);
	*y_raw = (MPU_BUF[2]<<8)|MPU_BUF[3]; //��ȡ����Y������
	//printf("%f\t",*y_raw);
	*z_raw = (MPU_BUF[4]<<8)|MPU_BUF[5];   //��ȡ����Z������
	//printf("%f\t\r\n",*z_raw);	

	//�������
	//printf("������x = %d ������y = %d ������z = %d\r\n", T_X, T_Y, T_Z);
}

//******��ȡMPU3050����  DMA��ʽ****************************************
void ReadData_MPU3050_DMA(float *x_raw, float *y_raw, float *z_raw)
{
	IIC_Read_DMA(MPU_SlaveAddress, MPU3050_RA_GX_H, RecvBuff, 6);
	short temp = 0;
	*x_raw = (RecvBuff[0]<<8)|RecvBuff[1];		//��ȡ����X������
	//printf("%f\t",*x_raw);
	*y_raw = (RecvBuff[2]<<8)|RecvBuff[3]; //��ȡ����Y������
	//printf("%f\t",*y_raw);
	*z_raw = (RecvBuff[4]<<8)|RecvBuff[5];   //��ȡ����Z������
	//printf("%f\t\r\n",*z_raw);	
	//�������
	//printf("������x = %d ������y = %d ������z = %d\r\n", T_X, T_Y, T_Z);
}


/* ADXL345 */
static void Init_ADXL345(void) 
{
	unsigned char ADXL_InitBuf[] = {0X0B, 0x0A, 0x08, 0x00, 0x00, 0x00, 0xF0};
	Delay_ms(2);  //�ϵ�ȴ�
	IIC_Single_Write(ADXL_SlaveAddress,ADXL345_POWER_CTL,1,&ADXL_InitBuf[2]);		//����ģʽ
	IIC_Single_Write(ADXL_SlaveAddress,ADXL345_DATA_FORMAT,1,&ADXL_InitBuf[0]);		//13λȫ�ֱ���,16g���� 
	IIC_Single_Write(ADXL_SlaveAddress,ADXL345_BW_RATE,1,&ADXL_InitBuf[1]);		//��������ٶ�Ϊ100Hz
	IIC_Single_Write(ADXL_SlaveAddress,ADXL345_INT_ENABLE,1,&ADXL_InitBuf[3]);		//��ʹ���ж�		 
	IIC_Single_Write(ADXL_SlaveAddress,ADXL345_OFSX,1,&ADXL_InitBuf[4]);
	IIC_Single_Write(ADXL_SlaveAddress,ADXL345_OFSY,1,&ADXL_InitBuf[5]);
	IIC_Single_Write(ADXL_SlaveAddress,ADXL345_OFSZ,1,&ADXL_InitBuf[6]);
	
//	IIC_Write_DMA(ADXL_SlaveAddress,ADXL345_POWER_CTL,&ADXL_InitBuf[2],2);		//����ģʽ
//	IIC_Write_DMA(ADXL_SlaveAddress,ADXL345_DATA_FORMAT,&ADXL_InitBuf[0],2);		//13λȫ�ֱ���,16g���� 
//	IIC_Write_DMA(ADXL_SlaveAddress,ADXL345_BW_RATE,&ADXL_InitBuf[1],2);		//��������ٶ�Ϊ100Hz
//	IIC_Write_DMA(ADXL_SlaveAddress,ADXL345_INT_ENABLE,&ADXL_InitBuf[3],2);		//��ʹ���ж�		 
//	IIC_Write_DMA(ADXL_SlaveAddress,ADXL345_OFSX,&ADXL_InitBuf[4],2);
//	IIC_Write_DMA(ADXL_SlaveAddress,ADXL345_OFSY,&ADXL_InitBuf[5],2);
//	IIC_Write_DMA(ADXL_SlaveAddress,ADXL345_OFSZ,&ADXL_InitBuf[6],2);
}


void ReadData_ADXL345(float *x_raw, float *y_raw, float *z_raw)
{
	IIC_Multiple_Read(ADXL_SlaveAddress, ADXL345_DATAX0, 6, ADXL_BUF);
	
	*x_raw = ADXL_BUF[1] << 8 | ADXL_BUF[0];
	//printf("%f\t",*x_raw);
	
	*y_raw = ADXL_BUF[3] << 8 | ADXL_BUF[2];
	//printf("%f\t",*y_raw);
	
	*z_raw = ADXL_BUF[5] << 8 | ADXL_BUF[4];
	//printf("%f\r\n",*z_raw);

	// �������
	//printf("���ٶȼ�x = %d ���ٶȼ�y = %d ���ٶȼ�z = %d\r\n", x, y,z);
}

void ReadData_ADXL345_DMA(float *x_raw, float *y_raw, float *z_raw)
{
	IIC_Read_DMA(ADXL_SlaveAddress, ADXL345_DATAX0, RecvBuff, 6);
	
	*x_raw = RecvBuff[1] << 8 | RecvBuff[0];
	//printf("%f\t",*x_raw);
	*y_raw = RecvBuff[3] << 8 | RecvBuff[2];
	//printf("%f\t",*y_raw);
	*z_raw = RecvBuff[5] << 8 | RecvBuff[4];
	//printf("%f\r\n",*z_raw);

	// �������
	//printf("���ٶȼ�x = %d ���ٶȼ�y = %d ���ٶȼ�z = %d\r\n", x, y,z);
}

