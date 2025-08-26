#include "MS5611.h"
#include "GY86_IIC.h"
#include "sys.h"
#include "delay.h"
#include "math.h"

uint16_t Cal_C[8];                                                                 //���ڴ��PROM�е�6������ 
int32_t  Temperature;                                                              //��ǰʵ���¶�
float dT;                                                                          //ʵ�ʺͲο��¶�֮��Ĳ��� 
float Pressure=0;                                                             //��ǰʵ�ʴ���ѹֵ
 float  H=0,a,b,VREF,altitude,altitude1;                                                                   //��ǰ�߶ȣ��ף�
unsigned char i=1,c=20;
	
//**********MS5611��ʼ������Ҫ��ȡ����У׼����������У׼�¶Ⱥ���ѹ����************************//
void Init_MS561101BA()
{
  u8 data[2];
	u8 i=0;
	
  GY86_Write_commendByte(MS561101BA_Addr,MS561101BA_RST);	                         //MS5611��λ
	delay_ms(100); 
	for(i=0;i<7;i++)                                                                 //��ȡ����У׼���ݣ������飬������
	{
	GY86_Read_Byte(MS561101BA_Addr,(MS561101BA_PROM_RD+i*2),2,data);            
	Cal_C[i]=data[0]<<8|data[1];
	}
	delay_ms(100); 
}

//**********��ȡMS5611���¶�ԭʼ���ݲ������ʵ���¶ȣ����ڶ���У׼��ѹ���¶Ȳ���**************//
void MS561101BA_GetTemperature(u8 OSR_Temp)
{
	 uint32_t D2_Temp;                                                               // ��������¶� ����

	 D2_Temp= MS561101BA_DO_CONVERSION(OSR_Temp);
//	 dT=D2_Temp - (Cal_C[5]*256);  
//   Temperature=2000+(dT*(Cal_C[6]/8388608));  //����¶�ֵ��100��������2001��ʾ20.01��
	
	dT=D2_Temp - ((uint32_t)Cal_C[5]*pow(2,8));                                      //�ο�Ӣ�������ֲ�Page 7
	Temperature=2000+((dT*Cal_C[6])/pow(2,23));                                      //��ʽTEMP=2000+dT*C6/8388608
	
}


//*************************��ȡ����ѹ��ֵ�����ö����¶Ȳ�������ʵʱѹ��**********************//
u8 MS561101BA_getPressure(u8 OSR_Temp) 
{
  uint32_t  D1_Pres;                                                               //���������ѹ����
	int64_t OFF, SENS;
  signed int  Temperature2;  
	const double T1 = 15.0 + 273.15;	/* temperature at base height in Kelvin */
	const double a  = -6.5 / 1000;	/* temperature gradient in degrees per metre */
	const double g  = 9.80665;	/* gravity constant in m/s/s */
	const double R  = 287.05;	/* ideal gas constant in J/kg/K */
	double p1 = 101325 / 1000.0;/* the _msl_pressure  default value is 101325 */
		/* measured pressure in kPa */
		double p =0.0;
	float q ,w ,e ,r;
	
	u32 Aux ,Aux2,OFF2,SENS2;	
	D1_Pres= MS561101BA_DO_CONVERSION(OSR_Temp);                                     //��ȡ������ѹ����
	OFF=Cal_C[2]*pow(2,16)+((Cal_C[4]*dT)/pow(2,7));                                 //����ʵ���¶ȵ�������
	SENS=(Cal_C[1]*pow(2,15))+((Cal_C[3]*dT)/pow(2,8));                              //����ʵ���¶�����������
	
 /*��ʼ�����¶Ȳ�����ѹ*/
 if(Temperature<2000)                                                              //�Ƿ��ǵ��£�<20.00�棩                                   
 {
  Temperature2 = (dT*dT)/pow(2,31); 
  Aux = (Temperature-2000)*(Temperature-2000);  
  OFF2 = 5*Aux/2; 
  SENS2 = 1.25*Aux; 
	 if(Temperature<-1500)                                                           //�Ƿ�طǳ��͵��¶ȣ�<-15.00�棩
	 {
		 Aux2 = (Temperature+1500)*(Temperature+1500);
	   OFF2 = OFF2+7*Aux2;
		 SENS2 = SENS+11*Aux2/2;
	 }
     Temperature = (Temperature - Temperature2);                                   //����ʵ���¶�
     OFF = OFF - OFF2;                                                             //�����¶ȵ�������
     SENS = SENS - SENS2;                                                          //�����¶�������
  } 
 Pressure=((D1_Pres*SENS)/pow(2,21)-OFF)/pow(2,15);                                //(������100��)�¶Ȳ�����Ĵ���ѹ������λmbar(1mbar=100pa)
	
	p=Pressure/1000;
	//altitude=(((pow((), (-(a * R) / g))) ) - T1) / a;
	q=p / p1;
	w=a * R/ g;
	e=pow(q,-w);
	r=e* T1-T1;
	altitude=r/a;
	
	//altitude (44330.0f*(1.0f - pow((float)Pressure/101325.0f, 0.190295f))) ;
//  a= Pressure/101325.0;                                                               //�򵥼��㵱ǰ�߶�
//	b= pow(a, 0.190295);
//	if (i==1)
//	{
//	   i=0;
//		VREF= 44330 * (1.0 - b);
//	}
//	if(c>0)
//	{
//	c--;
//	H= 44330 * (1.0 - b)-VREF;
//		altitude1=altitude1+H;
//		if(c==0)
//		{
//			c=20;
//		altitude=altitude1/20;
//			altitude1=0;
//		}
//	}


	
 return 0;
}



//************************************����ADת������ȡ����*********************************//
uint32_t MS561101BA_DO_CONVERSION(uint8_t command)  
{  
   u8 conversiontable[3];                                                    //����²�DT���ݵ�����
   u8 conversion; 	                                                         //����²�DT�ı���
  
   GY86_Write_commendByte(MS561101BA_Addr,command);                                //��ADת�����������      
   delay_ms(20);                                                                   //��ʱ,ȥ�����ݴ���  
   GY86_Read_Byte(MS561101BA_Addr,0x00,3,conversiontable);                         //��ADת�����
   conversion=conversiontable[0]<<16|conversiontable[1]<<8|conversiontable[2];     //������ȡ���ݺϲ�
   return conversion;  
  
}  

