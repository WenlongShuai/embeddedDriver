#include "sys.h"
#include "usart2.h"
//C��
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

uint8_t USART2_RxBuff[usart2_rxsize]={0};
//uint8_t USART2_TxBuff[usart2_txsize]={0};
//uint8_t usart2_buff[usart2_rxsize]={0};//������

uint8_t flag_Rx2DMA=0;
uint8_t flag_Tx2DMA=0;

void USART2_Init(uint32_t baud)
{

	GPIO_InitTypeDef  GPIO_usart2_InitStructure;
	USART_InitTypeDef USART2_InitStruc;
	NVIC_InitTypeDef NVIC_InitStructure;
	//USART_DeInit(USART2);  //��λ����2
	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //ʹ�ܶ˿�ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	GPIO_usart2_InitStructure.GPIO_Pin = GPIO_Pin_2;	 //usart2:TX
 	GPIO_usart2_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //�������
	GPIO_usart2_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//�ٶ�50MHz
 	GPIO_Init(GPIOA, &GPIO_usart2_InitStructure);	  
	
	GPIO_usart2_InitStructure.GPIO_Pin = GPIO_Pin_3;	//usart2:RX
 	GPIO_usart2_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		 
	GPIO_usart2_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//�ٶ�50MHz
 	GPIO_Init(GPIOA, &GPIO_usart2_InitStructure);	  //��ʼ��GPIOD3,6
	
	//���ڳ�ʼ������
	USART2_InitStruc.USART_BaudRate=baud;
	USART2_InitStruc.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART2_InitStruc.USART_StopBits=USART_StopBits_1;
	USART2_InitStruc.USART_WordLength=USART_WordLength_8b;
	USART2_InitStruc.USART_Parity=USART_Parity_No;  //У��λ
	USART2_InitStruc.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_Init(USART2,&USART2_InitStruc);
	
	//Usart2 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
		
	
//		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
		USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
//	USART2->CR1 |= USART_IT_RXNE;
//	USART2->CR1 |= USART_IT_IDLE;
	USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���2 
}

//���ô��ڽ���DMA
void USART2_RX_DMA(uint32_t baud)
{
	USART2_Init(baud);//���ڳ�ʼ��
	
	NVIC_InitTypeDef NVIC_DMA_InitSttuc;
	DMA_InitTypeDef DMA_USART2_InitStruc;//DMA��ʼ���ṹ�壨DMA_USART2_RX��
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 ,ENABLE);//DMA1ʱ�ӿ���
	DMA_Cmd(DMA1_Channel6,DISABLE);
	DMA_DeInit(DMA1_Channel6);//USART2_RX DMAӳ��DMA1_Channel6��ʼ��
	
	NVIC_DMA_InitSttuc.NVIC_IRQChannel=DMA1_Channel6_IRQn;
	NVIC_DMA_InitSttuc.NVIC_IRQChannelCmd=ENABLE;
	NVIC_DMA_InitSttuc.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_DMA_InitSttuc.NVIC_IRQChannelSubPriority=2;
	NVIC_Init(&NVIC_DMA_InitSttuc);
	
	DMA_USART2_InitStruc.DMA_DIR= DMA_DIR_PeripheralSRC;//���䷽�����ã�������Ϊ���ݴ���Ŀ�ĵ�or��Դ��������Դ��ַ��	
	DMA_USART2_InitStruc.DMA_BufferSize= usart2_rxsize;//ָ��DMAͨ�������С
	DMA_USART2_InitStruc.DMA_PeripheralBaseAddr= (uint32_t)&USART2->DR;//�������ַ
	DMA_USART2_InitStruc.DMA_PeripheralInc= DMA_PeripheralInc_Disable;//�趨�����ַ�Ĵ����������
	DMA_USART2_InitStruc.DMA_PeripheralDataSize= DMA_PeripheralDataSize_Byte;
	
	DMA_USART2_InitStruc.DMA_MemoryBaseAddr= (uint32_t)&USART2_RxBuff;//���ݴ�Ż���ַ
	DMA_USART2_InitStruc.DMA_MemoryInc= DMA_MemoryInc_Enable;//�趨�ڴ��ַ�Ĵ����������
	DMA_USART2_InitStruc.DMA_MemoryDataSize= DMA_MemoryDataSize_Byte;	
	DMA_USART2_InitStruc.DMA_Mode= DMA_Mode_Normal;
	DMA_USART2_InitStruc.DMA_Priority= DMA_Priority_Medium;//DMAͨ�����ȼ�
	DMA_USART2_InitStruc.DMA_M2M= DMA_M2M_Disable;//ʹ�� DMA ͨ�����ڴ浽�ڴ洫��
	DMA_Init(DMA1_Channel6,&DMA_USART2_InitStruc);
	
//	DMA_ITConfig(DMA1_Channel6,DMA1_IT_TC6,ENABLE);
	
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);//USART2_RX DMA����ʹ��
	DMA_ClearFlag(DMA1_FLAG_GL6);//���DMA1_Channel6ȫ�ֱ�־λ
	DMA_Cmd(DMA1_Channel6,ENABLE);
}

//����һ��DMA����
void MYDMA_Enable(DMA_Channel_TypeDef *DMA_Streamx,u16 ndtr)     //ʹ��DMA
{
 
    DMA_Cmd(DMA_Streamx, DISABLE);                      //�ر�DMA���� 
    
    //while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}    //ȷ��DMA���Ա�����  
        
    DMA_SetCurrDataCounter(DMA_Streamx,ndtr);          //���ݴ�����  
	
	//DMA_ClearFlag(DMA1_FLAG_GL6);//���DMA1_Channel6ȫ�ֱ�־λ
    DMA_Cmd(DMA_Streamx, ENABLE);                      //����DMA���� 
//	while (DMA_GetFlagStatus(DMA1_FLAG_TC6) == RESET);
//	DMA_ClearFlag(DMA1_FLAG_TC6);
}      

//����һ���ֽ�
void Usart_SendByte(USART_TypeDef * pUSARTx,uint8_t date)
{
	 USART_SendData(pUSARTx,date);
	 while( USART_GetFlagStatus(pUSARTx,USART_FLAG_TXE)== RESET);
}

//����һ��16λ������
void Usart_SendHalfWord(USART_TypeDef * pUSARTx,uint16_t date)
{
	uint16_t tmp_h;
	uint16_t tmp_l;
	tmp_h =date>>0x08;
	tmp_l =date & 0xff;
	Usart_SendByte(pUSARTx,tmp_h);
	Usart_SendByte(pUSARTx,tmp_l);
}

//����һ��8λ������
void Usart_SendArray(USART_TypeDef * pUSARTx,uint8_t *arr,uint16_t num)
{ 
		pUSARTx->SR;//���־λ
		pUSARTx->DR;
   while(num--)
	 {
		 Usart_SendByte( pUSARTx ,*arr++);
	 }
 while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TC)== RESET);
}

//�����ַ���
//void Usart_SendString( USART_TypeDef * pUSARTx, char *str)
//{
// while( *str!='\0' )
// {
//	 Usart_SendByte( pUSARTx, *str++); 
// }

// while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TC)== RESET);
//}

void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len)
{

	unsigned short count = 0;
	USARTx->SR;//���־λ
	USARTx->DR;
	for(; count < len; count++)
	{
		USART_SendData(USARTx, *str++);									//��������
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);		//�ȴ��������
	}

}

//��ʽ����ӡ����
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...)
{

	unsigned char UsartPrintfBuf[296];
	va_list ap;
	unsigned char *pStr = UsartPrintfBuf;
	
	va_start(ap, fmt);
	vsnprintf((char *)UsartPrintfBuf, sizeof(UsartPrintfBuf), fmt, ap);							//��ʽ��
	va_end(ap);
	
	while(*pStr != 0)
	{
		USART_SendData(USARTx, *pStr++);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
	}

}




