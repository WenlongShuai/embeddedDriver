#include "sys.h"


//����������IIC������HMC5883L
#define	HMC5883L_Addr   0x3D	//�ų�������������ַ

#define HMC58X3_R_CONFA 0x00  //���üĴ���A��ַ
#define HMC58X3_R_CONFB 0x01  //���üĴ���B��ַ
#define HMC58X3_R_MODE  0x02  //����ģʽ��ַ


void  Init_HMC5883L(void);
void READ_HMC5883L(void);

