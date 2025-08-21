#include "timer.h"
//////////////////////////////////////////////////////////////////////////////////

uint8_t flag = 0;				//���ڼ�¼�ж��ź��������ػ����½���
uint32_t number = 0;			//��¼��ʱ���жϵĴ���
uint32_t times = 0;			//��¼�����źŵĳ���ʱ��
float distance = 0;    //�����ľ���

//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIMx_Init(u32 arr,u32 psc)
{		
	/* ����GPIO */ 	
	//��ʼ��GPIO��,Trigʹ���������,Echoʹ�ø�������
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(GPIO_RCC_GPIO_PORT,ENABLE);        //ʹ��GPIOA������ʱ��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;                        //ѡ�񸡿�����ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_GPIO_PIN;                                    //ѡ��GPIO_Pin_1
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIO_GPIO_PORT,&GPIO_InitStructure);	

	/* ���ö�ʱ�� */
	RCC_APB1PeriphClockCmd(TIM_RCC_PERIPH,ENABLE);
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;                        //ʹ��ʱ�ӷ�Ƶ1
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;                    //���ϼ���
	//84MHz/84/100=10000,ÿ�붨ʱ������10000��,���ÿ������Ϊ100us
	TIM_TimeBaseInitStructure.TIM_Period = arr;                                       //�Զ���װ��
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;                                     //Ԥ��Ƶϵ��
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;                               //�ò��������߼���ʱ��ʹ��
	TIM_TimeBaseInit(TIMx,&TIM_TimeBaseInitStructure);
	TIM_ITConfig(TIMx,TIM_IT_Update,ENABLE);                    //ʹ���ж�����ź�
	TIM_ClearITPendingBit(TIMx,TIM_IT_Update);

	//NVIC���䶨ʱ�����ж����ȼ�
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIMx_IRQ;                             //ָ��Tim2���ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TIM_NVIC_PREEMPTIONPRIORITY;                   //��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = TIM_NVIC_SUBPRIORITY;                          //��Ӧ���ȼ�
	NVIC_Init(&NVIC_InitStructure);

	/* �����ⲿ�ж� */
	//AFIOӳ���ж�����
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOx, EXTI_PinSourcex);

	//EXTI�ж�����
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Linex;                                  //echoʹ�õĶ˿�1,���ѡ��1���ж���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;              //�����غ��½��ض������ж�
	EXTI_Init(&EXTI_InitStructure);

	//NVIC�����ⲿ�жϵ��ж����ȼ�
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);             //ָ���жϷ���
	NVIC_InitTypeDef NVIC_InitStructure2;
	NVIC_InitStructure2.NVIC_IRQChannel = EXTIx_IRQ;                          //ʹ�õĶ˿�1,���ѡ���������
	NVIC_InitStructure2.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure2.NVIC_IRQChannelPreemptionPriority = EXTI_NVIC_PREEMPTIONPRIORITY;                   //��ռ���ȼ�
	NVIC_InitStructure2.NVIC_IRQChannelSubPriority = EXTI_NVIC_SUBPRIORITY;                          //��Ӧ���ȼ� 
	NVIC_Init(&NVIC_InitStructure2);
}

