#include "pid_position.h"
#include "usart.h"

float Position_KP=150,Position_KI=0.8,Position_KD=500; //PIDϵ��

/**************************************************************************
�������ܣ�λ��ʽPID������
��ڲ���������������λ����Ϣ��Ŀ��λ��
����  ֵ�����PWM
����λ��ʽ��ɢPID��ʽ 
pwm=Kp*e(k)+Ki*��e(k)+Kd[e��k��-e(k-1)]
e(k)������ƫ�� 
e(k-1)������һ�ε�ƫ��  
��e(k)����e(k)�Լ�֮ǰ��ƫ����ۻ���;����kΪ1,2,,k;
pwm�������
**************************************************************************/
int Position_PID(int position,int target)
{ 	
	static float Bias,Pwm,Integral_bias,Last_Bias;
	Bias = target - position;                                  //����ƫ��
	Integral_bias += Bias;	                                 //���ƫ��Ļ���
	if(Integral_bias > 3000)Integral_bias = 3000;
	if(Integral_bias < -3000)Integral_bias = -3000;
	Pwm = Position_KP * Bias+Position_KI * Integral_bias + Position_KD * (Bias-Last_Bias);       //λ��ʽPID������
	Last_Bias = Bias;                                       //������һ��ƫ�� 
	return Pwm;                                           //�������
}

