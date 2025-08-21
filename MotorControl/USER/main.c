#include <stdio.h>
#include "usart.h"
#include "systick.h"
#include "pwm.h"
#include "encoder.h"
#include "timer.h"
 
extern int Moto;//���PWM���� Ӧ��Motor��
extern int Position,Target_Position; //���������������

extern int Encoder,Target_Velocity;

int main()
{
	SysTick_Init();
	uart_init(115200);
	
//	Motor_Ctrl_Init();
	
//	KEY_Init();
	
	Encoder_Init_TIM(0xffff, 3);
	
//	Timerx_Init(500-1, 16800-1);
	
//	Remote_Ctrl(2);
	
//	TIM_SetCompare1(MOTOR_PWMA_TIMx, 8300);
	
	int temp = 0;
	
	while(1)
	{
//		printf("Position: %d  ",Position);		//��ǰ������λ�ô�ӡ������
//		printf("Moto: %d  ",Moto);				//��ǰ��PID�����������ӡ������
//		printf("Target: %d\r\n",Target_Position);//��ǰ��λ��Ŀ��ֵ��ӡ������
//		Delay_ms(1000);
		
//		printf("%d\t%d\t%d\r\n",Position,Moto,Target_Position);		//��ǰ������λ�ô�ӡ������
//		Delay_ms(10);
		
//			printf("Velocity: %d  ",Encoder);		//��ǰ������λ�ô�ӡ������
//			printf("Moto: %d  ",Moto);				//��ǰ��PID�����������ӡ������
//			printf("Target: %d\r\n",Target_Velocity);//��ǰ��λ��Ŀ��ֵ��ӡ������
//			Delay_ms(1000);
		
//		printf("%d\t%d\t%d\r\n",Encoder,Moto,Target_Velocity);		//��ǰ������λ�ô�ӡ������
		temp = Read_Encoder_TIM();
		printf("%d\r\n",temp);
		Delay_ms(500);
	}
	
	return 0;
}

