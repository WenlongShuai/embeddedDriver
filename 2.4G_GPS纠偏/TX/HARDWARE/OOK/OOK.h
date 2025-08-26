#ifndef __OOK_H
#define __OOK_H
#include "sys.h"
#include "oled.h"



#define LED_PC13_ON 	GPIOC->ODR &= ~GPIO_Pin_13;
#define LED_PC13_OFF 	GPIOC->ODR |= GPIO_Pin_13;
#define LED0 PCout(13)

typedef struct
{
        unsigned long RF_Data;
        unsigned long RF_Data_Last;
        unsigned char R_Same_CountS;
        unsigned char dly;
}_OOK_info;

extern _OOK_info ook_info;

extern unsigned char flag_FS_OVER;

void LED_GPIO_confing(void);
void OOK_TIM4Iint(uint16_t arr,uint16_t psc);
void OOK_GPIO_Config(void);
void Get_RF_Data(void);
void ook_process(void);
void TIM3_Int_Init(u16 arr,u16 psc);

#endif
