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

	if(USART_GetITStatus( USART1, USART_IT_IDLE ) != RESET )
	{		
		//����жϱ�־λ
		USART1->DR;
		USART1->SR;
		
		//ǰ����Ϣδ������ɣ���Ϣ����
		if(Flag_DMA_RX1)
		{
			DMA_RX1(DMA1_Channel5,UART_RX1_SIZE);//���¸�ֵDMA����
		}
		else
			Flag_DMA_RX1=1;
			
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
