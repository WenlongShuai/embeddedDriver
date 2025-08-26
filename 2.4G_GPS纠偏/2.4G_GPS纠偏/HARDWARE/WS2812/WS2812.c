#include "ws2812.h"



ws2812_t g_ws2812 = { 0 };
/* Functions Prototypes ------------------------------------------------------*/
void WS2812_Show(void);
void rgb_to_hsv(uint16_t *H, uint16_t *S, uint16_t *V, uint8_t r, uint8_t g, uint8_t b);
void hsv_to_rgb(int h, int s, int v, uint8_t *r, uint8_t *g, uint8_t *b);

extern void spi_send_bytes_dma(uint8_t* txbuff,uint16_t len);
/* Functions -----------------------------------------------------------------*/


//基准时钟

//uint32 转  uint8 RGB
void Sw28_Color2RGB(uint32_t color, uint8_t* r, uint8_t* g, uint8_t* b)
{
    *b = (uint8_t)color;
    *r = (uint8_t)(color>>8);
    *g = (uint8_t)(color>>16);
}

/**
 * @brief uint8 RGB 转 uint32
 *
 * @param red
 * @param green
 * @param blue
 * @return uint32_t
 */
uint32_t WS2812_Color(uint8_t red, uint8_t green, uint8_t blue) {
	return red << 16 | green << 8 | blue;
}

/**
 * @brief 设置某一个WS2812
 *
 * @param num
 * @param RGB
 */
void WS2812_OneSet(uint8_t num, uint32_t RGB) {
	uint8_t i;
	uint32_t TempR = 0, TempG = 0, TempB = 0;

	//MSB First
	for (i = 0; i < 8; ++i, RGB >>= 1) {
		(RGB & 0x00010000) != 0 ? (TempR |= (WS2812_HIG << (i * 3))) : (TempR |= (WS2812_LOW << (i * 3)));
		(RGB & 0x00000100) != 0 ? (TempG |= (WS2812_HIG << (i * 3))) : (TempG |= (WS2812_LOW << (i * 3)));
		(RGB & 0x00000001) != 0 ? (TempB |= (WS2812_HIG << (i * 3))) : (TempB |= (WS2812_LOW << (i * 3)));
	}

	for (i = 0; i < 3; i++) {
		g_ws2812.Col[num].RGB.R[i] = TempR >> (16 - 8 * i);
		g_ws2812.Col[num].RGB.G[i] = TempG >> (16 - 8 * i);
		g_ws2812.Col[num].RGB.B[i] = TempB >> (16 - 8 * i);
	}
}

/**
 * @brief   将数据通过DMA发送去WS2812
 *
 */
extern void sys_spi_1_write_tx_more_low_rate(uint8_t *buf,uint8_t len);
void WS2812_Show(void) {
    //发送数据
//	for (int i = 0;i < WS2812_NUM; i++) 
//	{
//		for (int x = 0; x < 9; x=x+3) 
//		{
//			tx_data_wp=GPCOM_GET_TX_WP(GPCOM3_BASE_ADDR);
//			GPCOM_SEND_TX_DATA(GPCOM3_BASE_ADDR,tx_data_wp+0,g_ws2812.Col[i].Buff[x]);
//			GPCOM_SEND_TX_DATA(GPCOM3_BASE_ADDR,tx_data_wp+1,g_ws2812.Col[i].Buff[x+1]);
//			GPCOM_SEND_TX_DATA(GPCOM3_BASE_ADDR,tx_data_wp+2,g_ws2812.Col[i].Buff[x+2]);
//			tx_data_wp+=3;
//			GPCOM_SEND_TX_WP(GPCOM3_BASE_ADDR,tx_data_wp);
//			while(!(GPCOM_TX_FIFO_EMPTY(GPCOM3_BASE_ADDR))){};
//        }
//    }
//	for(int i = 0;i < 9;i++)
//		g_ws2812.Col[0].Buff[i] = 0x55;
//	NSS_H;
	SPI_SendData_DMA(&g_ws2812.Col[0].Buff[0],WS2812_NUM*9);//sys_spi_1_write_tx_more_low_rate(&g_ws2812.Col[0].Buff[0],WS2812_NUM*9);
	
}

/**
 * @brief   关闭所有的WS2812
 *
 */
void WS2812_CloseAll(void) {
	uint16_t i;

	for (i = 0; i < WS2812_NUM; i++) {
		WS2812_OneSet(i, 0);
	}
	WS2812_Show();
}

/**
 * @brief   给所有的WS2812设定某一个颜色
 *
 * @param RGB
 */
void WS2812_SetAll(uint32_t RGB) {
	uint16_t i;

	for (i = 0; i < WS2812_NUM; i++) {
		WS2812_OneSet(i, RGB);
	}
	WS2812_Show();
}
uint8_t led_status[WS2812_NUM] ={0};
uint8_t call_list_cnt = 0;
uint8_t LED_StateBuff[WS2812_NUM]={0x01};//LED状态外部传入buff
uint8_t Flag_WS2812=0;
void LED_Pro(uint8_t *LED_State,uint8_t num)
{
	uint8_t Led_ColseAll=0;
	if(num==1)//只有一个LED时(G360用)
	{
		Led_ColseAll=1;
		for(uint8_t loop_i=0;loop_i<7;loop_i++)
		led_status[loop_i]=*LED_State;
	}
	else 
		memcpy(led_status,LED_State,num);
	//设置灯色
	for(uint8_t i = 0;i < WS2812_NUM;i++)
	{
		switch(led_status[i])
		{
			case 0://OFF
				WS2812_OneSet(i, 0x000000);
			if(Led_ColseAll)
				//led_dis_trig=0;//LED闪烁标志(中断执行)
			break;
			case 1://GREEN
				WS2812_OneSet(i, 0x000f00);
			break;
			case 2://BLUE
				WS2812_OneSet(i, 0x00000f);
			break;
			case 3://YELLOW
				WS2812_OneSet(i, 0x070700);
			break;
			case 4://RED
				WS2812_OneSet(i, 0x0f0000);
			break;
			default:
			break;		
		}
	}
	//输出LED显示
	WS2812_Show();
}

uint32_t get_rand(void)
{
	//time_t t;
	uint32_t RAND=0;
	 /* 初始化随机数发生器 */
	 srand((uint16_t)TIM3->CNT);//time(&t)
	 RAND=rand();
	//RAND/=8;
	 return RAND|=0x00AAAAAA;
}

void ws2812_RandShow(uint32_t Rand,uint8_t num)
{
	
	for(int i=0;i<num;i++)
	{
		WS2812_OneSet(num,Rand);
	}
	WS2812_Show();
}

//extern unsigned char usart_buffer[USART_BUFF_SIZE];
//void update_led_status(void)
//{
//	uint8_t index = 0; 
//	uint8_t *p;
//	p = &usart_buffer[10];
//	for(uint8_t i = 0;i < 64;i++)
//	{
//		led_status[index++] = *p >> 4;
//		led_status[index++] = *p & 0x0f;
//		p++;
//	}
//}



DMA_InitTypeDef DMA_ws2812_InitStruc;

//PB15 SPI2_MOSI DMA1_Channel5
void ws2812_SPI_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_ws2812_InitStruc;
	
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;// GPIO_Mode_IPU
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	SPI_ws2812_InitStruc.SPI_BaudRatePrescaler= SPI_BaudRatePrescaler_16;//2.25Mhz
	SPI_ws2812_InitStruc.SPI_CPHA=  SPI_CPHA_1Edge;
	SPI_ws2812_InitStruc.SPI_CPOL= SPI_CPOL_Low;
	SPI_ws2812_InitStruc.SPI_CRCPolynomial= 7;
	SPI_ws2812_InitStruc.SPI_DataSize= SPI_DataSize_8b;
	SPI_ws2812_InitStruc.SPI_Direction= SPI_Direction_2Lines_FullDuplex;
	SPI_ws2812_InitStruc.SPI_FirstBit= SPI_FirstBit_MSB;
	SPI_ws2812_InitStruc.SPI_Mode= SPI_Mode_Master;
	SPI_ws2812_InitStruc.SPI_NSS= SPI_NSS_Hard;
	
	SPI_Init(SPI2,&SPI_ws2812_InitStruc);
	
	DMA_ws2812_InitStruc.DMA_MemoryInc= DMA_MemoryInc_Enable;
	DMA_ws2812_InitStruc.DMA_MemoryDataSize= DMA_MemoryDataSize_Byte;
	DMA_ws2812_InitStruc.DMA_Mode= DMA_Mode_Normal;
	DMA_ws2812_InitStruc.DMA_PeripheralInc= DMA_PeripheralInc_Disable;
	DMA_ws2812_InitStruc.DMA_PeripheralDataSize= DMA_PeripheralDataSize_Byte;
	DMA_ws2812_InitStruc.DMA_M2M= DMA_M2M_Disable;
	
	
	SPI_I2S_DMACmd(SPI2,SPI_I2S_DMAReq_Tx,ENABLE);
	SPI_Cmd(SPI2,ENABLE);
	
}

void SPI_SendData_DMA(uint8_t* txbuff,uint8_t len)
{
	//DMA_Cmd(DMA1_Channel4,DISABLE);//SIP2_RX
	DMA_Cmd(DMA1_Channel5,DISABLE);//SIP2_TX
	
	DMA_ws2812_InitStruc.DMA_BufferSize= len;
	DMA_ws2812_InitStruc.DMA_DIR= DMA_DIR_PeripheralDST;
	DMA_ws2812_InitStruc.DMA_MemoryBaseAddr= (uint32_t)txbuff;
	DMA_ws2812_InitStruc.DMA_PeripheralBaseAddr= (uint32_t)&SPI2->DR;
	DMA_ws2812_InitStruc.DMA_Priority= DMA_Priority_Medium;
	DMA_Init(DMA1_Channel5,&DMA_ws2812_InitStruc);
	
	//SPI_I2S_DMACmd(SPI2,SPI_I2S_DMAReq_Tx,ENABLE);
	DMA_Cmd(DMA1_Channel5,ENABLE);
//	while(DMA_GetFlagStatus(DMA1_FLAG_TC5) == RESET);
//	DMA_ClearFlag(DMA1_FLAG_TC5);
}





