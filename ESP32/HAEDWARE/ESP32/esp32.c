#include "hip600d.h"
#include <stdlib.h>
#include <string.h>

//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 HIP_USART_RX_BUF[HIP_USART_REC_LEN] = {0};     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
u8 HIP_USART_RX_STA_LINE = 0;    //һ�����ݱ�־	
u8 HIP_USART_RX_STA_FRAM = 0;   //һ֡���ݱ�־
u16 HIP_USART_RX_STA_COUNT = 0;

void hip600d_USART_GPIO_Init(uint32_t bound)
{
	 //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(HIP_USART_RCC_GPIO,ENABLE); //ʹ��GPIOBʱ��
	RCC_APB1PeriphClockCmd(HIP_USART_RCC,ENABLE);//ʹ��USART3ʱ��
 
	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(HIP_USART_RX_GPIO, HIP_RX_PinSourcex, HIP_RX_AF); //GPIOB10����ΪUSART3
	GPIO_PinAFConfig(HIP_USART_TX_GPIO, HIP_TX_PinSourcex, HIP_TX_AF); //GPIOB11����ΪUSART3
	
	//USART1�˿�����
  GPIO_InitStructure.GPIO_Pin = HIP_USART_RX_PIN; //GPIOB11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(HIP_USART_RX_GPIO,&GPIO_InitStructure); //��ʼ��PB11
	
	GPIO_InitStructure.GPIO_Pin = HIP_USART_TX_PIN; //GPIOB10
	GPIO_Init(HIP_USART_TX_GPIO,&GPIO_InitStructure); //��ʼ��PB10

   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(HIP_USARTx, &USART_InitStructure); //��ʼ������3
	
  USART_Cmd(HIP_USARTx, ENABLE);  //ʹ�ܴ���3 
		
	USART_ITConfig(HIP_USARTx, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart1 NVIC ����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
  NVIC_InitStructure.NVIC_IRQChannel = HIP_USART_IRQn;//����3�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����
}

//������������
void parseNavigationData(u8 *dataStr) 
{
	u8 dataSize = 0;
	char *endptr;
	u8 splitStrList[20][20] = {0};

	GNRMCStruct rmc;
	
	u8 targetStr[50] = {0};
		
	int res = myFindString(dataStr,(u8 *)"$GNRMC", targetStr);
	if(res == 0)
	{
		//printf("%s\n",targetStr);
		mySplitStr(',', targetStr, splitStrList, &dataSize);
		memcpy(rmc.UTC, splitStrList[1],10); 
	
		rmc.Status = *splitStrList[2];
		
		rmc.Latitude = strtof((char *)splitStrList[3], &endptr); 
		
		rmc.LatitudeDir = *splitStrList[4];
		
		rmc.Longitude = strtof((char *)splitStrList[5], &endptr);
		
		rmc.LongitudeDir = *splitStrList[6];
		
		rmc.SpeedOverGround = strtof((char *)splitStrList[7], &endptr);
		
		rmc.CourseOverGround = strtof((char *)splitStrList[8], &endptr);
		
		memcpy(rmc.Date, splitStrList[9],6); 
		
		rmc.MagneticVariation = strtof((char *)splitStrList[10], &endptr);
		
		rmc.MagneticVariationDir = *splitStrList[11];
		
		rmc.ModeIndicator = *splitStrList[12];
		
		rmc.NavigationalStatus = *splitStrList[13];
		
		printf("UTCʱ�䣺%c%c:%c%c:%c%c::%c%c\n",rmc.UTC[0],rmc.UTC[1],rmc.UTC[2],rmc.UTC[3],rmc.UTC[4],rmc.UTC[5],rmc.UTC[7],rmc.UTC[8]);
		printf("��λ״̬��%d\n",rmc.Status);
		printf("γ�ȣ�%f\n",rmc.Latitude);
		printf("γ�ȷ���%d\n",rmc.LatitudeDir);
		printf("���ȣ�%f\n",rmc.Longitude);
		printf("���ȷ���%d\n",rmc.LongitudeDir);
		
		printf("�����ٶȣ�%f\n",rmc.SpeedOverGround);
		printf("���溽��%f\n",rmc.CourseOverGround);
		printf("���ڣ�%c%c.%c%c.%c%c\n",rmc.Date[0],rmc.Date[1],rmc.Date[2],rmc.Date[3],rmc.Date[4],rmc.Date[5]);
		
		printf("��ƫ�ǣ�%f\n",rmc.MagneticVariation);
		printf("��ƫ�Ƿ���%d\n",rmc.MagneticVariationDir);
		
		printf("ģʽ��%c\n",rmc.ModeIndicator);
		printf("����״̬��%c\n",rmc.NavigationalStatus);
	}
}



void mySplitStr(u8 symbol, u8 *dataStr, u8 (*splitStrList)[20], u8 *splitCount)
{
	u8 pos = 0,count = 0;
	u8 *temp = dataStr;
	while(*temp != 0x0A)   //���з�
	{
		if(*temp != symbol)
		{
			splitStrList[count][pos] = *temp;
			pos++;
		}
		else 
		{
			pos = 0;
			count++;
		}
		temp++;
	}
	
	*splitCount = count+1;
}

int myFindString(u8 *dataStr, u8 *subStr, u8 *targetStr)
{
	char *result = strstr((char *)dataStr, (char *)subStr);
	
	if(result)
	{
		int i = 0;
		while(*result != 0x0A)
		{
			targetStr[i] = *result;
			i++;
			result++;
		}
		targetStr[i] = *result;
		return 0;
	}
		
	return 1;
}


