#include "time.h"






void my_PWM1(void)
	
{
	GPIO_InitTypeDef GPIO_InitStructure;
    // ʹ��GPIOAʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    // ����PA0������Ϊ����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;// GPIO_Mode_IPU
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
		//GPIOB->ODR |= GPIO_Pin_12;
}

