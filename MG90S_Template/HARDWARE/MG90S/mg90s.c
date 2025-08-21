#include "mg90s.h"
#include "delay.h"

static void TIM_MG90S_Init(u32 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//ʹ��TIM3ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //ʹ��GPIOAʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//��ʼ����ʱ��2 
	TIM_TimeBaseStructure.TIM_Period = arr; //�趨�������Զ���װֵ 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 	//Ԥ��Ƶ��   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
  
	//��ʼ��TIM3����Ƚ�PWM������
	TIM_OCInitStructure.TIM_OCIdleState = 0;  //������TIM1��TIM8
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCNIdleState = 0;  //������TIM1��TIM8
	TIM_OCInitStructure.TIM_OCNPolarity = 0;  //������TIM1��TIM8
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputNState = 0;	//������TIM1��TIM8
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0x00;
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);	
	
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_CtrlPWMOutputs(TIM3, ENABLE);
	
  TIM_Cmd(TIM3, ENABLE ); 	//ʹ�ܶ�ʱ��3
}


void MG90S_Init(void)
{
	TIM_MG90S_Init(20000-1, 72-1); //20ms
}

/*
PWM�ź���180�ȶ���Ĺ�ϵ��
0.5ms----------------0�ȣ�
1ms -----------------45�ȣ�
1.5ms----------------90�ȣ�
2ms -----------------135�ȣ�
2.5ms ---------------180�ȣ�
PWM�ź���360���ת�ٵĹ�ϵ��
0.5ms----------------�������ת�٣�
1.5ms----------------�ٶ�Ϊ0��
2.5ms----------------�������ת�٣�
*/
void MG90S_SetAngle(float Angle)
{
	//������ת���Ƕ�Ϊ0ʱ�����ö�ʱ����Ԥ��װ��ֵΪ500����Ϊ��ʼ��ʱ��ʱ����ARR���óɵ�20000��20ms = 20000us,����0.5ms=500us
//	TIM_SetCompare1(TIM3, Angle/180*2000+500);  
	TIM_SetCompare1(TIM3, 1000);
//	delay_ms(50);
//	TIM_SetCompare1(TIM3, 1500);
}


