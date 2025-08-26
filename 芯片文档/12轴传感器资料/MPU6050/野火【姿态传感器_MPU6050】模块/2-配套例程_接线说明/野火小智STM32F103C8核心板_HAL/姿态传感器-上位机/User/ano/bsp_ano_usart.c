/**
  ******************************************************************************
  * @file    bsp_ano_usart.c
  * @author  fire
  * @version V1.0
  * @date    2024-xx-xx
  * @brief   ʹ�ô���1���ض���c��printf������usart�˿ڣ��жϽ���ģʽ
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32 F103 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 
#include "./ano/bsp_ano_usart.h"
#include "./usart/bsp_debug_usart.h"


#define BYTE0(dwTemp)       (*(char *)(&dwTemp))
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))
    
extern UART_HandleTypeDef UartHandle;

/**
  * @brief  ���ƴ��ڷ���1���ַ�
  * @param  c:Ҫ���͵��ַ�
  * @retval none
  */
void usart_send_char(uint8_t c)
{
	while(__HAL_UART_GET_FLAG(&UartHandle,USART_FLAG_TXE)==RESET); //ѭ������,ֱ���������
	HAL_UART_Transmit(&UartHandle,&c,1,1000);
}



/*�������ܣ���������������λ��Э��д����ʾŷ���ǵĳ�����λ��V7�汾��
 *����Э��˵����鿴��λ������İ���˵����
 */
void Data_Send_Status(float Pitch,float Roll,float Yaw)
{
	unsigned char i=0;
	unsigned char _cnt=0,sum = 0,add=0;
	unsigned int _temp;
	uint8_t data_to_send[50];

	data_to_send[_cnt++]=0xAA;//֡ͷ
	data_to_send[_cnt++]=0xFF;//Ŀ���ַ��0xFFΪ�㲥
	data_to_send[_cnt++]=0x03;//ŷ���Ǹ�ʽ
	data_to_send[_cnt++]=7;   //���ݳ���
	
	_temp = (int)(Roll*100);
	data_to_send[_cnt++]=BYTE0(_temp);
	data_to_send[_cnt++]=BYTE1(_temp);
	_temp = (int)(Pitch*100);
	data_to_send[_cnt++]=BYTE0(_temp);
	data_to_send[_cnt++]=BYTE1(_temp);
	_temp = (int)(Yaw*100);
	data_to_send[_cnt++]=BYTE0(_temp);
	data_to_send[_cnt++]=BYTE1(_temp);
	
	data_to_send[_cnt++]=0x00;
	
	//��У��
	for(i=0;i<_cnt;i++)
    {
		sum+= data_to_send[i];
        add+= sum;
    }
	data_to_send[_cnt++]=sum;
	data_to_send[_cnt++]=add;
    
	//���ڷ�������
	for(i=0;i<_cnt;i++)
		usart_send_char(data_to_send[i]);
}

/*�������ܣ���������������λ��Э��д����ʾ���������ݣ���λ��V7�汾��
 *����Э��˵����鿴��λ������İ���˵����
 */
void Send_Data(int16_t *Gyro,int16_t *Accel)
{
	unsigned char i=0;
	unsigned char _cnt=0,sum = 0,add = 0;

	uint8_t data_to_send[50];

	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0xFF;
	data_to_send[_cnt++]=0x01;
	data_to_send[_cnt++]=13;
	

	data_to_send[_cnt++]=BYTE0(Accel[0]);
	data_to_send[_cnt++]=BYTE1(Accel[0]);
	data_to_send[_cnt++]=BYTE0(Accel[1]);
	data_to_send[_cnt++]=BYTE1(Accel[1]);
	data_to_send[_cnt++]=BYTE0(Accel[2]);
	data_to_send[_cnt++]=BYTE1(Accel[2]);
	
	data_to_send[_cnt++]=BYTE0(Gyro[0]);
	data_to_send[_cnt++]=BYTE1(Gyro[0]);
	data_to_send[_cnt++]=BYTE0(Gyro[1]);
	data_to_send[_cnt++]=BYTE1(Gyro[1]);
	data_to_send[_cnt++]=BYTE0(Gyro[2]);
	data_to_send[_cnt++]=BYTE1(Gyro[2]);

    data_to_send[_cnt++]=0;

	//��У��
	for(i=0;i<_cnt;i++)
    {
		sum+= data_to_send[i];
        add+= sum;
    }
	data_to_send[_cnt++]=sum;
    data_to_send[_cnt++]=add;
	
	//���ڷ�������
	for(i=0;i<_cnt;i++)
		usart_send_char(data_to_send[i]);
}

