#include "usart3.h"
#include "sys.h"
#include <stdio.h>
#include <string.h>

uint8_t USART3_RxBuff[usart3_rxsize]={0};
uint8_t USART3_TxBuff[usart3_txsize]={0};


void USART3_Init(uint32_t boud)
{
	GPIO_InitTypeDef  GPIO_usart3_InitStructure;
	USART_InitTypeDef USART3_InitStruc;
	NVIC_InitTypeDef NVIC_InitStructure;
	//USART_DeInit(USART3);  //��λ����3
	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //ʹ�ܶ˿�ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	GPIO_usart3_InitStructure.GPIO_Pin = GPIO_Pin_10;	 //usart3:TX
 	GPIO_usart3_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //�������
	GPIO_usart3_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//�ٶ�50MHz
 	GPIO_Init(GPIOA, &GPIO_usart3_InitStructure);	  
	
	GPIO_usart3_InitStructure.GPIO_Pin = GPIO_Pin_11;	//usart3:RX
 	GPIO_usart3_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		 
	GPIO_usart3_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//�ٶ�50MHz
 	GPIO_Init(GPIOB, &GPIO_usart3_InitStructure);	  //��ʼ��GPIOD3,6
	
	//���ڳ�ʼ������
	USART3_InitStruc.USART_BaudRate=boud;
	USART3_InitStruc.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART3_InitStruc.USART_StopBits=USART_StopBits_1;
	USART3_InitStruc.USART_WordLength=USART_WordLength_8b;
	USART3_InitStruc.USART_Parity=USART_Parity_No;  //У��λ
	USART3_InitStruc.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_Init(USART3,&USART3_InitStruc);
	
		//Usart3 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��NVIC�Ĵ���
	
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
	
}




