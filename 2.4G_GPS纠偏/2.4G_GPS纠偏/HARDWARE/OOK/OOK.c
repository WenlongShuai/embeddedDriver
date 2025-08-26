#include "ook.h"


#define PIN_RF ((GPIOA->IDR & GPIO_Pin_0))
//#define PIN_RF ((GPIOB->DI & GPIO_PIN_12) ^ GPIO_PIN_12)

static unsigned char rf_start = 0;
static unsigned short error_bit_large = 0;
//static unsigned short error_bit_small = 0;
static unsigned short rf_bit_cnt = 0;
static unsigned long Timer_cnt = 0;

static unsigned char b_cnt= 0;
static unsigned long rf_dat = 0;


unsigned char Flag_RF_Data=0;

//void OOK_EN(void)
//{
//	GPIOA->DO &= ~GPIO_PIN_8;
//	TIM6->CTR1 |= 0x01;
//}
//void OOK_DN(void)
//{
//	GPIOA->DO |= GPIO_PIN_8;
//	TIM6->CTR1 &= ~0x01;
//	TIM6->CNT = 0;
//}

_OOK_info ook_info;

void LED_GPIO_confing(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;// GPIO_Mode_IPU
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIOC->ODR |=  GPIO_Pin_13;
}

void OOK_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    // ʹ��GPIOAʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // ����PA0������Ϊ����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;// GPIO_Mode_IPU
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // ����EXTI��·
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
    
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; // ��ƽ��ת�����ж� EXTI_Trigger_Falling
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    // ����EXTI�ж�
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


void OOK_TIM4Iint(uint16_t arr,uint16_t psc)
{

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    // ʹ��TIM4ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    
    // ����TIM4��������
    TIM_TimeBaseStructure.TIM_Period = arr - 1;  // ��������Ϊarr����ÿ1ms����һ���ж�
    TIM_TimeBaseStructure.TIM_Prescaler = psc-1; // Ԥ��Ƶϵ��Ϊpsc����72MHz / 24 = 3MHz
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    
    // ����TIM4�ж�
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    // ʹ��TIM4�����ж�
    //TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    
    // ����TIM4����
    TIM_Cmd(TIM4, ENABLE);
	
}

//ͨ�ö�ʱ���жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
void TIM3_Int_Init(u16 arr,u16 psc)//��������5ms
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = arr-1; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc-1; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(  //ʹ�ܻ���ʧ��ָ����TIM�ж�
		TIM3, //TIM2
		TIM_IT_Update ,
		ENABLE  //ʹ��
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx����
							 
}

//�ⲿ�����ж� ���� ����Ԫ���ߺ�����Ϣ
unsigned char flag_FS_OVER=0;
uint32_t FS_buff[5];//���ն��л���

void Get_RF_Data(void)
{
	Timer_cnt = TIM4->CNT;
	TIM4->CNT = 0;
	if((Timer_cnt>9000 ))// �յ�ͬ��
	{
		rf_bit_cnt = Timer_cnt/16;//Լֵ����������//rf_large =   (Timer_cnt/31*32)/16;
		error_bit_large = rf_bit_cnt*3;
//		error_bit_small = rf_bit_cnt/2;
		if(b_cnt==24 )
		{
			if(rf_dat != ook_info.RF_Data_Last)//���ϴβ�ͬ 
			{
				ook_info.R_Same_CountS = 0;
			}
			else//���ϴ���ͬ
			{
				//ook ���κ���
//				OOK_DN();
//				flag_FS_OVER=1;
//				FS_buff[0]=rf_dat;
//				ook_info.RF_Data_Last=0;
//				rf_dat=0;
				
				ook_info.dly = 10;    //2s
				ook_info.R_Same_CountS ++;	//��ͬ���м���
				if(rf_dat != ook_info.RF_Data)//����ͬһ������ 
				{
					ook_info.RF_Data = rf_dat;
					//
					for(uint8_t i=0;i<flag_FS_OVER;i++)
					{
						if(FS_buff[i] == rf_dat)
						{
							rf_start = 1;
							rf_dat = 0;
							b_cnt = 0;
							return;
						}
					}
					if(flag_FS_OVER < 5)
					{
						FS_buff[flag_FS_OVER] = rf_dat ;
						flag_FS_OVER++;	
					}
					else 
					{
						for(uint8_t j=0; j<3; j++)
						{
							FS_buff[j]=FS_buff[j+1];
						}
						FS_buff[4] = rf_dat ;
					}
				}
				else
				{
					rf_start = 1;
					rf_dat = 0;
					b_cnt = 0;
					return;
				}
			}				
			ook_info.RF_Data_Last = rf_dat;
		}
		rf_start = 1;
		rf_dat = 0;
		b_cnt = 0;
		return;
	}
	else 
	{
		if(Timer_cnt>error_bit_large)//�쳣��λ
		{
			rf_dat = 0;
			b_cnt = 0;
			rf_start = 0;
			return;
		}
		else
		{
			if(PIN_RF)//������
			{
				if(rf_start == 0)//ûͬ��
				{
					rf_dat = 0;
					b_cnt = 0;
				}
				else//��ͬ��
				{ 
					if(b_cnt<24)
					{
						rf_dat<<=1;
						if(Timer_cnt > rf_bit_cnt)
							rf_dat+=0;
						else
							rf_dat+=1;
					}
					b_cnt++;
				}
			}
			else
			{
				
			}
		}
/*
		if(PIN_RF)//������
		{
			if(rf_start == 0)//ûͬ��
			{
				rf_dat = 0;
				b_cnt = 0;
			}
			else//��ͬ��
			{
				if(Timer_cnt>error_bit)//�쳣��λ
				{
					rf_dat = 0;
					b_cnt = 0;
					rf_start = 0;
				}
				else 
				{
					if(b_cnt<24)
					{
						rf_dat<<=1;
						if(Timer_cnt > rf_large)
							rf_dat+=0;
						else
							rf_dat+=1;
					}
					b_cnt++;
				}
			}
		}
		else
		{
			if(Timer_cnt>error_bit)//�쳣��λ
			{
				rf_dat = 0;
				b_cnt = 0;
				rf_start = 0;
			}
		}
*/
	}
}


void ook_process(void)
{
	//uint8_t temp=0;
	//uint8_t ook_Num=0;
//	uint8_t buff[12]={0};
//	if(flag_FS_OVER)	//ÿһ�� ֻȡ�ʼ�����ݣ�Ȼ������buff����һ��
//	{		
//		temp = (uint8_t)FS_buff[0] & 0x0f;  //ȡ��ֵ
//		FS_buff[0] &= 0xfffffff0;           //��ֵ��0
//		for(uint8_t j=0;j<4;j++)			//�����ֽ�˳�� --> ���ֽ���ǰ 
//		{
//			buff[3-j]=(uint8_t)(FS_buff[0]>>(8*j));
//		}
//		buff[4]=temp&0x0f;                  //���ݺ�һ���ֽڴ�ż�ֵ
//		usart_buffer_len = uart_protocol_send( 0x11, ( unsigned char* )&buff, sizeof( buff ), usart_buffer );
//		uart_send( usart_buffer, usart_buffer_len );
//		clr_buff( usart_buffer, usart_buffer_len );
		
//		flag_FS_OVER--;//OOK_test(���ν���)
	
		OLED_ShowHexNum(20,48,FS_buff[0],8);
		OLED_Refresh();
		FS_buff[0]=0; 
		if(--flag_FS_OVER) // ook ���ݻ��� FS_buff[i]=FS_buff[i+1]
		{	
			for(uint8_t k=0; k<flag_FS_OVER+1; k++)
			{
				FS_buff[k]=FS_buff[k+1];
				FS_buff[k+1]=0;
			}		
		}
			
//	}
}
