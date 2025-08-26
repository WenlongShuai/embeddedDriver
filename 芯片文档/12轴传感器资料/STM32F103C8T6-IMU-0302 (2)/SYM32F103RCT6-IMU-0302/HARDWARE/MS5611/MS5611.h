#include "sys.h"

#define  MS561101BA_Addr                0xEE          //������ַ
#define  MS561101BA_RST                 0x1E          //��λָ��
#define  MS561101BA_PROM_RD             0xA0          //PROM��ʼ��ַ(����У׼���ݴ�ŵ�λ��)
#define  MS561101BA_PROM_CRC            0xAE          //У��ָ���ŵĵ�ַ

//**********************ADCͨ������ת����ַ***************************//
#define  MS561101BA_OSR_256             0x00          //8λ�ֱ���ת�����Ƶ�ַ
#define  MS561101BA_OSR_512             0x02          //9λ�ֱ���ת�����Ƶ�ַ
#define  MS561101BA_OSR_1024            0x04          //10λ�ֱ���ת�����Ƶ�ַ
#define  MS561101BA_OSR_2048            0x06          //11λ�ֱ���ת�����Ƶ�ַ
#define  MS561101BA_OSR_4096            0x08          //12λ�ֱ���ת�����Ƶ�ַ
//**********************����ѹ���Ĵ�����ַ****************************//
#define  MS561101BA_D1_OSR_256          0x40          //8λ�ֱ��ʵ�ַ
#define  MS561101BA_D1_OSR_512          0x42          //9λ�ֱ��ʵ�ַ
#define  MS561101BA_D1_OSR_1024         0x44          //10λ�ֱ��ʵ�ַ
#define  MS561101BA_D1_OSR_2048         0x46          //11λ�ֱ��ʵ�ַ
#define  MS561101BA_D1_OSR_4096         0x48          //12λ�ֱ��ʵ�ַ
//**********************�����¶ȼĴ�����ַ****************************//
#define  MS561101BA_D2_OSR_256          0x50          //8λ�ֱ��ʵ�ַ
#define  MS561101BA_D2_OSR_512          0x52          //9λ�ֱ��ʵ�ַ
#define  MS561101BA_D2_OSR_1024         0x54          //10λ�ֱ��ʵ�ַ
#define  MS561101BA_D2_OSR_2048         0x56          //11λ�ֱ��ʵ�ַ
#define  MS561101BA_D2_OSR_4096         0x58          //12λ�ֱ��ʵ�ַ



void Init_MS561101BA(void);
void MS561101BA_GetTemperature(u8 OSR_Temp);
uint32_t MS561101BA_DO_CONVERSION(uint8_t command);
u8 MS561101BA_getPressure(u8 OSR_Temp);
