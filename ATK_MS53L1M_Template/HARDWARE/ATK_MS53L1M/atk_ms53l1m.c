/**
 ****************************************************************************************************
 * @file        atk_ms53l1m.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2024-06-12
 * @brief       ATK-MS53L1Mģ����������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� MiniSTM32 V4������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "atk_ms53l1m.h"
#include "delay.h"

/**
 * @brief       ATK-MS53L1Mģ��Ӳ����ʼ��
 * @param       ��
 * @retval      ��
 */
void atk_ms53l1m_init(void)
{
	#if 0
  atk_ms53l1m_iic_init();
	#else
	Hardware_IIC_Init();
	#endif
  
	delay_ms(100);
}
