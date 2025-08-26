#include "GY86_IIC.h"
#include "MPU6050.h"
#include "inv_mpu.h"
#include "delay.h"
#include "sys.h"
#include <math.h>
#include "inv_mpu_dmp_motion_driver.h"

float Pitch,Roll,Yaw; 

//************MPU6050��ʼ��******************************************************************//

u8 Init_MPU6050(void)
{
	u8 temp[1]={0};
  GY86_Write_shortByte(MPU_ADDR,MPU_PWR_MGMT1_REG,0X80);	                      //��λMPU6050	
  delay_ms(200);
  GY86_Write_shortByte(MPU_ADDR,MPU_PWR_MGMT1_REG,0X00);	                      //����MPU6050 	
	GY86_Write_shortByte(MPU_ADDR,MPU_PWR_MGMT1_REG,0X02);	                      //����CLKSEL,PLL Y��Ϊ�ο�
	GY86_Write_shortByte(MPU_ADDR,MPU_PWR_MGMT2_REG,0X00);	                      //���ٶ��������Ƕ�����
	MPU_Set_Gyro_Fsr(1);					                                                //�����Ǵ�����,��1000dps
	MPU_Set_Accel_Fsr(0);					                                                //���ٶȴ�����,��2g	
	MPU_Set_Rate(200);
	mpu_dmp_init();

	GY86_Read_Byte(MPU_ADDR,MPU_USER_CTRL_REG,1,temp);
	GY86_Write_shortByte(MPU_ADDR,MPU_USER_CTRL_REG,temp[0]&0xC8);         //I2C_MST_EN=0
 
	GY86_Read_Byte(MPU_ADDR,MPU_INTBP_CFG_REG,1,temp);
	GY86_Write_shortByte(MPU_ADDR,MPU_INTBP_CFG_REG,temp[0]|0x02);        // I2C_BYPASS_EN=1;
	
//	MPU6050_setI2CMasterModeEnabled(0);	                                          //����MPU6050 ����AUXI2C
//	MPU6050_setI2CBypassEnabled(1);
//	GY86_Write_shortByte(MPU_ADDR,MPU_FIFO_EN_REG,0X23);	//����FIFO
//	GY86_Write_shortByte(MPU_ADDR,MPU_INTBP_CFG_REG,0X80);	//INT���ŵ͵�ƽ��Ч

  return 0;
}


//����MPU6050�����Ǵ����������̷�Χ
//fsr:0,��250dps;1,��500dps;2,��1000dps;3,��2000dps
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
u8 MPU_Set_Gyro_Fsr(u8 fsr)
{
	return GY86_Write_shortByte(MPU_ADDR,MPU_GYRO_CFG_REG,fsr<<3);                //���������������̷�Χ  
}


//����MPU6050���ٶȴ����������̷�Χ
//fsr:0,��2g;1,��4g;2,��8g;3,��16g
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
u8 MPU_Set_Accel_Fsr(u8 fsr)
{
	return GY86_Write_shortByte(MPU_ADDR,MPU_ACCEL_CFG_REG,fsr<<3);               //���ü��ٶȴ����������̷�Χ  
}


//����MPU6050�Ĳ�����(�ٶ�Fs=1KHz)
//rate:4~1000(Hz)
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
u8 MPU_Set_Rate(u16 rate)
{
	u8 data;
	if(rate>1000)rate=1000;
	if(rate<4)rate=4;
	data=1000/rate-1;
	data=GY86_Write_shortByte(MPU_ADDR,MPU_SAMPLE_RATE_REG,data);	               //�������ֵ�ͨ�˲���
 	return MPU_Set_LPF(rate/2);                                       	         //�Զ�����LPFΪ�����ʵ�һ��
}	

//����MPU6050�����ֵ�ͨ�˲���
//lpf:���ֵ�ͨ�˲�Ƶ��(Hz)
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
u8 MPU_Set_LPF(u16 lpf)
{
	u8 data=0;
	if(lpf>=188)data=1;
	else if(lpf>=98)data=2;
	else if(lpf>=42)data=3;
	else if(lpf>=20)data=4;
	else if(lpf>=10)data=5;
	else data=6; 
	return GY86_Write_shortByte(MPU_ADDR,MPU_CFG_REG,data);                      //�������ֵ�ͨ�˲���  
}


//�ر�I2C_MAST ʹ��
u8 MPU6050_setI2CMasterModeEnabled(u8 enable)
{
  GY86_bitchange(MPU_ADDR,MPU_USER_CTRL_REG,5,enable);
	return 0;
}

//�������HMC5883L
u8 MPU6050_setI2CBypassEnabled(u8 enable)
{
  GY86_bitchange(MPU_ADDR,MPU_INTBP_CFG_REG,1,enable);
	return 0;
}


//�õ�dmp����������(ע��,��������Ҫ�Ƚ϶��ջ,�ֲ������е��)
//pitch:������ ����:0.1��   ��Χ:-90.0�� <---> +90.0��
//roll:�����  ����:0.1��   ��Χ:-180.0��<---> +180.0��
//yaw:�����   ����:0.1��   ��Χ:-180.0��<---> +180.0��
//����ֵ:0,����
//    ����,ʧ��
void READ_MPU6050()
{
	  float q0=1.0f,q1=0.0f,q2=0.0f,q3=0.0f;
	  short gyro[3], accel[3], sensors;
	  unsigned long sensor_timestamp;
		unsigned char more;
		long quat[4];

				dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, &sensors, &more);		
				if (sensors & INV_WXYZ_QUAT )
				{    
					 q0=quat[0] / q30;
					 q1=quat[1] / q30;
					 q2=quat[2] / q30;
					 q3=quat[3] / q30;
					 Pitch = asin(-2 * q1 * q3 + 2 * q0* q2)* 57.3; 	
					 Roll = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1)* 57.3; // roll		 
					 Yaw = 	atan2(2*(q1*q2 + q0*q3),q0*q0+q1*q1-q2*q2-q3*q3) * 57.3;
				}
}


