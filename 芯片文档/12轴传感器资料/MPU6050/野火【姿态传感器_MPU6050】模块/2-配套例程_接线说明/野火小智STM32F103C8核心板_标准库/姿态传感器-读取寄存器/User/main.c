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
    
    while(1)
    {
    if( task_readdata_finish ) //task_readdata_finish = 1 ��ʾ��ȡMPU6050�������
    {
    
        printf("\r\n���ٶȣ� %8d%8d%8d    ",Accel[0],Accel[1],Accel[2]);
        printf("�����ǣ� %8d%8d%8d    ",Gyro[0],Gyro[1],Gyro[2]);
        printf("�¶ȣ� %8.2f",Temp);
        
        task_readdata_finish = 0; // �����־λ
    }
    }
	
}

/*********************************************END OF FILE**********************/
