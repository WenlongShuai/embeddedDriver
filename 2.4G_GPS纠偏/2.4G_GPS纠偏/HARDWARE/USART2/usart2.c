#include "sys.h"
#include "usart2.h"
//C库
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

uint8_t USART2_RxBuff[usart2_rxsize]={0};
//uint8_t USART2_TxBuff[usart2_txsize]={0};
//uint8_t usart2_buff[usart2_rxsize]={0};//缓存器

uint8_t flag_Rx2DMA=0;
uint8_t flag_Tx2DMA=0;

void USART2_Init(uint32_t baud)
{

	GPIO_InitTypeDef  GPIO_usart2_InitStructure;
	USART_InitTypeDef USART2_InitStruc;
	NVIC_InitTypeDef NVIC_InitStructure;
	//USART_DeInit(USART2);  //复位串口2
	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能端口时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	GPIO_usart2_InitStructure.GPIO_Pin = GPIO_Pin_2;	 //usart2:TX
 	GPIO_usart2_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //推挽输出
	GPIO_usart2_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOA, &GPIO_usart2_InitStructure);	  
	
	GPIO_usart2_InitStructure.GPIO_Pin = GPIO_Pin_3;	//usart2:RX
 	GPIO_usart2_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		 
	GPIO_usart2_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOA, &GPIO_usart2_InitStructure);	  //初始化GPIOD3,6
	
	//串口初始化配置
	USART2_InitStruc.USART_BaudRate=baud;
	USART2_InitStruc.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART2_InitStruc.USART_StopBits=USART_StopBits_1;
	USART2_InitStruc.USART_WordLength=USART_WordLength_8b;
	USART2_InitStruc.USART_Parity=USART_Parity_No;  //校验位
	USART2_InitStruc.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_Init(USART2,&USART2_InitStruc);
	
	//Usart2 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
		
	
//		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接收中断
		USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
//	USART2->CR1 |= USART_IT_RXNE;
//	USART2->CR1 |= USART_IT_IDLE;
	USART_Cmd(USART2, ENABLE);                    //使能串口2 
}

//配置串口接收DMA
void USART2_RX_DMA(uint32_t baud)
{
	USART2_Init(baud);//串口初始化
	
	NVIC_InitTypeDef NVIC_DMA_InitSttuc;
	DMA_InitTypeDef DMA_USART2_InitStruc;//DMA初始化结构体（DMA_USART2_RX）
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 ,ENABLE);//DMA1时钟开启
	DMA_Cmd(DMA1_Channel6,DISABLE);
	DMA_DeInit(DMA1_Channel6);//USART2_RX DMA映像，DMA1_Channel6初始化
	
	NVIC_DMA_InitSttuc.NVIC_IRQChannel=DMA1_Channel6_IRQn;
	NVIC_DMA_InitSttuc.NVIC_IRQChannelCmd=ENABLE;
	NVIC_DMA_InitSttuc.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_DMA_InitSttuc.NVIC_IRQChannelSubPriority=2;
	NVIC_Init(&NVIC_DMA_InitSttuc);
	
	DMA_USART2_InitStruc.DMA_DIR= DMA_DIR_PeripheralSRC;//传输方向配置，外设作为数据传输目的地or来源（外设做源地址）	
	DMA_USART2_InitStruc.DMA_BufferSize= usart2_rxsize;//指定DMA通道缓存大小
	DMA_USART2_InitStruc.DMA_PeripheralBaseAddr= (uint32_t)&USART2->DR;//外设基地址
	DMA_USART2_InitStruc.DMA_PeripheralInc= DMA_PeripheralInc_Disable;//设定外设地址寄存器递增与否
	DMA_USART2_InitStruc.DMA_PeripheralDataSize= DMA_PeripheralDataSize_Byte;
	
	DMA_USART2_InitStruc.DMA_MemoryBaseAddr= (uint32_t)&USART2_RxBuff;//数据存放基地址
	DMA_USART2_InitStruc.DMA_MemoryInc= DMA_MemoryInc_Enable;//设定内存地址寄存器递增与否
	DMA_USART2_InitStruc.DMA_MemoryDataSize= DMA_MemoryDataSize_Byte;	
	DMA_USART2_InitStruc.DMA_Mode= DMA_Mode_Normal;
	DMA_USART2_InitStruc.DMA_Priority= DMA_Priority_Medium;//DMA通道优先级
	DMA_USART2_InitStruc.DMA_M2M= DMA_M2M_Disable;//使能 DMA 通道的内存到内存传输
	DMA_Init(DMA1_Channel6,&DMA_USART2_InitStruc);
	
//	DMA_ITConfig(DMA1_Channel6,DMA1_IT_TC6,ENABLE);
	
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);//USART2_RX DMA请求使能
	DMA_ClearFlag(DMA1_FLAG_GL6);//清除DMA1_Channel6全局标志位
	DMA_Cmd(DMA1_Channel6,ENABLE);
}

//开启一次DMA传输
void MYDMA_Enable(DMA_Channel_TypeDef *DMA_Streamx,u16 ndtr)     //使能DMA
{
 
    DMA_Cmd(DMA_Streamx, DISABLE);                      //关闭DMA传输 
    
    //while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}    //确保DMA可以被设置  
        
    DMA_SetCurrDataCounter(DMA_Streamx,ndtr);          //数据传输量  
	
	//DMA_ClearFlag(DMA1_FLAG_GL6);//清除DMA1_Channel6全局标志位
    DMA_Cmd(DMA_Streamx, ENABLE);                      //开启DMA传输 
//	while (DMA_GetFlagStatus(DMA1_FLAG_TC6) == RESET);
//	DMA_ClearFlag(DMA1_FLAG_TC6);
}      

//发送一个字节
void Usart_SendByte(USART_TypeDef * pUSARTx,uint8_t date)
{
	 USART_SendData(pUSARTx,date);
	 while( USART_GetFlagStatus(pUSARTx,USART_FLAG_TXE)== RESET);
}

//发送一个16位的数据
void Usart_SendHalfWord(USART_TypeDef * pUSARTx,uint16_t date)
{
	uint16_t tmp_h;
	uint16_t tmp_l;
	tmp_h =date>>0x08;
	tmp_l =date & 0xff;
	Usart_SendByte(pUSARTx,tmp_h);
	Usart_SendByte(pUSARTx,tmp_l);
}

//发送一个8位的数组
void Usart_SendArray(USART_TypeDef * pUSARTx,uint8_t *arr,uint16_t num)
{ 
		pUSARTx->SR;//清标志位
		pUSARTx->DR;
   while(num--)
	 {
		 Usart_SendByte( pUSARTx ,*arr++);
	 }
 while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TC)== RESET);
}

//发送字符串
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
	USARTx->SR;//清标志位
	USARTx->DR;
	for(; count < len; count++)
	{
		USART_SendData(USARTx, *str++);									//发送数据
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);		//等待发送完成
	}

}

//格式化打印函数
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...)
{

	unsigned char UsartPrintfBuf[296];
	va_list ap;
	unsigned char *pStr = UsartPrintfBuf;
	
	va_start(ap, fmt);
	vsnprintf((char *)UsartPrintfBuf, sizeof(UsartPrintfBuf), fmt, ap);							//格式化
	va_end(ap);
	
	while(*pStr != 0)
	{
		USART_SendData(USARTx, *pStr++);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
	}

}




