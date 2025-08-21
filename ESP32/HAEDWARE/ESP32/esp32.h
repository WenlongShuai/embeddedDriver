#ifndef __HIP600D_H__
#define __HIP600D_H__

#include "sys.h"


//GNSS����
typedef enum
{
	GNGGA,
	GNGSA,
	GPGSV,
	GAGSV,
	GBGSV,
	GNRMC,
	GNVTG,
	GNENU
}GNSSType;

typedef struct
{
	u8 Name[10];  //��������
	float UTC;    //UTCʱ��
	float Latitude;  //γ��
	u8 LatitudeDir; //γ�ȷ���
	float Longitude;  //����
	u8 LongitudeDir;  //���ȷ���
	u8 QualityIndicator;  //GPS ����ָʾ��
	u8 SatellitesNum;   //��������
	float HorizontalDilution;  //ˮƽ��������
	float mean_sea_leve;  //ƽ����ƽ��
	u8 mean_sea_leveUnit; //ƽ����ƽ�浥λ��M��
	float GeoidalSeparation;  //ƽ����ƽ����
	u8 GeoidalSeparationUnit;  //ƽ����ƽ���൥λ
	float DifferentialGPSData;  //���GPS����
	u8 DifferentialReferenceID;  //��ֲο�ID
}GNGGAStruct;
	
typedef struct
{
	u8 Name[10];  //��������
	u8 Mode1;  //ģʽ���л�2D/3D
	u8 Mode2;  //ģʽ��2D/3D
	u8 IDNum[12];  //���������ʹ�õ����ǵ�ID��
	float PDOP;
	float HDOP;
	float VDOP;
	u8 GNSSSystemID;  //GNSSϵͳID
}GNGSAStruct;

typedef struct
{
	u8 Name[10];  //��������
	u8 TotalNum;  //����
	u8 SentenceNum;  //��������
	u16 SatellitesTotalNum;  //��������
	u16 SatelliteID;  //����ʶ����
	u16 ElevationAngle;  //���ǣ����90��
	u8 AzimuthAngle;  //��λ�ǣ�0-359
	u8 SNR;  //�����
	u8 SV1[3];
	u8 SV2[4];
	u8 SignalID;  //�ź�ID
}GP_GA_GBGSVStruct;

typedef struct
{
	u8 Name[10];
	u8 UTC[11];
	u8 Status;  //��λ״̬
	double Latitude;  //γ��
	u8 LatitudeDir;   //γ�ȷ���
	double Longitude;  //����
	u8 LongitudeDir; //���ȷ���
	float SpeedOverGround; //�����ٶ�
	float CourseOverGround;  //���溽��
	u8 Date[6];  //���ڣ�������
	float MagneticVariation; //��ƫ��
	u8 MagneticVariationDir; //��ƫ�Ƿ���
	u8 ModeIndicator;  //ģʽָʾ
	u8 NavigationalStatus;  //����״̬
}GNRMCStruct;

typedef struct
{
	u8 Name[10];
	float CourseGround1;  //���溽��
	u8 DegreesTrue;   //����
	float CourseGround2;  //���溽��
	u8 DegreesMagnetic;   //�Ŷ�
	float SpeedGround1;  //�Ե��ٶ�
	u8 SpeedGroundUnit1;  //��
	float SpeedGround2;   //�����ٶ�
	u8 SpeedGroundUnit2;   //km/hr 
	u8 ModeIndicator;  //״ָ̬ʾ��
}GNVTGStruct;


#define HIP_USARTx 					USART3
#define HIP_USART_RCC				RCC_APB1Periph_USART3
#define HIP_USART_RCC_GPIO	RCC_AHB1Periph_GPIOB

#define HIP_USART_RX_GPIO 	GPIOB
#define HIP_USART_RX_PIN		GPIO_Pin_11
#define HIP_RX_PinSourcex 	GPIO_PinSource11
#define HIP_RX_AF 					GPIO_AF_USART3

#define HIP_USART_TX_GPIO		GPIOB
#define HIP_USART_TX_PIN		GPIO_Pin_10
#define HIP_TX_PinSourcex 	GPIO_PinSource10
#define HIP_TX_AF 					GPIO_AF_USART3

#define HIP_USART_IRQn			USART3_IRQn

#define HIP_USART_REC_LEN  			128  	//�����������ֽ��� 200

extern u8  HIP_USART_RX_BUF[HIP_USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
//����״̬
extern u8 HIP_USART_RX_STA_LINE;    //һ�����ݱ�־	
extern u8 HIP_USART_RX_STA_FRAM;   //һ֡���ݱ�־
extern u16 HIP_USART_RX_STA_COUNT;

void hip600d_USART_GPIO_Init(uint32_t bound);

void parseNavigationData(u8 *dataStr);
void mySplitStr(u8 symbol, u8 *dataStr, u8 (*splitStrList)[20], u8 *splitCount);
int myFindString(u8 *dataStr, u8 *subStr, u8 *targetStr);

#endif /* __HIP600D_H__ */
