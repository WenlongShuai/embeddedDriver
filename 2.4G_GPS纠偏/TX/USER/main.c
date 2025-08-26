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

//����Э���ʽ
// Head   Data_num   0x00  Data   END + 0x0a + 0x0d    

#define rtcm_Head  "rtcm_Head"
#define rtcm_End   "rtcm_End\r\n"   // \r �س� ֵ��0x0d   \n ���� ֵ��0x0a

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
	
	DMA_RX1(DMA1_Channel5,UART_RX1_SIZE);
//	MYDMA_Enable(DMA1_Channel6,Rx2DMA_NUM);//����һ��DMA����
	
	while(1)
	{
	
		/* ���մ���rtcm���� */
		if(Flag_DMA_RX1)
		{
			int Rx1DMA_CNT=0;
			static int DuSe_CNT=0 ,Rec_cnt=0;
			/* ��ȡDMA���յ������ݸ��� */
			Rx1DMA_CNT = UART_RX1_SIZE - RX1_DMA_CH->CNDTR ;
			/*  CRCУ�飬�ɹ�����1  */
			if(RTCM_Parse(Buff_USART1_RX ,Rx1DMA_CNT))
			{
				/*  	Э���ʽ																			*/
				/*		Head   Data_num   0x00  Data   END + \r\n  	*/
				uint16_t rDATA_len=0;  																							//���ݰ�����¼
				rDATA_len = sizeof(rtcm_Head)-1;
				memcpy(Buff_USART1_TX ,rtcm_Head , rDATA_len);  										//�������ͷ
				Buff_USART1_TX[rDATA_len++] = Rx1DMA_CNT >> 8;											//������ݳ���
				Buff_USART1_TX[rDATA_len++] = Rx1DMA_CNT ;
				Buff_USART1_TX[rDATA_len++] = 0x00;																	//����0x00
				memcpy(Buff_USART1_TX+rDATA_len ,Buff_USART1_RX ,Rx1DMA_CNT);				//��������	
				rDATA_len+=Rx1DMA_CNT;      																				//��¼���ݰ�����
				memcpy(Buff_USART1_TX + rDATA_len, rtcm_End , sizeof(rtcm_End)-1);	//�������β											
				rDATA_len+=sizeof(rtcm_End)-1;																			//���ݰ��ܳ���
				
				OLED_ShowNum(0,20,Rx1DMA_CNT,4,12);
				if(Flag_DMA_TX1 == 0)									//ȷ����һ֡�����Ѵ������
				{
					Flag_DMA_TX1=1;		 									//��λ��־λ,��ɺ����ж������
					DMA_TX1(TX1_DMA_CH,rDATA_len);			//����ͨ��DMA���ͳ�ȥ
				}
				else 												//��¼���ݶ�������
				{
					
 				}	
			}
			Flag_DMA_RX1 = 0; 									//�����־λ
			DMA_RX1(RX1_DMA_CH,UART_RX1_SIZE);	//׼����һ֡���ݽ���			
		}
		
//		/* ��ѯ����2���������Ƿ���� */
//		if(flag_Rx2DMA)
//		{
//			volatile int  Rx2DMA_CNT=0;//����2DMA���ո���
//			double pLat=0.0, pLon=0.0;
//			/* ��ʾ�ո�ˢ�¶�λ����ֵ */
//			OLED_ShowChar(120,2,' ',12);
//			
//			/* ��ȡDMA���յ������ݸ��� */
//			Rx2DMA_CNT = Rx2DMA_NUM - DMA1_Channel6->CNDTR ;
//			
//			/* �������� */
//			nmeagga((char*)USART2_RxBuff ,nmea_raw.buf ,Rx2DMA_CNT);//��ȡGGA�����ֶ�
//		
//			MYDMA_Enable(DMA1_Channel6,Rx2DMA_NUM);//������һ�ε�DMA����
//			flag_Rx2DMA=0;//�����������־λ
//			
//			PCout(13)=!PCout(13);//ָʾ�Ʒ�ת
//			//���ݽ���
//			if(decode_nmea(&nmea_raw) != 0 ) continue;
//			
//			/* GPS���ݾ�ƫ */
////			bd2wgs_Exactly(nmea_raw.lat, nmea_raw.lon, &pLat, &pLon);//�ٶ�ת��������
////			wgs2gcj(nmea_raw.lat, nmea_raw.lon, &pLat, &pLon);//GPSת�ߵ�����
////			bd2gcj(nmea_raw.lat, nmea_raw.lon, &pLat, &pLon);//�ٶ�ת�ߵ�
//			
//			/* ��ʾ��λ����ֵ */
//			OLED_ShowNum(120,0,nmea_raw.solq,1,12);
//			char show_lat[21]={0};
//			char show_lon[21]={0};
//			sprintf(show_lat,nmea_Lat,nmea_raw.lat);
//			OLED_ShowString(0,26,(uint8_t*)show_lat,12);//��ʾlat(�ٶ�����)
//			sprintf(show_lon,nmea_Lon,nmea_raw.lon);
//			OLED_ShowString(0,39,(uint8_t*)show_lon,12);//��ʾlon
//		}
	}
}















