#include "HMC5883L.h"
#include "IMU_IIC_Hardware.h"
#include "IMU_IIC_Software.h"
#include "delay.h"
#include "sys.h"
#include "math.h"  

float  angle = 0;                                                  //����Ƕ�
                                                   //HMC5883L����ԭʼ����
//************************��ʼ��HMC5883L**************************************//
void  Init_HMC5883L(void)
{
	#if HMC5883L_IIC_TYPE
		Software_iic_init();                                                  /* ��ʼ��IIC�ӿ� */
	#else
		Hardware_IIC_Init();
	#endif
	HMC5883L_write_byte(HMC5883L_Addr, HMC58X3_R_CONFA, 0x14);	//����ƽ����1 �������75Hz ��������
	HMC5883L_write_byte(HMC5883L_Addr, HMC58X3_R_CONFB, 0x00);	//�������(��1.3Ga,1090��˹,-2048~2047)
	HMC5883L_write_byte(HMC5883L_Addr, HMC58X3_R_MODE, 0x00);		//��������ģʽ
}


/**
 * @brief       ��ATK-MS6050��ָ���Ĵ���д��һ�ֽ�����
 * @param       addr: ATK-MS6050��IICͨѶ��ַ
 *              reg : ATK-MS6050�Ĵ�����ַ
 *              dat : д�������
 * @retval      ATK_MS6050_EOK : ����ִ�гɹ�
 *              ATK_MS6050_EACK: IICͨѶACK���󣬺���ִ��ʧ��
 */
uint8_t HMC5883L_write_byte(uint8_t addr, uint8_t reg, uint8_t dat)
{
	uint8_t res = 0;
	#if HMC5883L_IIC_TYPE
		res = Software_Write_longByte(addr, reg, 1, &dat);
		if(res == 0)
			return HMC58X3_EOK;
		else 
			return HMC58X3_EACK;
	#else
		res = Hardware_Write_shortByte(addr, reg, dat);
		if(res == 0)
			return HMC58X3_EOK;
		else
			return HMC58X3_EACK;
	#endif
}

//*************************��ȡHMC5883L����***********************************//
void READ_HMC5883L(int16_t *mx, int16_t *my, int16_t *mz)
{
	int x,y,z;  
	u8 BUF[6],flag[1];                             //�������ݻ�����

	#if HMC5883L_IIC_TYPE
		Software_bitchange(HMC5883L_Addr,HMC58X3_R_CONFA,7,0);         //��һλʵʱ��0�����޷�����
		Software_Read_Byte(HMC5883L_Addr,0x09,1,flag);                 //�鿴HMC5883L���ݴ��״̬
	#else
		Hardware_bitchange(HMC5883L_Addr,HMC58X3_R_CONFA,7,0);         //��һλʵʱ��0�����޷�����
		Hardware_Read_Byte(HMC5883L_Addr,0x09,1,flag);                 //�鿴HMC5883L���ݴ��״̬
	#endif
	
	if(flag[0]&0x01)                                           //�ж�HMC5883L�����Ƿ������
	{
		#if HMC5883L_IIC_TYPE
			Software_Read_Byte(HMC5883L_Addr,0x03,6,BUF);
		#else
			Hardware_Read_Byte(HMC5883L_Addr,0x03,6,BUF);                //��ȡHMC5883L��X,Y,Z�����ݣ�OUT_X_L_A>OUT_X_H_A>OUT_Z_L_A>OUT_Z_H_A>OUT_Y_L_A>OUT_Y_H_A��
		#endif
		x=(BUF[0] << 8) | BUF[1];                                //�ϲ�X��Ĵ���������ݵĸ�λ���λ
		z=(BUF[2] << 8) | BUF[3];                                //�ϲ�Z��Ĵ���������ݵĸ�λ���λ
		y=(BUF[4] << 8) | BUF[5];                                //�ϲ�Y��Ĵ���������ݵĸ�λ���λ
		if(x>0x7fff)x-=0xffff;	                                 //X�������޷�
		if(z>0x7fff)z-=0xffff;	                                 //Z�������޷�
		if(y>0x7fff)y-=0xffff;	                                 //Y�������޷�
		
		*mx = x;
		*my = y;
		*mz = z;
//		*angle= atan2(x,y) * (180 / 3.14159265) + 180;            //����ɽǶ�
	}
}
