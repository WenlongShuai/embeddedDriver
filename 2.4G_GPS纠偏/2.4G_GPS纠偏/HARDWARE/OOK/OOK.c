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
    
    // 使能GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // 配置PA0引脚作为输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;// GPIO_Mode_IPU
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 配置EXTI线路
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
    
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; // 电平翻转触发中断 EXTI_Trigger_Falling
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    // 配置EXTI中断
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
    
    // 使能TIM4时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    
    // 配置TIM4基本参数
    TIM_TimeBaseStructure.TIM_Period = arr - 1;  // 计数周期为arr，即每1ms触发一次中断
    TIM_TimeBaseStructure.TIM_Prescaler = psc-1; // 预分频系数为psc，即72MHz / 24 = 3MHz
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    
    // 配置TIM4中断
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    // 使能TIM4更新中断
    //TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    
    // 启动TIM4计数
    TIM_Cmd(TIM4, ENABLE);
	
}

//通用定时器中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
void TIM3_Int_Init(u16 arr,u16 psc)//计数器用5ms
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = arr-1; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc-1; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(  //使能或者失能指定的TIM中断
		TIM3, //TIM2
		TIM_IT_Update ,
		ENABLE  //使能
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设
							 
}

//外部引脚中断 调用 解析元无线呼叫信息
unsigned char flag_FS_OVER=0;
uint32_t FS_buff[5];//接收队列缓存

void Get_RF_Data(void)
{
	Timer_cnt = TIM4->CNT;
	TIM4->CNT = 0;
	if((Timer_cnt>9000 ))// 收到同步
	{
		rf_bit_cnt = Timer_cnt/16;//约值减少运算量//rf_large =   (Timer_cnt/31*32)/16;
		error_bit_large = rf_bit_cnt*3;
//		error_bit_small = rf_bit_cnt/2;
		if(b_cnt==24 )
		{
			if(rf_dat != ook_info.RF_Data_Last)//与上次不同 
			{
				ook_info.R_Same_CountS = 0;
			}
			else//与上次相同
			{
				//ook 单次呼叫
//				OOK_DN();
//				flag_FS_OVER=1;
//				FS_buff[0]=rf_dat;
//				ook_info.RF_Data_Last=0;
//				rf_dat=0;
				
				ook_info.dly = 10;    //2s
				ook_info.R_Same_CountS ++;	//相同呼叫计数
				if(rf_dat != ook_info.RF_Data)//不是同一个呼叫 
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
		if(Timer_cnt>error_bit_large)//异常大位
		{
			rf_dat = 0;
			b_cnt = 0;
			rf_start = 0;
			return;
		}
		else
		{
			if(PIN_RF)//上升沿
			{
				if(rf_start == 0)//没同步
				{
					rf_dat = 0;
					b_cnt = 0;
				}
				else//已同步
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
		if(PIN_RF)//上升沿
		{
			if(rf_start == 0)//没同步
			{
				rf_dat = 0;
				b_cnt = 0;
			}
			else//已同步
			{
				if(Timer_cnt>error_bit)//异常大位
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
			if(Timer_cnt>error_bit)//异常大位
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
//	if(flag_FS_OVER)	//每一次 只取最开始的数据，然后数据buff左移一个
//	{		
//		temp = (uint8_t)FS_buff[0] & 0x0f;  //取键值
//		FS_buff[0] &= 0xfffffff0;           //键值补0
//		for(uint8_t j=0;j<4;j++)			//调整字节顺序 --> 高字节在前 
//		{
//			buff[3-j]=(uint8_t)(FS_buff[0]>>(8*j));
//		}
//		buff[4]=temp&0x0f;                  //数据后一个字节存放键值
//		usart_buffer_len = uart_protocol_send( 0x11, ( unsigned char* )&buff, sizeof( buff ), usart_buffer );
//		uart_send( usart_buffer, usart_buffer_len );
//		clr_buff( usart_buffer, usart_buffer_len );
		
//		flag_FS_OVER--;//OOK_test(单次接收)
	
		OLED_ShowHexNum(20,48,FS_buff[0],8);
		OLED_Refresh();
		FS_buff[0]=0; 
		if(--flag_FS_OVER) // ook 数据还有 FS_buff[i]=FS_buff[i+1]
		{	
			for(uint8_t k=0; k<flag_FS_OVER+1; k++)
			{
				FS_buff[k]=FS_buff[k+1];
				FS_buff[k+1]=0;
			}		
		}
			
//	}
}
