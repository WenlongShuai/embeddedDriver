#include "HMC5883L.h"
#include "GY86_IIC.h"
#include "delay.h"
#include "sys.h"
#include "math.h"  

float  angle;                                                  //����Ƕ�
                                                   //HMC5883L����ԭʼ����
//************************��ʼ��HMC5883L**************************************//
void  Init_HMC5883L(void)
{

	GY86_Write_shortByte(HMC5883L_Addr,HMC58X3_R_CONFA,0x14);    //����ƽ����1 �������75Hz ��������
  GY86_Write_shortByte(HMC5883L_Addr,HMC58X3_R_CONFB,0x00);    //�������(��1.3Ga,1090��˹,-2048~2047)
  GY86_Write_shortByte(HMC5883L_Addr,HMC58X3_R_MODE,0x00);     //��������ģʽ
 
}

//*************************��ȡHMC5883L����***********************************//
void READ_HMC5883L(void)
{
	   int x,y,z;  
	   u8 BUF[6],flag[1];                             //�������ݻ�����
	    
	  GY86_bitchange(HMC5883L_Addr,HMC58X3_R_CONFA,7,0);         //��һλʵʱ��0�����޷�����
    GY86_Read_Byte(HMC5883L_Addr,0x09,1,flag);                 //�鿴HMC5883L���ݴ��״̬
		if(flag[0]&0x01)                                           //�ж�HMC5883L�����Ƿ������
		{
			GY86_Read_Byte(HMC5883L_Addr,0x03,6,BUF);                //��ȡHMC5883L��X,Y,Z�����ݣ�OUT_X_L_A>OUT_X_H_A>OUT_Z_L_A>OUT_Z_H_A>OUT_Y_L_A>OUT_Y_H_A��
			x=(BUF[0] << 8) | BUF[1];                                //�ϲ�X��Ĵ���������ݵĸ�λ���λ
			z=(BUF[2] << 8) | BUF[3];                                //�ϲ�Z��Ĵ���������ݵĸ�λ���λ
      y=(BUF[4] << 8) | BUF[5];                                //�ϲ�Y��Ĵ���������ݵĸ�λ���λ
			if(x>0x7fff)x-=0xffff;	                                 //X�������޷�
			if(z>0x7fff)z-=0xffff;	                                 //Z�������޷�
			if(y>0x7fff)y-=0xffff;	                                 //Y�������޷�
			angle= atan2(x,y) * (180 / 3.14159265) + 180;            //����ɽǶ�
			
		}
}
