/**
  ******************************************************************************
  * @file    main.c
  * @author  LYC
  * @version V1.0
  * @date    2024-03-9
  * @brief   MPU6050 ���IIC����
  ******************************************************************************
  * @attention
  * ʵ��ƽ̨:Ұ�� F103-C8T6 STM32 ���İ� 
  ******************************************************************************
  */
  
#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include "./systick/bsp_SysTick.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_usart.h"
#include "./mpu6050/mpu6050.h"

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
#include "mpu6050_SL.h"


#ifdef   soft_IIC
#include "./iic/bsp_soft_i2c.h"
#else
#include "./iic/bsp_hard_i2c.h"
#endif

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
    
    /* ����SysTick��ʱ�����ж� */
    SysTick_Init(); //���� SysTick Ϊ 1ms �ж�һ�Σ����ж����ȡ����������
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; //������ʱ��
    
    /* LED �˿ڳ�ʼ�� */
    LED_GPIO_Config();
    /* ����1ͨ�ų�ʼ�� */
    USART_Config();
    
    //I2C��ʼ��
    #ifdef soft_IIC
        i2c_GPIO_Config();
    #else
        MPU_I2C_Config();
    #endif
    
    //MPU6050��ʼ��
    MPU6050_Init();
    
    //��ʼ��MPU6050
    MPU6050_mpu_init();
    MPU6050_mpl_init();
    MPU6050_config();
    printf("����MPU6050������ʾ����\n");
    //���MPU6050
    if(MPU6050ReadID() == 0)
    {
        printf("\r\nû�м�⵽MPU6050��������\r\n");
        LED1_ON; 
        while(1);	
    }
    printf("\r\n ��ӭʹ��Ұ��  STM32 �����塣\r\n");
    printf("\r\n ����һ��MPU6050�������� \r\n");
    
    mdelay(500);
    while (1)
    {
        unsigned long sensor_timestamp;
        int new_data = 0;

        /* ���յ�INT�������ж���Ϣ��������� */
        if (!hal.sensors || !hal.new_gyro)
        {
            LED1_ON;
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
            mdelay(100);
        }

    }
	
}

/*********************************************END OF FILE**********************/
