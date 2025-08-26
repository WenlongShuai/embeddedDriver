/**
  ******************************************************************************
  * @file    GPIO/IOToggle/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h" 
#include "sys.h"

uint8_t Flag_100ms=0;
uint8_t Flag_500ms=0;



void NMI_Handler(void)
{
}
 
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}
 
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

 
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}
 
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}
 
void SVC_Handler(void)
{
}
 
void DebugMon_Handler(void)
{
}
 
void PendSV_Handler(void)
{
}
 
void SysTick_Handler(void)
{
}



void TIM4_IRQHandler(void)
{
	
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
    {

    }
}

void EXTI0_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        // �����ж��¼�
       

    }
}

void TIM3_IRQHandler(void)   //TIM3�ж� 100us
{

}


void USART1_IRQHandler(void)
{
	static char flag_rxData =0;
  int 			  rx1DMA_CNT   =0;	  //��¼���ν��յ������ݸ���
	if(USART_GetITStatus( USART1, USART_IT_IDLE ) != RESET )
	{		
		//����жϱ�־λ
		USART1->DR;
		USART1->SR;		
		
		rx1DMA_CNT = UART_RX1_SIZE - RX1_DMA_CH->CNDTR ; //��ȡ����DMA���յ����ݸ���
//		Buff_USART1_RX[rx1DMA_CNT]='\0';
		/* �ж��Ƿ�����ͷ ��  �ϰ������Ѵ����� */
		if( custom_str_find((char*)Buff_USART1_RX, rtcm_Head ,rx1DMA_CNT) &&  Flag_DMA_RX1==0 )
		{
			if(flag_rxData )  												  //��־λδ��� (δ�յ�β�ٴ��յ�ͷ�����ݲ�����)
			{ 
				rx1Data_CNT = 0;     										  //�������(���ݰ��ܸ���)
				flag_rxData=0;       										  //�����־λ		
			}	
			
			rx1Data_CNT = rx1DMA_CNT;  						   		//�����ݸ�����¼
			memcpy(rxBuff, Buff_USART1_RX, rx1DMA_CNT);	//�����������ͷ������
			flag_rxData = 1;   													//�ѽ�������ͷ��־λ����λ��ʼ���ձ�������ֱ���ٴν��յ�����ͷ��β

		}
		/*  */
		else if(flag_rxData)
		{
			
			if((rx1Data_CNT+rx1DMA_CNT) > rxBuff_SIZE)																//����Խ��
			{
				rx1Data_CNT = 0;
				flag_rxData = 0;
			}
			else
			{
				memcpy(rxBuff + rx1Data_CNT, Buff_USART1_RX, rx1DMA_CNT);	  //��������
				/* ��β֡���� */
				if(custom_str_find( (char*)Buff_USART1_RX , rtcm_End , rx1DMA_CNT) )
				{
					flag_rxData  = 0;  //���ݽ�����ɱ�־λ���
					Flag_DMA_RX1 = 1;	 //�������ݽ�����ɱ�־λ
				}	
				rx1Data_CNT += rx1DMA_CNT;  										            //�����ݸ�����¼
			}	
		}
		DMA_RX1(DMA1_Channel5,UART_RX1_SIZE);		 //���¸�ֵDMA����
	}

}
//DMA_TX1 ��������ж�
void DMA1_Channel4_IRQHandler(void)
{
	if(DMA1->ISR & DMA1_IT_TC4)//DMA ������ɱ�־λ
	{
		
		DMA_ClearFlag(DMA1_FLAG_TC4);
		
		Flag_DMA_TX1=0;//������ϣ������־λ
	}
}


void USART2_IRQHandler(void)
{
	 if (USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
		{
				//����м���жϱ�־λ �ȶ�USART_SR��Ȼ���USART_DR
				USART2->SR;
				USART2->DR;
				/* �ϰ�����δ������ */
				if(flag_Rx2DMA)
						MYDMA_Enable(DMA1_Channel6,usart2_rxsize);//������һ�ε�DMA����
				else 
						flag_Rx2DMA=1;	
		}
}

//void DMA1_Channel1_IRQHandler(void)
//{

//	if(DMA_GetITStatus(DMA1_IT_TC1)!=RESET)
//	{

//	}
//}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
