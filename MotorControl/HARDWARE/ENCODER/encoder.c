#include "encoder.h"

/*TIM2��ʼ��Ϊ�������ӿ�*/
void Encoder_Init_TIM(u16 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure; //����һ�����ų�ʼ���Ľṹ��  
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;//����һ����ʱ����ʼ���Ľṹ��
  TIM_ICInitTypeDef TIM_ICInitStructure; //����һ����ʱ��������ģʽ��ʼ���Ľṹ��
	
	RCC_APB1PeriphClockCmd(ENCODER_TIM_RCC, ENABLE); //ʹ��TIM5ʱ��
	RCC_AHB1PeriphClockCmd(ENCODER_TIM_GPIO_A_RCC | ENCODER_TIM_GPIO_B_RCC, ENABLE); //ʹ��CPIOAʱ��
 
	GPIO_PinAFConfig(ENCODER_PORT_A, GPIO_PinSource5, GPIO_AF_TIM2);
	GPIO_PinAFConfig(ENCODER_PORT_B, GPIO_PinSource3, GPIO_AF_TIM2);
	
	GPIO_InitStructure.GPIO_Pin = ENCODER_PIN_A;	//PA0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(ENCODER_PORT_A, &GPIO_InitStructure);	//����GPIO_InitStructure�Ĳ�����ʼ��GPIOA0
	
	GPIO_InitStructure.GPIO_Pin = ENCODER_PIN_B;	//PA1
	GPIO_Init(ENCODER_PORT_B, &GPIO_InitStructure);	//����GPIO_InitStructure�Ĳ�����ʼ��GPIOA0

	TIM_TimeBaseStructure.TIM_Period = arr; //�趨�������Զ���װֵ
	TIM_TimeBaseStructure.TIM_Prescaler = psc; // Ԥ��Ƶ�� 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //ѡ��ʱ�ӷ�Ƶ������Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(ENCODER_TIMx, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct�Ĳ�����ʼ����ʱ��TIM5
	
	TIM_EncoderInterfaceConfig(ENCODER_TIMx, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//ʹ�ñ�����ģʽ3��CH1��CH2ͬʱ�������ķ�Ƶ
	TIM_ICStructInit(&TIM_ICInitStructure); //��TIM_ICInitStruct �е�ÿһ��������ȱʡֵ����
	TIM_ICInitStructure.TIM_ICFilter = 10;  //�����˲�������
	TIM_ICInit(ENCODER_TIMx, &TIM_ICInitStructure); //��TIM_ICInitStructure������ʼ����ʱ��TIM5������ģʽ

	TIM_Cmd(ENCODER_TIMx, ENABLE); //ʹ�ܶ�ʱ��5
}

//��ȡ����������
int Read_Encoder_TIM(void)
{
	int Encoder_TIM;
	Encoder_TIM = ENCODER_TIMx->CNT; //��ȡ����
	//printf("Encoder_TIM = %d\r\n", Encoder_TIM); 
	if(Encoder_TIM > 0x10D0)Encoder_TIM = Encoder_TIM - 0x20D0; //ת������ֵΪ�з����ֵ������0��ת��С��0��ת��
	                                                     //TIM4->CNT��ΧΪ0-0xffff����ֵΪ0��
	ENCODER_TIMx->CNT = 0; //��ȡ����������
	return Encoder_TIM; //����ֵ
}


/**************************************************************************
�������ܣ���λʱ���ȡ����������
��ڲ�������ʱ��
����  ֵ���ٶ�ֵ
**************************************************************************/
int Read_Velocity(void)
{
	int Encoder_TIM;    

	Encoder_TIM = (short)ENCODER_TIMx->CNT;
	ENCODER_TIMx->CNT=0;
	
	return Encoder_TIM;
}
/**************************************************************************
�������ܣ���ȡλ����Ϣ
��ڲ�������ʱ��
����  ֵ��λ��ֵ
**************************************************************************/
int Read_Position(void)
{
	int Encoder_TIM;    
	Encoder_TIM = (short)ENCODER_TIMx->CNT;
	return Encoder_TIM;
}
