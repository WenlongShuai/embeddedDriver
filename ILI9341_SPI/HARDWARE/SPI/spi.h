#ifndef __SPI_H__
#define __SPI_H__

#include "sys.h"

//-----------------------------SPI ��������--------------------------------------//
#define USE_HARDWARE_SPI     0  //1:Enable Hardware SPI;0:USE Soft SPI


//�����Գ���ʹ�õ���ģ��SPI�ӿ�����
//�����ɸ��Ľӿ�IO���ã�ʹ����������4 IO������ɱ���Һ��������ʾ
/******************************************************************************
�ӿڶ�����Lcd_Driver.h�ڶ��壬����ݽ����޸Ĳ��޸���ӦIO��ʼ��LCD_GPIO_Init()
#define LCD_CTRL   	  	GPIOB		//����TFT���ݶ˿�
#define LCD_LED        	GPIO_Pin_9  //PB9 ������TFT -LED
#define LCD_RS         	GPIO_Pin_10	//PB10������TFT --RS
#define LCD_CS        	GPIO_Pin_11 //PB11 ������TFT --CS
#define LCD_RST     	  GPIO_Pin_12	//PB12������TFT --RST
#define LCD_SCL        	GPIO_Pin_13	//PB13������TFT -- CLK
#define LCD_SDA        	GPIO_Pin_15	//PB15������TFT - SDI
*******************************************************************************/
#define LCD_CTRL   	  	GPIOB		//����TFT���ݶ˿�
#define LCD_LED        	GPIO_Pin_9  //MCU_PB9--->>TFT --BL
#define LCD_RS         	GPIO_Pin_10	//PB11--->>TFT --RS/DC
#define LCD_CS        	GPIO_Pin_11 //MCU_PB11--->>TFT --CS/CE
#define LCD_RST     		GPIO_Pin_12	//PB10--->>TFT --RST
#define LCD_SCL        	GPIO_Pin_13	//PB13--->>TFT --SCL/SCK
#define LCD_SDA        	GPIO_Pin_15	//PB15 MOSI--->>TFT --SDA/DIN


//#define LCD_CS_SET(x) LCD_CTRL->ODR=(LCD_CTRL->ODR&~LCD_CS)|(x ? LCD_CS:0)

//Һ�����ƿ���1�������궨��
#define	LCD_CS_SET  	LCD_CTRL->BSRRL|=LCD_CS    
#define	LCD_RS_SET  	LCD_CTRL->BSRRL|=LCD_RS    
#define	LCD_SDA_SET  	LCD_CTRL->BSRRL|=LCD_SDA    
#define	LCD_SCL_SET  	LCD_CTRL->BSRRL|=LCD_SCL    
#define	LCD_RST_SET  	LCD_CTRL->BSRRL|=LCD_RST    
#define	LCD_LED_SET  	LCD_CTRL->BSRRL|=LCD_LED   

//Һ�����ƿ���0�������궨��
#define	LCD_CS_CLR  	LCD_CTRL->BSRRH|=LCD_CS    
#define	LCD_RS_CLR  	LCD_CTRL->BSRRH|=LCD_RS    
#define	LCD_SDA_CLR  	LCD_CTRL->BSRRH|=LCD_SDA    
#define	LCD_SCL_CLR  	LCD_CTRL->BSRRH|=LCD_SCL    
#define	LCD_RST_CLR  	LCD_CTRL->BSRRH|=LCD_RST    
#define	LCD_LED_CLR  	LCD_CTRL->BSRRH|=LCD_LED 


#define LCD_DATAOUT(x) LCD_DATA->ODR=x; //�������
#define LCD_DATAIN     LCD_DATA->IDR;   //��������

#define LCD_WR_DATA(data){\
LCD_RS_SET;\
LCD_CS_CLR;\
LCD_DATAOUT(data);\
LCD_WR_CLR;\
LCD_WR_SET;\
LCD_CS_SET;\
} 

void LCD_GPIO_Init(void);
void  SPIv_WriteData(uint8_t Data);
u8 SPI_WriteByte(SPI_TypeDef* SPIx,u8 Byte);
void SPI_SetSpeed(SPI_TypeDef* SPIx,u8 SpeedSet);
void SPI2_Init(void);

#endif /* __SPI_H__ */
