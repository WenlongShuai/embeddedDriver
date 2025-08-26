#include "delay.h"
#include "sys.h"
#include "oled.h"
#include "bmp.h"

/* 定时器标志位 */
extern uint8_t Flag_100ms;
extern uint8_t Flag_500ms;

/* 串口2DMA接收数据量 */
#define Rx2DMA_NUM 1600

//经纬度数据显示
#define nmea_Lat "lat:%f"
#define nmea_Lon "lon:%f"

//数据协议格式
// Head   Data_num   0x00  Data   END + 0x0a + 0x0d    

#define rtcm_Head  "rtcm_Head"
#define rtcm_End   "rtcm_End\r\n"   // \r 回车 值：0x0d   \n 换行 值：0x0a

/* nmea数据结构体 */
nmea_raw_t nmea_raw;

int main(void)
{

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组
	
	uart_init(115200);
	DMA_RX1Init();    //DMA1_CH5 -- RX1
	DMA_TX1Init();		//DMA1_CH4 -- TX1
	
	USART2_RX_DMA(115200);	//DMA1_CH6 -- RX2

	delay_init();
	OLED_Init();
	OLED_Clear();
	OLED_ShowString(56,0,(uint8_t*)"Test",12);
	
	DMA_RX1(DMA1_Channel5,UART_RX1_SIZE);
//	MYDMA_Enable(DMA1_Channel6,Rx2DMA_NUM);//开启一次DMA传输
	
	while(1)
	{
	
		/* 接收处理rtcm数据 */
		if(Flag_DMA_RX1)
		{
			int Rx1DMA_CNT=0;
			static int DuSe_CNT=0 ,Rec_cnt=0;
			/* 获取DMA接收到的数据个数 */
			Rx1DMA_CNT = UART_RX1_SIZE - RX1_DMA_CH->CNDTR ;
			/*  CRC校验，成功返回1  */
			if(RTCM_Parse(Buff_USART1_RX ,Rx1DMA_CNT))
			{
				/*  	协议格式																			*/
				/*		Head   Data_num   0x00  Data   END + \r\n  	*/
				uint16_t rDATA_len=0;  																							//数据包长记录
				rDATA_len = sizeof(rtcm_Head)-1;
				memcpy(Buff_USART1_TX ,rtcm_Head , rDATA_len);  										//添加数据头
				Buff_USART1_TX[rDATA_len++] = Rx1DMA_CNT >> 8;											//添加数据长度
				Buff_USART1_TX[rDATA_len++] = Rx1DMA_CNT ;
				Buff_USART1_TX[rDATA_len++] = 0x00;																	//插入0x00
				memcpy(Buff_USART1_TX+rDATA_len ,Buff_USART1_RX ,Rx1DMA_CNT);				//拷贝数据	
				rDATA_len+=Rx1DMA_CNT;      																				//记录数据包长度
				memcpy(Buff_USART1_TX + rDATA_len, rtcm_End , sizeof(rtcm_End)-1);	//添加数据尾											
				rDATA_len+=sizeof(rtcm_End)-1;																			//数据包总长度
				
				OLED_ShowNum(0,20,Rx1DMA_CNT,4,12);
				if(Flag_DMA_TX1 == 0)									//确保上一帧数据已传输完成
				{
					Flag_DMA_TX1=1;		 									//置位标志位,完成后在中断中清除
					DMA_TX1(TX1_DMA_CH,rDATA_len);			//数据通过DMA发送出去
				}
				else 												//记录数据堵塞次数
				{
					
 				}	
			}
			Flag_DMA_RX1 = 0; 									//清除标志位
			DMA_RX1(RX1_DMA_CH,UART_RX1_SIZE);	//准备下一帧数据接收			
		}
		
//		/* 查询串口2接收数据是否完成 */
//		if(flag_Rx2DMA)
//		{
//			volatile int  Rx2DMA_CNT=0;//串口2DMA接收个数
//			double pLat=0.0, pLon=0.0;
//			/* 显示空格刷新定位质量值 */
//			OLED_ShowChar(120,2,' ',12);
//			
//			/* 获取DMA接收到的数据个数 */
//			Rx2DMA_CNT = Rx2DMA_NUM - DMA1_Channel6->CNDTR ;
//			
//			/* 处理数据 */
//			nmeagga((char*)USART2_RxBuff ,nmea_raw.buf ,Rx2DMA_CNT);//抽取GGA整个字段
//		
//			MYDMA_Enable(DMA1_Channel6,Rx2DMA_NUM);//开启下一次的DMA传输
//			flag_Rx2DMA=0;//处理完清除标志位
//			
//			PCout(13)=!PCout(13);//指示灯翻转
//			//数据解析
//			if(decode_nmea(&nmea_raw) != 0 ) continue;
//			
//			/* GPS数据纠偏 */
////			bd2wgs_Exactly(nmea_raw.lat, nmea_raw.lon, &pLat, &pLon);//百度转地球坐标
////			wgs2gcj(nmea_raw.lat, nmea_raw.lon, &pLat, &pLon);//GPS转高德坐标
////			bd2gcj(nmea_raw.lat, nmea_raw.lon, &pLat, &pLon);//百度转高德
//			
//			/* 显示定位质量值 */
//			OLED_ShowNum(120,0,nmea_raw.solq,1,12);
//			char show_lat[21]={0};
//			char show_lon[21]={0};
//			sprintf(show_lat,nmea_Lat,nmea_raw.lat);
//			OLED_ShowString(0,26,(uint8_t*)show_lat,12);//显示lat(百度坐标)
//			sprintf(show_lon,nmea_Lon,nmea_raw.lon);
//			OLED_ShowString(0,39,(uint8_t*)show_lon,12);//显示lon
//		}
	}
}















