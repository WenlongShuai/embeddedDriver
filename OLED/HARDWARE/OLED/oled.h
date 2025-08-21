#ifndef __OLED_H
#define __OLED_H

#include "sys.h"
#include "software_iic.h"
#include "hardware_iic.h"
#include "key.h"

//#define SOFTWARE_IIC
/* ���/Ӳ��IIC�л��� 0����� 1��Ӳ�� */
#define IIC_SELECT 1


/* OLED���ý��� */
typedef enum
{
	TIME_SET = 0,  //ʱ������
	BLUETOOTH_SET, //��������
	WIFI_SET,      //WIFI����
}OLED_SET_MENU;

/* OLED��ʾ���� */
typedef enum
{
	MOTOR_LEFT_DIS = 0, //�������ʾ
	MOTOR_RIGHT_DIS,    //�������ʾ
	GPS_DIS,						//GPS��ʾ
	IMU_DIS,						//IMU��ʾ
	CONTROL_MODE_DIS,   //����ģʽ��ʾ
	ALL_DIS,						//ȫ����ʾ
}OLED_DIS_MENU;

/* OLED������ */
typedef enum
{
	SET_MENU = 0,
	DIS_MENU,
}OLED_MAIN_MENU;

typedef struct
{
	uint8_t chooseNum[3][3];
	OLED_MAIN_MENU main_menu;
	OLED_SET_MENU set_menu;
	OLED_DIS_MENU dis_menu;
}OLED_MENU_STRUCT;


#define OLED_ID          0x78
#define OLED_WR_CMD      0x00
#define OLED_WR_DATA     0x40

// ����SSD1306�Դ滺������128��64���أ�ÿҳ8�У���8ҳ��
#define OLED_WIDTH  128
#define OLED_HEIGHT 64
#define OLED_PAGES  (OLED_HEIGHT / 8)  // 8ҳ

void OLED_Init(void);
void OLED_SetPos(unsigned char x, unsigned char y);
void OLED_Fill(unsigned char fill_data);
void OLED_Appoint(unsigned char startX, unsigned char startY,unsigned char width, unsigned char height);
void OLED_CLS(void);
void OLED_ON(void);
void OLED_OFF(void);
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char textsize);
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char n);
void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);
void OLED_UpdateScreen(uint16_t x, uint16_t y, uint16_t w, uint16_t h);

void oled_Main_Menu_Display(void);
void oled_Set_Menu_Display(void);
void oled_Dis_Menu_Display(void);
void keyDoubleClickOLEDDis(void (*doubleClickOperate)(void));


void dis_Menu_Operate(void);
void set_Menu_Operate(void);
void main_Menu_Operate(void);
void set_Time_Menu_Operate(void);
void set_Bluetooth_Menu_Operate(void);
void set_WIFI_Menu_Operate(void);

void dis_MotorLeft_Menu_Operate(void);
void dis_MotorRight_Menu_Operate(void);
void dis_GPS_Menu_Operate(void);
void dis_IMU_Menu_Operate(void);
void dis_ControlMode_Menu_Operate(void);
void dis_All_Menu_Operate(void);

void keyClickOLEDDis(void (*operate)(void));

void setTimeMenuDisplay(void);
void setBluetoothMenuDisplay(void);
void setWIFIMenuDisplay(void);




void disMotorLeftDisplay(void);
void disMotorRightDisplay(void);
void disGPSDisplay(void);
void disIMUDisplay(void);
void disControlModeDisplay(void);
void disALLDisplay(void);


#endif /* __OLED_H */
