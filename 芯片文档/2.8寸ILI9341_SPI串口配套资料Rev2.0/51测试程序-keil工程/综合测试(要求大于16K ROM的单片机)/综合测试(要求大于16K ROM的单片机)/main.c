#include <reg51.h>
#include <intrins.h>
#include <sys\sys.h>
#include <lcd\lcd.h>
#include <font\font.h>
#include <touch\touch.h>
/* 
������Ի���:��Ƭ��STC12LE5A60S2,����33M  ��Ƭ��������ѹ3.3V
����Ĭ��IO���ӷ�ʽ��
sbit LCD_CS     =P0^0;     //Ƭѡ	
sbit LCD_DC     =P0^2;	  //����/�����л�
sbit LCD_SDI    =P1^5;	  //����
sbit LCD_SCK    =P1^7;	  //ʱ��
sbit LCD_REST   =P0^1;	  //��λ  
�����������ӷ�ʽ��(��ʹ�ô����ɲ�����)
T_CLK-P1^7;  T_CS-P1^4;  T_DIN-P3^0;  T_OUT-P3^1;  T_IRQ-P3^4;
*/		
sbit key1=P3^2;
sbit key2=P3^3;
sbit key3=P3^4;
u8 ref=0;//ˢ����ʾ

u8 jiance() //��ⴥ���Ͱ���
{
	if (tpstate()==0) //����������£�������ͼ����
	 {
	 	LCD_Clear(WHITE); //����
		BACK_COLOR=WHITE;
		POINT_COLOR=RED;
	 	point();
		return 1;
	 }	
	if(key1==0)	//�������1����,����У׼���� 
	   {
		Touch_Adjust();//У׼
		return 1;
	   }
	 if(key2==0)	//��ɫˢ��
	   {
		 LCD_Clear(RED);
		 delayms(3000);
		 LCD_Clear(GREEN);
		 delayms(3000);
		 LCD_Clear(BLUE);
		 delayms(3000);
		 return 1;
	   }
	   return 0;
}
void xianshi()//��ʾ��Ϣ
{   
	u16 lx,ly;
	BACK_COLOR=WHITE;
	POINT_COLOR=RED;	
	//��ʾ32*32����
	showhanzi32(0,0,0);	 //��
	showhanzi32(40,0,1); //��
	showhanzi32(80,0,2); //��
	//��ʾ16*16����
	showhanzi16(0,35,0);	  //ר
	showhanzi16(20,35,1);	  //ע
	showhanzi16(40,35,2);	  //��
	showhanzi16(60,35,3);	  //ʾ
	showhanzi16(80,35,4);	  //��
	showhanzi16(100,35,5);	  //��
    LCD_ShowString(0,55,"2.8 TFT SPI 240*320");
	lx=0;ly=75;			
	LCD_ShowString(lx,ly,"VX:");lx+=40;LCD_ShowNum(lx,ly,vx,5);					
	lx+=60;
	LCD_ShowString(lx,ly,"Vy:");lx+=40;LCD_ShowNum(lx,ly,vy,5);					
	lx=0;ly=95; 
	LCD_ShowString(lx,ly,"CHX:");lx+=40;LCD_ShowNum(lx,ly,chx,5);					
	lx+=60; 
	LCD_ShowString(lx,ly,"CHY:");lx+=40;LCD_ShowNum(lx,ly,chy,5);
}
void showimage() //��ʾ40*40ͼƬ
{
  	int i,j,k; 
	LCD_Clear(WHITE); //����  
	xianshi(); //��ʾ��Ϣ
	for(k=3;k<8;k++)
	{
	   	for(j=0;j<6;j++)
		{	
			Address_set(40*j,40*k,40*j+39,40*k+39);		//��������
		    for(i=0;i<1600;i++)
			 { 	
				 if(jiance()) //��ⴥ���Ͱ���
				 {	
				 	ref=1;
				  	return;	
				 }		
			  	 LCD_WR_DATA8(image[i*2+1]);	 
				 LCD_WR_DATA8(image[i*2]);				
			 }	
		 }
	}
	ref=0;				
}
main()
{ 
	spistar()  ;//ģ��SPI��ʼ��
	Lcd_Init();   //tft��ʼ��
	LCD_Clear(WHITE); //����
	BACK_COLOR=BLACK;;POINT_COLOR=WHITE; 
    showimage(); //��ʾ40*40ͼƬ

	while(1)
	{
	    if(jiance()) //��ⴥ���Ͱ���
	  {	
	  	showimage(); //��ʾ40*40ͼƬ
	  }
	  if(ref){showimage();} //��ʾ40*40ͼƬ

    }


}
