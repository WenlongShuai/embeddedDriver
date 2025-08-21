#include "HC_SR04.h"

extern float distance;

static void Hcsr04_TRIG_GPIO_Init()
{
	RCC_APB2PeriphClockCmd(RCC_TRIG_PERIPH,ENABLE);        //ʹ��GPIOA������ʱ��
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//ѡ���������ģʽ
	GPIO_InitStructure.GPIO_Pin = TRIG_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TRIG_PORT,&GPIO_InitStructure);
	GPIO_ResetBits(TRIG_PORT,TRIG_PIN);
}

/**
 * @description: ������ģ������벶��ʱ��ͨ����ʼ��
 * @param {TIM_HandleTypeDef} *htim
 * @param {uint32_t} Channel
 * @return {*}
 */
void Hcsr04Init()
{
	TIMx_CHx_Cap_Init(0xFFFF,72-1);
	Hcsr04_TRIG_GPIO_Init();
}

/**
 * @description: HC-SR04����
 * @param {*}
 * @return {*}
 */
void Hcsr04Start()
{
	GPIO_SetBits(TRIG_PORT,TRIG_PIN);
	Delay_us(10);//����˵����,��Ҫ�ṩ����10us�ĸߵ�ƽ
  GPIO_ResetBits(TRIG_PORT,TRIG_PIN);
}


/**
 * @description: ��ȡ���� 
 * @param {*}
 * @return {*}
 */
float Hcsr04Read()
{
  // ������޷�
  if(distance >= 400)
  {
    distance = 400;
  }
	else if(distance <= 2)
	{
		distance = 2;
	}
		
  return distance;
}
