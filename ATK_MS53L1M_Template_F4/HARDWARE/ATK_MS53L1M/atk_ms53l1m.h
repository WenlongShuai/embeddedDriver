/**
 ****************************************************************************************************
 * @file        atk_ms53l1m.h
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

#ifndef __ATK_MS53L1M_H
#define __ATK_MS53L1M_H

#include "sys.h"
#include "atk_ms53l1m_iic.h"
#include "vl53l1_api.h"

#define ATK_MS53L1M_NUM 1

/* ATK-MS53L0Mģ���ϵ�Ĭ��IICͨѶ��ַ */
#define ATK_MS53L1M_IIC_ADDR    0x52

/* ATK-MS53L0ģ��ID */
#define ATK_MS53L1M_MODULE_ID   0xEACC

typedef struct
{
    uint32_t TimingBudget;  /*��������,ִ��һ�����̲��������ʱ��,��λ:us,��Χ[20ms, 1000ms] */
    uint32_t InterPeriod; /*��������֮��ļ��ʱ�� ��λ:ms, ��Χ[1ms, 1000ms]*/   

    /*
      1������������ʱ����ڶ�ʱԤ��,��һ���豸��ɲ��,����һ�����������ʼ.
      2�����������ʱ��Ӧ���ü�ʱԤ��Ͳ�������,�û�����ֹͣ���,������Щ�����������������.
    */
    
}vl53l1_mode; 

extern VL53L1_Dev_t dev;    /* vl53l1x�豸 */

/* �������� */
void atk_ms53l1m_init(uint8_t dir);

#endif
