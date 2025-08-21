#ifndef __HC_SR04_H__
#define __HC_SR04_H__

#include "delay.h"
#include "timer.h"
#include "stm32f4xx_conf.h"

#define RCC_TRIG_PERIPH 	RCC_AHB1Periph_GPIOA
#define TRIG_PORT 				GPIOA
#define TRIG_PIN 					GPIO_Pin_2


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
