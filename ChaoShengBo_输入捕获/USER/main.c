#include "sys.h"
#include "usart.h"
#include "timer.h"
#include "HC_SR04.h"


extern u8  TIMxCHx_CAPTURE_STA;	//���벶��״̬		    				
extern u32	TIMxCHx_CAPTURE_VAL;	//���벶��ֵ(TIM2/TIM5��32λ)
extern float distance;

int main(void)
{ 
	u32 time=0; 
	SysTick_Init();
	uart_init(115200);
	Hcsr04Init();
	
	printf("hc-sr04 start!\r\n");
	while(1)
	{
		Hcsr04Start();//  ����������ģ����
		
		if(TIMxCHx_CAPTURE_STA&0X80)//�ɹ�������һ��������
		{
			time=TIMxCHx_CAPTURE_STA&0X3F;
			time *= 65535;//���ʱ���ܺ�
			time+=TIMxCHx_CAPTURE_VAL;//�õ��ܵĸߵ�ƽʱ��
			distance = time*0.033/2;
			printf("----> %.2f\r\n",distance);
			TIMxCHx_CAPTURE_STA=0;//������һ�β���
		}		
		Delay_ms(70);
	}
}
