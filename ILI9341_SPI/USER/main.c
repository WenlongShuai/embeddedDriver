#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "lcd_demo.h"
#include "lcd_ili9341.h"


int main()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);		//��ʱ��ʼ�� 
	uart_init(115200);	//���ڳ�ʼ��������Ϊ115200
	
	Lcd_Init();
	
	while(1)
	{
			QDTFT_Test_Demo();	//See the test details in QDTFT_Demo.c		
	}
	
	return 0;
}