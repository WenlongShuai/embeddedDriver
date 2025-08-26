#ifndef __USART2_H
#define __USART2_H

#include "sys.h"

#define USART_DEBUG		USART1		//调试打印所使用的串口组

#define usart2_rxsize   1600
//#define usart2_txsize   1

extern uint8_t flag_Rx2DMA;
extern uint8_t flag_Tx2DMA;

extern  uint8_t USART2_RxBuff[usart2_rxsize];
//extern  uint8_t USART2_TxBuff[usart2_txsize];
//extern  uint8_t usart2_buff[usart2_rxsize];//串口2缓存器，定义为和Rx缓存器大小


void Usart_SendByte(USART_TypeDef * pUSARTx,uint8_t date);
void Usart_SendHalfWord(USART_TypeDef * pUSARTx,uint16_t date);
void Usart_SendArray(USART_TypeDef * pUSARTx,uint8_t *arr,uint16_t num);
//void Usart_SendString( USART_TypeDef * pUSARTx, char *str);
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len);
void USART2_Init(uint32_t baud);
void USART2_RX_DMA(uint32_t baud);
void MYDMA_Enable(DMA_Channel_TypeDef *DMA_Streamx,u16 ndtr) ;
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...);














#endif
