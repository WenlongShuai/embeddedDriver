#include "timer.h"


extern int Moto;//���PWM���� Ӧ��Motor��
extern int Position,Target_Position; //���������������
extern int Encoder,Target_Velocity;

/**************************************************************************
�������ܣ���ʱ�жϳ�ʼ��
��ڲ�����arr���Զ���װֵ  psc��ʱ��Ԥ��Ƶ�� 
����  ֵ����
**************************************************************************/
void Timerx_Init(u16 arr,u16 psc)  
{  
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	
	TIM_TimeBaseInitStruct.TIM_Period = arr;               //��װ��ֵ
	TIM_TimeBaseInitStruct.TIM_Prescaler = psc;            //Ԥ��Ƶϵ��
	TIM_TimeBaseInitStruct.TIM_ClockDivision =0;           //ʱ�ӷָ�
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStruct);
	
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);               //ʹ�ܶ�ʱ���ж�
	
	NVIC_InitStruct.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;        //ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;           //ʹ���ⲿ�ж�ͨ��
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1; //��ռ���ȼ�1
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;        //��Ӧ���ȼ�3
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_Cmd(TIM1,ENABLE);
}  

/**************************************************************************
�������ܣ����еĿ��ƴ��붼��������
          TIM1���ƵĶ�ʱ�ж� 
**************************************************************************/
void TIM1_UP_TIM10_IRQHandler()
{
	if(TIM_GetFlagStatus(TIM1,TIM_FLAG_Update)==SET)//5ms��ʱ�ж�
	{   
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);                             //===�����ʱ��1�жϱ�־λ	  
		//Position = Read_Position();                      //===����λ����Ϣ
		Encoder = Read_Velocity();												//===�����ٶ���Ϣ
		Key();                                          //===��������Ŀ��ֵ
		//Moto = Position_PID(Position, Target_Position);    //===λ��PID������
		Moto = Incremental_PI(Encoder, Target_Velocity);    //===λ��PID������
		Xianfu_Pwm();                                  //===PWM�޷�
		Set_Pwm(Moto);
	}
}

