#include "delay.h"
#include "sys.h"
#include "oled.h"
#include "bmp.h"

/* ��ʱ����־λ */
extern uint8_t Flag_100ms;
extern uint8_t Flag_500ms;

/* ����2DMA���������� */
#define Rx2DMA_NUM 1600



//��γ��������ʾ
#define nmea_Lat "lat:%f"
#define nmea_Lon "lon:%f"

/* nmea���ݽṹ�� */
nmea_raw_t nmea_raw;

int main(void)
{

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����
	
	uart_init(115200);
	DMA_RX1Init();    //DMA1_CH5 -- RX1
	DMA_TX1Init();		//DMA1_CH4 -- TX1
	
	USART2_RX_DMA(115200);	//DMA1_CH6 -- RX2

	delay_init();
	OLED_Init();
	OLED_Clear();
	OLED_ShowString(56,0,(uint8_t*)"Test",12);
	OLED_ShowString(30,12,(uint8_t*)"jam:",12);
	OLED_ShowString(80,12,(uint8_t*)"crc:",12);
	
	DMA_RX1(DMA1_Channel5,UART_RX1_SIZE);
	MYDMA_Enable(DMA1_Channel6,Rx2DMA_NUM);//����һ��DMA����
	
	while(1)
	{
	
		/* ���մ���rtcm���� */
		if(Flag_DMA_RX1)
		{		
			static char jam_cnt=0 , crc_err=0;//���Ͷ���������crcУ��ʧ�ܴ���
			
			/*  ����Э���ʽ                                       */
			/*  Head   Data_num   0x00  Data   END + 0x0a + 0x0d  */ 
			uint16_t rtcmData_Len = 0 ;         						//�������ݳ���
			
			rtcmData_Len = rxBuff[0x09]<<8 | rxBuff[0x0A] ; //ƫ������ͷ���ȣ���ȡ���ݸ���
			OLED_ShowNum(1,0,rtcmData_Len,4,12); 						//��ʾ���յ�rtcm���ݸ���
			if(rtcmData_Len >= rxBuff_SIZE) 								//����Խ��
			{
				Flag_DMA_RX1 =0;
				continue;																			//�������
			}
			memcpy(Buff_USART1_TX ,(rxBuff + 0xC) , rtcmData_Len);//����ָ����������
			if(RTCM_Parse(Buff_USART1_TX ,rtcmData_Len))
			{
				/*    CRCУ��ɹ�,׼������    */
				if(Flag_DMA_TX1 == 0)				//ȷ����һ֡�����Ѵ������
				{
					Flag_DMA_TX1=1;		 				//��λ��־λ,��ɺ����ж������
					DMA_TX1(TX1_DMA_CH, rtcmData_Len);
//					OLED_ShowNum(1,12,rtcmData_Len,4,12);  //��ʾ���͵�rtcm���ݸ���
					OLED_ShowNum(54,12,jam_cnt,3,12);
					jam_cnt > 100 ?  jam_cnt=0 : jam_cnt ++;
				}
				else 												             //��¼���ݶ�������
				{
//					OLED_ShowNum(54,12,jam_cnt,3,12);
//					jam_cnt > 100 ?  jam_cnt=0 : jam_cnt ++;
 				}	
			}
			else //CRCУ��ʧ��
			{
				//������¼
				OLED_ShowNum(104,12,crc_err,3,12);
				crc_err > 100 ?  crc_err=0 : crc_err ++;
			}
			Flag_DMA_RX1 = 0; 									//�����־λ
//			DMA_RX1(RX1_DMA_CH,UART_RX1_SIZE);	//׼����һ֡���ݽ���			
		}
		
		/* ��ѯ����2���������Ƿ���� */
		if(flag_Rx2DMA)
		{
			volatile int  Rx2DMA_CNT=0;//����2DMA���ո���
			double pLat=0.0, pLon=0.0;
			/* ��ʾ�ո�ˢ�¶�λ����ֵ */
			OLED_ShowChar(118,2,' ',12);
			
			/* ��ȡDMA���յ������ݸ��� */
			Rx2DMA_CNT = Rx2DMA_NUM - DMA1_Channel6->CNDTR ;
			
			/* �������� */
			nmeagga((char*)USART2_RxBuff ,nmea_raw.buf ,Rx2DMA_CNT);//��ȡGGA�����ֶ�
		
			MYDMA_Enable(DMA1_Channel6,Rx2DMA_NUM);//������һ�ε�DMA����
			flag_Rx2DMA=0;//�����������־λ
			
			PCout(13)=!PCout(13);//ָʾ�Ʒ�ת
			//���ݽ���
			if(decode_nmea(&nmea_raw) != 0 ) continue;
			
			/* GPS���ݾ�ƫ */
			wgs2bd(nmea_raw.lat, nmea_raw.lon, &pLat, &pLon); 					//GPSת�ٶ�
//			bd2wgs_Exactly(nmea_raw.lat, nmea_raw.lon, &pLat, &pLon);		//�ٶ�ת��������
//			wgs2gcj(nmea_raw.lat, nmea_raw.lon, &pLat, &pLon);					//GPSת�ߵ�����
//			bd2gcj(nmea_raw.lat, nmea_raw.lon, &pLat, &pLon);						//�ٶ�ת�ߵ�
			
			/* ��ʾ��λ����ֵ */
			OLED_ShowNum(118,0,nmea_raw.solq,1,12);
			char show_lat[16]={0};
			char show_lon[16]={0};
			sprintf(show_lat,nmea_Lat,pLat);
			OLED_ShowString(1,36,(uint8_t*)show_lat,12);//��ʾlat(�ٶ�����)
			sprintf(show_lon,nmea_Lon,pLon);
			OLED_ShowString(1,48,(uint8_t*)show_lon,12);//��ʾlon
		}
	}
}


