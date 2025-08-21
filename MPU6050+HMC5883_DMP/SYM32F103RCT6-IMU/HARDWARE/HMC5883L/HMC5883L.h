#include "sys.h"

//IIC���ͣ�0��Ӳ��IIC 1�����IIC
#define HMC5883L_IIC_TYPE   1

//����������IIC������HMC5883L
//0x1E��ַ��δ���϶�дλ  ����(0x1E<<1)|1   д��(0x1E<<1)|0
#define	HMC5883L_Addr   0x1E	//�ų�������������ַ

#define HMC58X3_R_CONFA 0x00  //���üĴ���A��ַ
#define HMC58X3_R_CONFB 0x01  //���üĴ���B��ַ
#define HMC58X3_R_MODE  0x02  //����ģʽ��ַ

/* ����������� */
#define HMC58X3_EOK      0   /* û�д��� */
#define HMC58X3_EID      1   /* ID���� */
#define HMC58X3_EACK     2   /* IICͨѶACK���� */


void  Init_HMC5883L(void);
void READ_HMC5883L(void);
uint8_t HMC5883L_write_byte(uint8_t addr, uint8_t reg, uint8_t dat);

