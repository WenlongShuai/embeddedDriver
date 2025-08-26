#include "sys.h"
#include "delay.h"
#include "GY86_IIC.h"
#include "MPU6050.h"
#include "HMC5883L.h"
#include "MS5611.h"
#include "inv_mpu.h"
#include "usart.h"

extern float Pitch,Roll,Yaw; 
extern float  angle; 

int main(void)
{	
	uart_init(115200);
	delay_init();                                   //��ʼ����ʱ����
	GY86_IIC_Init();	                    	        //��ʼ��IIC����
//	Init_MPU6050();                                 //��ʼ��MPU6050 3����ټ�+3��������
	Init_HMC5883L();                                //��ʼ��HMC5883L 3���������
	//Init_MS561101BA();                            //��ʼ��MS5611 ��ѹ�ƣ��߶ȼƣ�
	while(1)
		{
			//MS561101BA_GetTemperature(MS561101BA_D2_OSR_4096);
			//MS561101BA_getPressure(MS561101BA_D1_OSR_4096);
	 	READ_HMC5883L();                              //��ȡ������HMC5883L����	
    delay_us(5);	
//    READ_MPU6050();                               //��ȡ������MPU6050����
//    delay_us(5);			
			
//		printf("Pitch = %f   Roll = %f Yaw = %f\r\n",Pitch,Roll,Yaw);
			
		printf("angle = %.2f\r\n",angle);
		delay_ms(500);
	  }
}

