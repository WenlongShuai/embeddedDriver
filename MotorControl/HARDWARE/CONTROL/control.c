#include "control.h"

int Moto;//���PWM���� Ӧ��Motor��
int Position=10,Target_Position=10000; //���������������
int Encoder=0,Target_Velocity=0;
	
/**************************************************************************
�������ܣ���ֵ��PWM�Ĵ���
��ڲ�����PWM
����  ֵ����
**************************************************************************/
void Set_Pwm(int moto)
{
	if(moto < 0)			
	{
		GPIO_ResetBits(MOTOR_AIN2_GPIO, MOTOR_AIN2_PIN);	 // �͵�ƽ
		GPIO_SetBits(MOTOR_AIN1_GPIO, MOTOR_AIN1_PIN);	 // �ߵ�ƽ
	}
	else
	{
		GPIO_ResetBits(MOTOR_AIN1_GPIO, MOTOR_AIN1_PIN);	 // �͵�ƽ
		GPIO_SetBits(MOTOR_AIN2_GPIO, MOTOR_AIN2_PIN);	 // �ߵ�ƽ
	}
	
	TIM_SetCompare1(MOTOR_PWMA_TIMx, myabs(moto));
}



/**************************************************************************
�������ܣ�����PWM��ֵ 
��ڲ�������
����  ֵ����
**************************************************************************/
void Xianfu_Pwm(void)
{	
	int Amplitude = 8300;    //===PWM������8400 ������8300
	if(Moto < -Amplitude) 
		Moto = -Amplitude;
	
	if(Moto > Amplitude)  
		Moto = Amplitude;		
}
/**************************************************************************
�������ܣ������޸�����״̬ 
��ڲ�������
����  ֵ����
**************************************************************************/
void Key(void)  
{	
//	int tmp,Position_Amplitude = 260; 
//	tmp = click_N_Double(10);//��ⰴ�� 
//	if(tmp==1)Target_Position += Position_Amplitude;  //��������λ��
//	else if(tmp==2)Target_Position -= Position_Amplitude;  //˫����Сλ��
	
	int tmp,Velocity_Amplitude = 10; 
	tmp = click_N_Double(10);//��ⰴ�� 
	if(tmp==1)Target_Velocity += Velocity_Amplitude;  //��������λ��
	else if(tmp==2)Target_Velocity -= Velocity_Amplitude;  //˫����Сλ��
	
	if(Target_Velocity > 40)
	{
		Target_Velocity = 40;
	}
	else if(Target_Velocity < -40)
	{
		Target_Velocity = -40;
	}
	
 }
/**************************************************************************
�������ܣ�ȡ����ֵ
��ڲ�����int
����  ֵ��unsigned int
**************************************************************************/
int myabs(int a)
{ 		   
	int temp;
	if(a < 0)  temp = -a;  
	else temp = a;
	return temp;
}
