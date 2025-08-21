#include "key.h"

/**************************************************************************
�������ܣ�������ʼ��
��ڲ�������
����  ֵ���� 
**************************************************************************/
void KEY_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);                      //ʹ�� PORTA ʱ�� 
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;  //��������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 			//PE4
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
} 
/**************************************************************************
�������ܣ�����ɨ��
��ڲ�����˫���ȴ�ʱ��
����  ֵ������״̬ 0���޶��� 1������ 2��˫�� 
**************************************************************************/
u8 click_N_Double (u8 time)
{
	static	u8 flag_key,count_key,double_key;	
	static	u16 count_single,Forever_count;
	if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4) == Bit_RESET)  
		Forever_count++;   //������־λδ��1
	else       
		Forever_count = 0;

	if(Bit_RESET == GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4) && 0 == flag_key)		
		flag_key=1;	

	if(0 == count_key)
	{
		if(flag_key==1) 
		{
			double_key++;
			count_key=1;	
		}
		
		if(double_key==2) 
		{
			double_key=0;
			count_single=0;
			return 2;//˫��ִ�е�ָ��
		}
	}
	
	if(Bit_SET == GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4))
	{
		flag_key = 0;
		count_key = 0;
	}

	if(1 == double_key)
	{
		count_single++;
		if(count_single>time && Forever_count<time)
		{
			double_key=0;
			count_single=0;	
			return 1;//����ִ�е�ָ��
		}
		if(Forever_count>time)
		{
			double_key=0;
			count_single=0;	
		}
	}	
	return 0;
}
/**************************************************************************
�������ܣ�����ɨ��
��ڲ�������
����  ֵ������״̬ 0���޶��� 1������ 
**************************************************************************/
u8 click(void)
{
	static u8 flag_key=1;//�������ɿ���־
	if(flag_key && GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4) == Bit_RESET)
	{
		flag_key=0;
		return 1;	// ��������
	}
	else if(Bit_SET == GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4))
		flag_key=1;
	
	return 0;//�ް�������
}
/**************************************************************************
�������ܣ��������
��ڲ�������
����  ֵ������״̬ 0���޶��� 1������2s
**************************************************************************/
u8 Long_Press(void)
{
	static u16 Long_Press_count,Long_Press;
	if(Long_Press == 0 && GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4) == Bit_RESET)  
		Long_Press_count++;   //������־λδ��1
	else                       
		Long_Press_count=0; 

	if(Long_Press_count>40)		
	{
		Long_Press=1;	
		Long_Press_count=0;
		return 1;
	}	
	
	if(Long_Press==1)     //������־λ��1
	{
		Long_Press=0;
	}
	return 0;
}

