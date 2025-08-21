#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "timer.h"
#include "HC_SR04.h"


int main(void)
{ 
	float temp = 0;
	delay_init(168);
	uart_init(115200);
	Hcsr04Init();
	printf("hc-sr04 start!\r\n");
	while(1)
	{
		Hcsr04Start();//  ����������ģ����
		delay_ms(150); //  �������200ms
		// ��ӡ�����
		temp = Hcsr04Read();
		//if(temp != 38.080002)
		printf("distance:%.2f cm\r\n", temp);
	}
}
