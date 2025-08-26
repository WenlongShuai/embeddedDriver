#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/8/18
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
//#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
 
#define RX1_DMA_CH 			DMA1_Channel5
#define TX1_DMA_CH			DMA1_Channel4

#define UART_RX1_SIZE		1400
#define UART_TX1_SIZE		1400
#define rxBuff_SIZE     1500

// \r �س� ֵ��0x0d   \n ���� ֵ��0x0a
#define rtcm_Head  "rtcm_Head"
#define rtcm_End   "rtcm_End\r\n"  
#define rtcm_Head_len  (sizeof(rtcm_Head)-1)
#define rtcm_End_len  (sizeof(rtcm_End)-1)

extern uint8_t Buff_USART1_RX[];//���ջ�����
extern uint8_t Buff_USART1_TX[];//���ͻ�����
extern uint8_t rxBuff[];

extern int  rx1Data_CNT; //DMA_RX1���ݰ��ܸ���

extern uint8_t Flag_DMA_RX1;   //RX1_DMA֡���ݽ�����ɱ�־λ
extern uint8_t Flag_DMA_TX1;   //RX1_DMA֡���ݷ�����ɱ�־λ


//extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	
//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart_init(u32 bound);

//���ô��ڽ���DMA DMA1_CH5
void DMA_RX1Init(void);
void DMA_RX1(DMA_Channel_TypeDef *DMA_Streamx,u16 ndtr); 

//���ô��ڽ���DMA DMA1_CH4
void DMA_TX1Init(void);
void DMA_TX1(DMA_Channel_TypeDef *DMA_Streamx,u16 ndtr);

char* custom_str_find(const char* array, const char* str, size_t length);

#endif


