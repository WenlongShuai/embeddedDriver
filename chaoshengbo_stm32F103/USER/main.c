#include "stm32f10x.h"
#include "usart.h"
#include "timer.h"
#include "HC_SR04.h"
#include "oled.h"


extern u8  TIMxCHx_CAPTURE_STA;	//���벶��״̬		    				
extern u32	TIMxCHx_CAPTURE_VAL;	//���벶��ֵ(TIM2/TIM5��32λ)
extern float distance;
u32 time=0; 
int main(void)
{ 
	char buff[10] = {0};
	float averageBuff[3] = {0};
	uint8_t count = 0;
	SysTick_Init();
	uart_init(115200);
	Hcsr04Init();
	OLED_Init();
		
	printf("hc-sr04 start!\r\n");
		
	while(1)
	{
		Hcsr04Start();//  ����������ģ����
		
		if(TIMxCHx_CAPTURE_STA&0X80)//�ɹ�������һ��������
		{
			time = TIMxCHx_CAPTURE_STA&0X3F;
			time *= 65535;//���ʱ���ܺ�
			time += TIMxCHx_CAPTURE_VAL;//�õ��ܵĸߵ�ƽʱ��
			distance = time*0.033/2;
			printf("----> %.2f\r\n", distance);
			
			if(distance < 400 && distance > 2)
			{
				count %= 3;
				averageBuff[count++] = distance;
			}
			
//			sprintf(buff, "dis:%.2fcm", distance);
//			OLED_ShowStr(0, 3, (unsigned char*)buff, 2);
			
			TIMxCHx_CAPTURE_STA=0;//������һ�β���
		}		
		
//		float sum = 0;
//		for(int i = 0;i < 3;i++)
//		{
//			sum += averageBuff[i];
//		}
		
		sprintf(buff, "dis:%.2fcm", distance);
		OLED_ShowStr(0, 3, (unsigned char*)buff, 2);
		
		Delay_ms(200);
	}
}
