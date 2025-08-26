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
	//USART_DeInit(USART3);  //复位串口3
	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能端口时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	GPIO_usart3_InitStructure.GPIO_Pin = GPIO_Pin_10;	 //usart3:TX
 	GPIO_usart3_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //推挽输出
	GPIO_usart3_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOA, &GPIO_usart3_InitStructure);	  
	
	GPIO_usart3_InitStructure.GPIO_Pin = GPIO_Pin_11;	//usart3:RX
 	GPIO_usart3_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		 
	GPIO_usart3_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOB, &GPIO_usart3_InitStructure);	  //初始化GPIOD3,6
	
	//串口初始化配置
	USART3_InitStruc.USART_BaudRate=boud;
	USART3_InitStruc.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART3_InitStruc.USART_StopBits=USART_StopBits_1;
	USART3_InitStruc.USART_WordLength=USART_WordLength_8b;
	USART3_InitStruc.USART_Parity=USART_Parity_No;  //校验位
	USART3_InitStruc.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_Init(USART3,&USART3_InitStruc);
	
		//Usart3 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化NVIC寄存器
	
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接收中断
	
}




