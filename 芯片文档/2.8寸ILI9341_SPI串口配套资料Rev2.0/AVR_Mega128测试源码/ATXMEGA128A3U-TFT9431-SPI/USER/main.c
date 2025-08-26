#include <avr/io.h>  
#include "clksys_driver.h"
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "TFT_Driver.h"
#include "spi_driver.h"

unsigned char Num[10]={0,1,2,3,4,5,6,7,8,9};

//����ϵͳʱ�� 32M
//----------------------------------------------------------------------------
void Sysclk_Init(void)
{
		/*  Enable internal 32 MHz ring oscillator and wait until it's
		 *  stable. Divide clock by two with the prescaler C and set the 16M
		 *  32 MHz ring oscillator as the main clock source. 
		 */
		CLKSYS_Enable( OSC_RC32MEN_bm );
		CLKSYS_Prescalers_Config( CLK_PSADIV_1_gc, CLK_PSBCDIV_1_1_gc );
		do {} while ( CLKSYS_IsReady( OSC_RC32MRDY_bm ) == 0 );
		CLKSYS_Main_ClockSource_Select( CLK_SCLKSEL_RC32M_gc );
}


void Redraw_Mainmenu(void)
{
	TFT_Clear(GRAY0);
	TFT_DrawFont_GBK16(16,0,BLUE,GRAY0,"ȫ�����Ӽ���");
	TFT_DrawFont_GBK16(16,20,RED,GRAY0,"Һ�����Գ���");
	DisplayButtonDown(15,38,113,58); //x1,y1,x2,y2
	TFT_DrawFont_GBK16(16,40,YELLOW,GRAY0,"��ɫ������");
	DisplayButtonUp(15,68,113,88); //x1,y1,x2,y2
	TFT_DrawFont_GBK16(16,70,BLUE,GRAY0,"������ʾ����");
	DisplayButtonUp(15,98,113,118); //x1,y1,x2,y2
	TFT_DrawFont_GBK16(16,100,RED,GRAY0,"ͼƬ��ʾ����");
	_delay_ms(1000);
	TFT_DrawFont_Num32(100,125,RED,GRAY0,Num[5]);
	_delay_ms(1000);
	TFT_DrawFont_Num32(100,125,RED,GRAY0,Num[4]);
	_delay_ms(1000);
	TFT_DrawFont_Num32(100,125,RED,GRAY0,Num[3]);
	_delay_ms(1000);
	TFT_DrawFont_Num32(100,125,RED,GRAY0,Num[2]);
	_delay_ms(1000);
	TFT_DrawFont_Num32(100,125,RED,GRAY0,Num[1]);
	_delay_ms(1000);
	TFT_DrawFont_Num32(100,125,RED,GRAY0,Num[0]);	
}

void Num_Test(void)
{
	u8 i=0;
	TFT_Clear(GRAY0);
	TFT_DrawFont_GBK16(16,20,RED,GRAY0,"Num Test");
	_delay_ms(1000);
	TFT_Clear(GRAY0);

	for(i=0;i<10;i++)
	{
	TFT_DrawFont_Num32((i%3)*40,32*(i/3)+30,RED,GRAY0,Num[i+1]);
	_delay_ms(100);
	}
	_delay_ms(1000);
}

void Font_Test(void)
{
	u8 x_offset;
#ifdef H_VIEW  //��������˺�����ʾ
	x_offset=40;
#else
	x_offset=0;
#endif
	TFT_Clear(GRAY0);
	TFT_DrawFont_GBK16(16,10,BLUE,GRAY0,  "   ������ʾ����   ");

	_delay_ms(1000);
	TFT_Clear(WHITE);
	TFT_DrawFont_GBK16(x_offset,10,RED,WHITE,  "     ȫ�����Ӽ������޹�˾      ");
	TFT_DrawFont_GBK16(x_offset,30,RED,WHITE,  "         QDtech .,LTD         ");
	TFT_DrawFont_GBK16(x_offset,100,GREEN,WHITE,   "     ȫ�̼���֧��     ");	
	TFT_DrawFont_GBK16(x_offset,120,RED,YELLOW,   "http://qdtech.taobao.com");
	TFT_DrawFont_GBK16(x_offset,140,RED,YELLOW,   "E-Mail:QDtech2008@gmail.com");
	TFT_DrawFont_GBK16(x_offset,160,RED,YELLOW,   "��������Ⱥ:324828016");	
	TFT_DrawFont_GBK16(x_offset,180,BLUE,WHITE,   "Tel:15989313508 ");
	_delay_ms(3000);	
	

}

void Color_Test(void)
{
	u8 i=2;
	TFT_Clear(GRAY0);
	
	TFT_DrawFont_GBK16(20,10,BLUE,GRAY0,"Color Test");
	_delay_ms(1000);

	while(i--)
	{
	TFT_Clear(WHITE);
	TFT_Clear(BLACK);
	TFT_Clear(RED);
  	TFT_Clear(GREEN);
  	TFT_Clear(BLUE);
	}		
}


int main(void)
{
	Sysclk_Init();

	//�轫PD�˿ڵ�SPI�ģ�SS��MOSI��SCK����Ϊ���
	PORTD.DIR |= (1<<4)|(1<<5)|(1<<7);
	PORTD.OUT |= (1<<4)|(1<<5)|(1<<7);
	PORTD.DIR &=~ (1<<6); //MISO����Ϊ����

	SPI_Master_t spiMasterD;
	SPI_MasterInit(&spiMasterD,
	               &SPID,
	               &PORTD,
	               false,
	               SPI_MODE_0_gc,
	               SPI_INTLVL_OFF_gc,
	               false,
	               SPI_PRESCALER_DIV4_gc);

	//TFT��LED-PB0��CS-PB1��RS-PB2 RST-PB3
	PORTB.DIR |= (1<<0)|(1<<1)|(1<<2)|(1<<3);
	PORTB.OUT |= (1<<0)|(1<<1)|(1<<2)|(1<<3);
	//LED��������ָʾ
	PORTC.DIR = 0x80;
	PORTC.OUT = 0x80;

	TFT_Init();	
	//TFT_LED_CLR;//��������
	TFT_Clear(WHITE);
	TFT_LED_SET;
  while(1) 
  {
		TFT_Init();			
		TFT_Clear(WHITE);
		_delay_ms(1000);
		Redraw_Mainmenu();//�򵥲˵�����
		TFT_Clear(WHITE);
		_delay_ms(1000);
	TFT_Init();	
		Color_Test();//��ˢ������
		TFT_Clear(WHITE);
		_delay_ms(1000);
	TFT_Init();	
		Num_Test();//�����������ʾ
		TFT_Clear(WHITE);
		_delay_ms(1000);
	TFT_Init();	
		Font_Test();//��Ӣ�Ĳ���  			
		TFT_Clear(WHITE);  
		_delay_ms(1000);
	TFT_Init();	
		TFT_DrawFont_GBK16(10,50,BLUE,GRAY0,"���Գɹ�.");
		TFT_Clear(WHITE); 
		_delay_ms(1000);
  }
 
}

