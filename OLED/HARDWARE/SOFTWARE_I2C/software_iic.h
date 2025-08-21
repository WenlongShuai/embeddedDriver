#ifndef __SOFTWARE_IIC_H__
#define __SOFTWARE_IIC_H__

#include "sys.h"


//IO����
#define IIC2_SDA_GPIO_RCC   	RCC_AHB1Periph_GPIOC
#define IIC2_SDA_GPIO_PORT   	GPIOC
#define IIC2_SDA_GPIO_PIN   	GPIO_Pin_9

#define IIC2_SCL_GPIO_RCC   	RCC_AHB1Periph_GPIOA
#define IIC2_SCL_GPIO_PORT   	GPIOA
#define IIC2_SCL_GPIO_PIN   	GPIO_Pin_8

//IO��������
#define SDA2_IN()  {GPIOC->MODER&=~(3<<(9*2));GPIOC->MODER|=0<<9*2;}	//PF0����ģʽ
#define SDA2_OUT() {GPIOC->MODER&=~(3<<(9*2));GPIOC->MODER|=1<<9*2;} 	//PF0���ģʽ
//IO��������	 
#define IIC2_SCL    PAout(8) //SCL
#define IIC2_SDA    PCout(9) //SDA	 
#define READ2_SDA   PCin(9)  //����SDA 

//IIC���в�������
void IICx_Init(void);
void IIC2_Start(void);				//����IIC��ʼ�ź�
void IIC2_Stop(void);	  			//����IICֹͣ�ź�
void IIC2_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 IIC2_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 IIC2_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC2_Ack(void);					//IIC����ACK�ź�
void IIC2_NAck(void);				//IIC������ACK�ź�

#endif /* __IIC_H__ */
