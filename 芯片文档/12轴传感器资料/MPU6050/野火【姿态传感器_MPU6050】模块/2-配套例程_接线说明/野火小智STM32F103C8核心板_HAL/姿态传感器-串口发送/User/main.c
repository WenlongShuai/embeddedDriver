/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103 STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx.h"
#include "stm32f1xx_it.h"
#include "./usart/bsp_debug_usart.h"
#include <stdlib.h>
#include "./led/bsp_led.h" 
#include "./mpu6050/mpu6050.h"
#include "mpu6050_SL.h"

/* mpu6050 DMP��  begin */
#include "eMPL_outputs.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "invensense.h"
#include "invensense_adv.h"
#include "log.h"
#include "mltypes.h"
#include "mpu.h"
#include "packet.h"

#ifdef i2c_soft
    #include "./iic/bsp_soft_i2c.h"
#else
    #include "./iic/bsp_hard_i2c.h"
#endif

extern void get_ms_user(unsigned long *count); 

#define get_tick_count get_ms_user

HAL_StatusTypeDef status = HAL_OK;
/* MPU6050���� */
short Accel[3];
short Gyro[3];
float Temp;
/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{   
    unsigned char new_temp = 0;
    unsigned long timestamp;
    
    /* ����ϵͳʱ��Ϊ72 MHz */
    SystemClock_Config();
    /* ��ʼ��USART1 ����ģʽΪ 115200 8-N-1 */
    DEBUG_USART_Config();
    /* ��ʼ��LED */
    LED_GPIO_Config();
    /* ��ʼ��MPU6050��INT���� */
    MPU_INT_GPIO_Init();
    /* ���ݺ궨��ѡ���������Ӳ��I2C */
    #ifdef i2c_soft
        i2c_GPIO_Config();
    #else
        MPU_I2C_Config();
    #endif
        

    //��ʼ��MPU6050
    MPU6050_mpu_init();
    MPU6050_mpl_init();
    MPU6050_config();
    
    printf("��ӭʹ��Ұ�𡪡�MPU6050����\n");
    HAL_Delay(500);
    
    while (1)
    {
        unsigned long sensor_timestamp;
        int new_data = 0;

        /* ���յ�INT�������ж���Ϣ��������� */
        if (!hal.sensors || !hal.new_gyro)
        {
            continue;
        }

        /* ÿ��500ms��ȡһ���¶� */
        get_tick_count(&timestamp);
        if (timestamp > hal.next_temp_ms)
        {
            hal.next_temp_ms = timestamp + TEMP_READ_MS;
            new_temp = 1;
        }

        /* ���յ������� ���� ����DMP */
        if (hal.new_gyro && hal.dmp_on)
        {
            short gyro[3], accel_short[3], sensors;
            unsigned char more;
            long accel[3], quat[4], temperature;
            /* ��DMP����ʹ��ʱ���ú�����FIFO��ȡ������ */
            dmp_read_fifo(gyro, accel_short, quat, &sensor_timestamp, &sensors, &more);
            /* ���more=0�������ݱ���ȡ��� */
            if (!more)
                hal.new_gyro = 0;

            /* ��ȡ���Ӧ�������ݣ�����MPL */
            //��Ԫ������
            if (sensors & INV_WXYZ_QUAT)
            {
                inv_build_quat(quat, 0, sensor_timestamp);
                new_data = 1;
            }

            //����������
            if (sensors & INV_XYZ_GYRO)
            {
                inv_build_gyro(gyro, sensor_timestamp);
                new_data = 1;
                if (new_temp)
                {
                    new_temp = 0;
                    /* ��ȡ�¶ȣ������������¶ȱȽ� */
                    mpu_get_temperature(&temperature, &sensor_timestamp);
                    inv_build_temp(temperature, sensor_timestamp);
                }
            }
            
            //���ٶȼ�����
            if (sensors & INV_XYZ_ACCEL)
            {
                accel[0] = (long)accel_short[0];
                accel[1] = (long)accel_short[1];
                accel[2] = (long)accel_short[2];
                inv_build_accel(accel, 0, sensor_timestamp);
                new_data = 1;
            }
        }

        if (new_data)
        {
            long data[9];
            int8_t accuracy;
            /* ������յ������� */
            if (inv_execute_on_data())
            {
                printf("���ݴ���\n");
            }

            float float_data[3];
            if (inv_get_sensor_type_euler(data, &accuracy, (inv_time_t *)&timestamp))
            {
                float_data[0] = data[0] * 1.0 / (1 << 16);
                float_data[1] = data[1] * 1.0 / (1 << 16);
                float_data[2] = data[2] * 1.0 / (1 << 16);
                printf("ŷ����(rad)\t\t: %7.5f\t %7.5f\t %7.5f\t\r\n", float_data[0], float_data[1], float_data[2]);
            }
            if (inv_get_sensor_type_accel(data, &accuracy, (inv_time_t *)&timestamp))
            {
                float_data[0] = data[0] * 1.0 / (1 << 16);
                float_data[1] = data[1] * 1.0 / (1 << 16);
                float_data[2] = data[2] * 1.0 / (1 << 16);
                printf("���ٶ�(g/s)\t\t: %7.5f\t %7.5f\t %7.5f\t\r\n", float_data[0], float_data[1], float_data[2]);
            }

            if (inv_get_sensor_type_gyro(data, &accuracy, (inv_time_t *)&timestamp))
            {
                float_data[0] = data[0] * 1.0 / (1 << 16);
                float_data[1] = data[1] * 1.0 / (1 << 16);
                float_data[2] = data[2] * 1.0 / (1 << 16);
                printf("���ٶ�(rad/s)\t\t: %7.5f\t %7.5f\t %7.5f\t\r\n", float_data[0], float_data[1], float_data[2]);
            }
            
            HAL_Delay(100);
        }

    }

}


/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 72000000
  *            HCLK(Hz)                       = 72000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 8000000
  *            HSE PREDIV1                    = 1
  *            PLLMUL                         = 9
  *            Flash Latency(WS)              = 2
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef clkinitstruct = {0};
  RCC_OscInitTypeDef oscinitstruct = {0};
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  oscinitstruct.OscillatorType  = RCC_OSCILLATORTYPE_HSE;
  oscinitstruct.HSEState        = RCC_HSE_ON;
  oscinitstruct.HSEPredivValue  = RCC_HSE_PREDIV_DIV1;
  oscinitstruct.PLL.PLLState    = RCC_PLL_ON;
  oscinitstruct.PLL.PLLSource   = RCC_PLLSOURCE_HSE;
  oscinitstruct.PLL.PLLMUL      = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&oscinitstruct)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  clkinitstruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1;
  clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV2;  
  if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_2)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
