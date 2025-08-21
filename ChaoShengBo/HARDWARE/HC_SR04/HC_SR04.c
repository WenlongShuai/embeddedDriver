#include "HC_SR04.h"

extern float distance;

static void Hcsr04_TRIG_GPIO_Init()
{
	RCC_AHB1PeriphClockCmd(RCC_TRIG_PERIPH,ENABLE);        //ʹ��GPIOA������ʱ��
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;                             //ѡ���������ģʽ
	GPIO_InitStructure.GPIO_Pin=TRIG_PIN;                                    //ѡ��GPIO_Pin_2
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;                            //Ĭ��ѡ��50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  //������
	GPIO_Init(TRIG_PORT,&GPIO_InitStructure);
}

/**
 * @description: ������ģ������벶��ʱ��ͨ����ʼ��
 * @param {TIM_HandleTypeDef} *htim
 * @param {uint32_t} Channel
 * @return {*}
 */
void Hcsr04Init()
{
	Hcsr04_TRIG_GPIO_Init();
	TIMx_Init(100-1,84-1);
}

/**
 * @description: HC-SR04����
 * @param {*}
 * @return {*}
 */
void Hcsr04Start()
{
	GPIO_SetBits(TRIG_PORT,TRIG_PIN);
  delay_us(15);                   //����˵����,��Ҫ�ṩ����10us�ĸߵ�ƽ
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
