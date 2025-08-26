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
	OLED_ShowString(30,12,(uint8_t*)"jam:",12);
	OLED_ShowString(80,12,(uint8_t*)"crc:",12);
	
	DMA_RX1(DMA1_Channel5,UART_RX1_SIZE);
	MYDMA_Enable(DMA1_Channel6,Rx2DMA_NUM);//开启一次DMA传输
	
	while(1)
	{
	
		/* 接收处理rtcm数据 */
		if(Flag_DMA_RX1)
		{		
			static char jam_cnt=0 , crc_err=0;//发送堵塞次数，crc校验失败次数
			
			/*  数据协议格式                                       */
			/*  Head   Data_num   0x00  Data   END + 0x0a + 0x0d  */ 
			uint16_t rtcmData_Len = 0 ;         						//保存数据长度
			
			rtcmData_Len = rxBuff[0x09]<<8 | rxBuff[0x0A] ; //偏移数据头长度，提取数据个数
			OLED_ShowNum(1,0,rtcmData_Len,4,12); 						//显示接收到rtcm数据个数
			if(rtcmData_Len >= rxBuff_SIZE) 								//数据越界
			{
				Flag_DMA_RX1 =0;
				continue;																			//程序结束
			}
			memcpy(Buff_USART1_TX ,(rxBuff + 0xC) , rtcmData_Len);//拷贝指定长度数据
			if(RTCM_Parse(Buff_USART1_TX ,rtcmData_Len))
			{
				/*    CRC校验成功,准备发送    */
				if(Flag_DMA_TX1 == 0)				//确保上一帧数据已传输完成
				{
					Flag_DMA_TX1=1;		 				//置位标志位,完成后在中断中清除
					DMA_TX1(TX1_DMA_CH, rtcmData_Len);
//					OLED_ShowNum(1,12,rtcmData_Len,4,12);  //显示发送的rtcm数据个数
					OLED_ShowNum(54,12,jam_cnt,3,12);
					jam_cnt > 100 ?  jam_cnt=0 : jam_cnt ++;
				}
				else 												             //记录数据堵塞次数
				{
//					OLED_ShowNum(54,12,jam_cnt,3,12);
//					jam_cnt > 100 ?  jam_cnt=0 : jam_cnt ++;
 				}	
			}
			else //CRC校验失败
			{
				//次数记录
				OLED_ShowNum(104,12,crc_err,3,12);
				crc_err > 100 ?  crc_err=0 : crc_err ++;
			}
			Flag_DMA_RX1 = 0; 									//清除标志位
//			DMA_RX1(RX1_DMA_CH,UART_RX1_SIZE);	//准备下一帧数据接收			
		}
		
		/* 查询串口2接收数据是否完成 */
		if(flag_Rx2DMA)
		{
			volatile int  Rx2DMA_CNT=0;//串口2DMA接收个数
			double pLat=0.0, pLon=0.0;
			/* 显示空格刷新定位质量值 */
			OLED_ShowChar(118,2,' ',12);
			
			/* 获取DMA接收到的数据个数 */
			Rx2DMA_CNT = Rx2DMA_NUM - DMA1_Channel6->CNDTR ;
			
			/* 处理数据 */
			nmeagga((char*)USART2_RxBuff ,nmea_raw.buf ,Rx2DMA_CNT);//抽取GGA整个字段
		
			MYDMA_Enable(DMA1_Channel6,Rx2DMA_NUM);//开启下一次的DMA传输
			flag_Rx2DMA=0;//处理完清除标志位
			
			PCout(13)=!PCout(13);//指示灯翻转
			//数据解析
			if(decode_nmea(&nmea_raw) != 0 ) continue;
			
			/* GPS数据纠偏 */
			wgs2bd(nmea_raw.lat, nmea_raw.lon, &pLat, &pLon); 					//GPS转百度
//			bd2wgs_Exactly(nmea_raw.lat, nmea_raw.lon, &pLat, &pLon);		//百度转地球坐标
//			wgs2gcj(nmea_raw.lat, nmea_raw.lon, &pLat, &pLon);					//GPS转高德坐标
//			bd2gcj(nmea_raw.lat, nmea_raw.lon, &pLat, &pLon);						//百度转高德
			
			/* 显示定位质量值 */
			OLED_ShowNum(118,0,nmea_raw.solq,1,12);
			char show_lat[16]={0};
			char show_lon[16]={0};
			sprintf(show_lat,nmea_Lat,pLat);
			OLED_ShowString(1,36,(uint8_t*)show_lat,12);//显示lat(百度坐标)
			sprintf(show_lon,nmea_Lon,pLon);
			OLED_ShowString(1,48,(uint8_t*)show_lon,12);//显示lon
		}
	}
}


