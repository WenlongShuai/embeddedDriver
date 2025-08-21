#ifndef __HC_SR04_H__
#define __HC_SR04_H__

#include "timer.h"
#include "SysTick.h"
#include "stm32f10x.h"

#define RCC_TRIG_PERIPH 	RCC_APB2Periph_GPIOA
#define TRIG_PORT 				GPIOA
#define TRIG_PIN 					GPIO_Pin_1


static void Hcsr04_TRIG_GPIO_Init(void);

/**
 * @description: ������ģ������벶��ʱ��ͨ����ʼ��
 * @param {TIM_HandleTypeDef} *htim
 * @param {uint32_t} Channel
 * @return {*}
 */
void Hcsr04Init(void);

/**
 * @description: HC-SR04����
 * @param {*}
 * @return {*}
 */
void Hcsr04Start(void);

/**
 * @description: ��ȡ���� 
 * @param {*}
 * @return {*}
 */
float Hcsr04Read(void);


#endif
