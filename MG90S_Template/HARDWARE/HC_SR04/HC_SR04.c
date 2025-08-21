#include "HC_SR04.h"

uint8_t flag = 0;				//���ڼ�¼�ж��ź��������ػ����½���
uint32_t number = 0;			//��¼��ʱ���жϵĴ���
uint32_t times = 0;			//��¼�����źŵĳ���ʱ��
float distance = 0;    //�����ľ���

//����״̬
//[7]:0,û�гɹ��Ĳ���;1,�ɹ�����һ��.
//[6]:0,��û���񵽵͵�ƽ;1,�Ѿ����񵽵͵�ƽ��.
//[5:0]:����͵�ƽ������Ĵ���(����32λ��ʱ����˵,1us��������1,���ʱ��:4294��)
u8  TIMxCHx_CAPTURE_STA = 0;	//���벶��״̬		    				
u16	TIMxCHx_CAPTURE_VAL;	//���벶��ֵ(TIM2/TIM5��32λ)

static void Hcsr04_TRIG_GPIO_Init()
{
	RCC_APB2PeriphClockCmd(RCC_TRIG_PERIPH,ENABLE);        //ʹ��GPIOA������ʱ��
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//ѡ���������ģʽ
	GPIO_InitStructure.GPIO_Pin = TRIG_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TRIG_PORT,&GPIO_InitStructure);
	GPIO_ResetBits(TRIG_PORT,TRIG_PIN);
}

//��ʱ��2ͨ��1���벶������
//arr���Զ���װֵ(TIM2,TIM5��32λ��!!)
//psc��ʱ��Ԥ��Ƶ��
static void TIMx_CHx_Cap_Init(u32 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
 
	RCC_APB1PeriphClockCmd(TIM_RCC_PERIPH, ENABLE);	//ʹ��TIM2ʱ��
 	RCC_APB2PeriphClockCmd(GPIO_RCC_GPIO_PORT, ENABLE);  //ʹ��GPIOAʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_GPIO_PORT, &GPIO_InitStructure);
	GPIO_ResetBits(GPIO_GPIO_PORT,GPIO_GPIO_PIN);						 //����
	
	//��ʼ����ʱ��2 
	TIM_TimeBaseStructure.TIM_Period = arr; //�趨�������Զ���װֵ 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 	//Ԥ��Ƶ��   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM_x, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
  
	//��ʼ��TIM5���벶�����
	TIM_ICInitStructure.TIM_Channel = TIMx_CHANNEL_x; //	ѡ������� IC1ӳ�䵽TI2��
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI2��
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
	TIM_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 ���������˲��� ���˲�
	TIM_ICInit(TIM_x, &TIM_ICInitStructure);
	
	//�жϷ����ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = TIMx_IRQ;  //TIM2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //��ռ���ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //�����ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ��� 
	
	TIM_ITConfig(TIM_x,TIM_IT_Update|TIMx_IT_CCx,ENABLE);//��������ж� ,����CC2IE�����ж�	
	
  TIM_Cmd(TIM_x,ENABLE ); 	//ʹ�ܶ�ʱ��2
}


/**
 * @description: ������ģ������벶��ʱ��ͨ����ʼ��
 * @param {TIM_HandleTypeDef} *htim
 * @param {uint32_t} Channel
 * @return {*}
 */
void Hcsr04Init()
{
	TIMx_CHx_Cap_Init(0xFFFF,72-1);
	Hcsr04_TRIG_GPIO_Init();
}

/**
 * @description: HC-SR04����
 * @param {*}
 * @return {*}
 */
void Hcsr04Start()
{
	GPIO_SetBits(TRIG_PORT,TRIG_PIN);
	delay_us(10);//����˵����,��Ҫ�ṩ����10us�ĸߵ�ƽ
  GPIO_ResetBits(TRIG_PORT,TRIG_PIN);
}


/**
 * @description: ��ȡ���� 
 * @param {*}
 * @return {*}
 */
float Hcsr04Read()
{
  // ������޷�
  if(distance >= 400)
  {
    distance = 400;
  }
	else if(distance <= 2)
	{
		distance = 2;
	}
		
  return distance;
}


void TIMx_IRQHANDLER(void)
{ 
 	if((TIMxCHx_CAPTURE_STA&0X80)==0)//��δ�ɹ�����	
	{	  
		if (TIM_GetITStatus(TIM_x, TIM_IT_Update) != RESET)
		{	    
			if(TIMxCHx_CAPTURE_STA&0X40)//�Ѿ����񵽸ߵ�ƽ��
			{
				if((TIMxCHx_CAPTURE_STA&0X3F)==0X3F)//�ߵ�ƽ̫����
				{
					TIMxCHx_CAPTURE_STA|=0X80;//��ǳɹ�������һ��
					TIMxCHx_CAPTURE_VAL=0XFFFF;
				}else TIMxCHx_CAPTURE_STA++;
			}	 
		}
		if (TIM_GetITStatus(TIM_x, TIMx_IT_CCx) != RESET)//����1���������¼�
		{	
			if(TIMxCHx_CAPTURE_STA&0X40)		//����һ���½��� 		
			{	  			
				TIMxCHx_CAPTURE_STA|=0X80;		//��ǳɹ�����
				TIMxCHx_CAPTURE_VAL=TIM_GetCapturex(TIM_x);
		   	TIM_OCxPolarityConfig(TIM_x,TIM_ICPolarity_Rising); //CC1P=0 ����Ϊ�����ز���
			}
			else  								//��δ��ʼ,��һ�β���������
			{
				TIMxCHx_CAPTURE_STA=0;			//���
				TIMxCHx_CAPTURE_VAL=0;
	 			TIM_SetCounter(TIM_x,0);
				TIMxCHx_CAPTURE_STA|=0X40;		//��ǲ�����������
		   	TIM_OCxPolarityConfig(TIM_x,TIM_ICPolarity_Falling);		//CC1P=1 ����Ϊ�½��ز���
			}	
		} 
 	}
	TIM_ClearITPendingBit(TIM_x, TIMx_IT_CCx | TIM_IT_Update); //����жϱ�־λ
}
