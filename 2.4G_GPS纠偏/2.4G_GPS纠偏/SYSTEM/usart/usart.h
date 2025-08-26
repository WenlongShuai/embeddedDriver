#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/8/18
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持
//#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
 
#define RX1_DMA_CH 			DMA1_Channel5
#define TX1_DMA_CH			DMA1_Channel4

#define UART_RX1_SIZE		1400
#define UART_TX1_SIZE		1400
#define rxBuff_SIZE     1500

// \r 回车 值：0x0d   \n 换行 值：0x0a
#define rtcm_Head  "rtcm_Head"
#define rtcm_End   "rtcm_End\r\n"  
#define rtcm_Head_len  (sizeof(rtcm_Head)-1)
#define rtcm_End_len  (sizeof(rtcm_End)-1)

extern uint8_t Buff_USART1_RX[];//接收缓存器
extern uint8_t Buff_USART1_TX[];//发送缓冲器
extern uint8_t rxBuff[];

extern int  rx1Data_CNT; //DMA_RX1数据包总个数

extern uint8_t Flag_DMA_RX1;   //RX1_DMA帧数据接收完成标志位
extern uint8_t Flag_DMA_TX1;   //RX1_DMA帧数据发送完成标志位


//extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	
//如果想串口中断接收，请不要注释以下宏定义
void uart_init(u32 bound);

//配置串口接收DMA DMA1_CH5
void DMA_RX1Init(void);
void DMA_RX1(DMA_Channel_TypeDef *DMA_Streamx,u16 ndtr); 

//配置串口接收DMA DMA1_CH4
void DMA_TX1Init(void);
void DMA_TX1(DMA_Channel_TypeDef *DMA_Streamx,u16 ndtr);

char* custom_str_find(const char* array, const char* str, size_t length);

#endif


