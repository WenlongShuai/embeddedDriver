#include "key.h"

uint8_t keyStateFlag = 0;
uint16_t time = 0;  //��ʱ����ʱʱ�䣬�����ͷźͰ���ʱ��Ļ�ȡ
uint8_t key_Process_Time = 0;  //״̬������ʱ��,10ms����һ��״̬��
Key_t key0;

// �ж����ȼ�����
static void BASIC_TIM_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 	
		// �����ж���Դ
    NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;	
		// ���������ȼ�Ϊ 0
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;	 
	  // ������ռ���ȼ�Ϊ3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*
 * ע�⣺TIM_TimeBaseInitTypeDef�ṹ��������5����Ա��TIM6��TIM7�ļĴ�������ֻ��
 * TIM_Prescaler��TIM_Period������ʹ��TIM6��TIM7��ʱ��ֻ���ʼ����������Ա���ɣ�
 * ����������Ա��ͨ�ö�ʱ���͸߼���ʱ������.
 *-----------------------------------------------------------------------------
 *typedef struct
 *{ TIM_Prescaler            ����
 *	TIM_CounterMode			     TIMx,x[6,7]û�У���������
 *  TIM_Period               ����
 *  TIM_ClockDivision        TIMx,x[6,7]û�У���������
 *  TIM_RepetitionCounter    TIMx,x[1,8,15,16,17]����
 *}TIM_TimeBaseInitTypeDef; 
 *-----------------------------------------------------------------------------
 */

static void BASIC_TIM_Mode_Config(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		
		// ������ʱ��ʱ��,���ڲ�ʱ��CK_INT=168M
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	
		// �Զ���װ�ؼĴ�����ֵ���ۼ�TIM_Period+1��Ƶ�ʺ����һ�����»����ж�
    TIM_TimeBaseStructure.TIM_Period = (1000-1);	

	  // ʱ��Ԥ��Ƶ��Ϊ
    TIM_TimeBaseStructure.TIM_Prescaler= 84-1;  //1ms
	
		// ʱ�ӷ�Ƶ���� ��������ʱ��û�У����ù�
    //TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
		
		// ����������ģʽ��������ʱ��ֻ�����ϼ�����û�м���ģʽ������
    //TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
		
		// �ظ���������ֵ��������ʱ��û�У����ù�
		//TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
	
	  // ��ʼ����ʱ��
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
		
		// ����������жϱ�־λ
    TIM_ClearFlag(TIM6, TIM_FLAG_Update);
	  
		// �����������ж�
    TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);
		
		// ʹ�ܼ�����
    TIM_Cmd(TIM6, ENABLE);	
}

static void BASIC_TIM_Init(void)
{
	BASIC_TIM_Mode_Config();
	BASIC_TIM_NVIC_Config();
}

/**************************************************************************
�������ܣ�������ʼ��
��ڲ�������
����  ֵ���� 
**************************************************************************/
void KEY_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);                      //ʹ�� PORTE ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);	
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;  //��������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_3; 			//PE4--K0   PE3--K1
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
  // ��GPIO���ӵ�EXTI��·��STM32F4ʹ��SYSCFG���ã�
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource4);
	
	// ����EXTI
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // �½��ش���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	// ����NVIC
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	// ��ʼ�������ṹ��
	key0.state = KEY_STATE_IDLE;
	key0.event = KEY_EVENT_NONE;
	key0.pressTime = 0;
	key0.releaseTime = 0;
	key0.pressed = 0;
	
	// ��ʼ����ʱ��
	BASIC_TIM_Init();
} 

//��ȡ������ƽ���͵�ƽ��ʾ���£�
static uint8_t Key_Read_K0(void) 
{
	return (GPIO_ReadInputDataBit(KEY0_GPIO_PORT, KEY0_GPIO_PIN) == 0);
}

//��ȡ������ƽ���͵�ƽ��ʾ���£�
static uint8_t Key_Read_K1(void) 
{
	return (GPIO_ReadInputDataBit(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == 0);
}

// ��ȡϵͳʱ�䣨���룩
static uint32_t GetSystemTimeMs(void) 
{
	if(time >=0xFFFF)
		time = 0;
	return time; // ʾ��ʵ�֣�ʵ��Ӧʹ�ö�ʱ��
}


// ����״̬���������ڶ�ʱ�ж��е��ã�����10ms����һ�Σ�
void Key_Process_K0(void) 
{
    uint32_t currentTime = GetSystemTimeMs();
    uint8_t keyLevel = Key_Read_K0();
    
    // ��ⰴ��״̬�仯
    if (keyLevel != key0.pressed) {
        // ����������ʱ20msȷ��
        if (GetSystemTimeMs() - key0.releaseTime > 20) {
            key0.pressed = keyLevel;
            if (key0.pressed) {
                key0.pressTime = currentTime;
            } else {
                key0.releaseTime = currentTime;
            }
        }
    }
    
    // ״̬������
    switch (key0.state) {
        case KEY_STATE_IDLE:
            if (key0.pressed) {
                key0.state = KEY_STATE_PRESSED;
            }
            key0.event = KEY_EVENT_NONE;
            break;
            
        case KEY_STATE_PRESSED:
            if (!key0.pressed) {
                // �̰��ͷ�
                key0.state = KEY_STATE_SHORT_RELEASED;
            } else if (currentTime - key0.pressTime > 2000) {
                // ����������2S��
                key0.state = KEY_STATE_LONG_PRESS;
                key0.event = KEY_EVENT_LONG_PRESS;
            }
            break;
            
        case KEY_STATE_SHORT_RELEASED:
            key0.state = KEY_STATE_WAIT_DOUBLE;
            break;
            
        case KEY_STATE_WAIT_DOUBLE:
            if (key0.pressed) {
                // ��⵽�ڶ��ΰ���
                key0.state = KEY_STATE_DOUBLE_PRESSED;
            } else if (currentTime - key0.releaseTime > 500) {
                // ��ʱδ��⵽˫��
                key0.state = KEY_STATE_IDLE;
                key0.event = KEY_EVENT_CLICK;
            }
            break;
						
				case KEY_STATE_DOUBLE_PRESSED:
					if (!key0.pressed) {
                // �ڶ��ΰ����ͷţ�ȷ��Ϊ˫��
                key0.state = KEY_STATE_IDLE;
                key0.event = KEY_EVENT_DOUBLE_CLICK;
            }
            break;
            
        case KEY_STATE_LONG_PRESS:
            if (!key0.pressed) {
                key0.state = KEY_STATE_IDLE;
            }
            break;
    }
}

void TIM6_DAC_IRQHandler()
{
	if(TIM_GetFlagStatus(TIM6,TIM_FLAG_Update)==SET)//5ms��ʱ�ж�
	{   
		TIM_ClearITPendingBit(TIM6,TIM_IT_Update);                             //===�����ʱ��1�жϱ�־λ	  
		time++;
		key_Process_Time++;
	}
}

// �жϴ�����
void EXTI4_IRQHandler(void) 
{
	if (EXTI_GetITStatus(EXTI_Line4) != RESET) {
		// ���������������ʱ��
    for (uint32_t i = 0; i < 40000; i++); // ����ʱ����
		if(keyStateFlag == 0)
			keyStateFlag = 1;
		// ����жϱ�־λ
		EXTI_ClearITPendingBit(EXTI_Line4);
	}
}
