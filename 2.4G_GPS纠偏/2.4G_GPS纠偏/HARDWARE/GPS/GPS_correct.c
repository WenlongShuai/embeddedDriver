/*
 WGS-84：是国际标准，GPS坐标（Google Earth使用、或者GPS模块）
 GCJ-02(火星坐标)：中国坐标偏移标准，Google Map、高德、腾讯使用
 BD-09：百度坐标偏移标准，Baidu Map使用
 pi: 圆周率。
 a: 卫星椭球坐标投影到平面地图坐标系的投影因子。
 ee: 椭球的偏心率。
 x_pi: 圆周率转换量。
 transformLat(double x, double y): 坐标转换方法。
 transformLon(double x, double y): 坐标转换方法。
 wgs2gcj(double lat, double lon, double* pLat, double* pLon): WGS坐标转换为GCJ坐标。
 gcj2bd(double lat, double lon, double* pLat, double* pLon): GCJ坐标转换为百度坐标。
*/
 
#include "GPS_correct.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>


/*
 pi: 圆周率。
 a: 卫星椭球坐标投影到平面地图坐标系的投影因子。
 ee: 椭球的偏心率。
 x_pi: 圆周率转换量。
*/
 /*
 经度:longitude
 纬度:latitude
	
 */
double pi = 3.14159265358979324;		
double a = 6378245.0;
double ee = 0.00669342162296594323;
double x_pi = 3.14159265358979324 * 3000.0 / 180.0;
 

double *OffSet(double lat, double lon);	// 偏移量
double transformLat(double x, double y);// 纬度偏移量
double transformLon(double x, double y);// 经度偏移量

// WGS84=>BD09 地球坐标系=>百度坐标系 
int wgs2bd(double lat, double lon, double* pLat, double* pLon) {
   double lat_ = 0.0, lon_ = 0.0;
   wgs2gcj(lat, lon, &lat_, &lon_);
   gcj2bd(lat_, lon_,  pLat, pLon);
   return 0;
}
 
// GCJ02=>BD09 火星坐标系=>百度坐标系  
int gcj2bd(double lat, double lon, double* pLat, double* pLon) {
   double x = lon, y = lat;
   double z = sqrt(x * x + y * y) + 0.00002 * sin(y * x_pi);
   double theta = atan2(y, x) + 0.000003 * cos(x * x_pi);
   *pLon = z * cos(theta) + 0.0065;
   *pLat = z * sin(theta) + 0.006;
   return 0;
}
 
// BD09=>GCJ02 百度坐标系=>火星坐标系 
int bd2gcj(double lat, double lon, double* pLat, double* pLon) {
   double x = lon - 0.0065, y = lat - 0.006;
   double z = sqrt(x * x + y * y) - 0.00002 * sin(y * x_pi);
   double theta = atan2(y, x) - 0.000003 * cos(x * x_pi);
   *pLon = z * cos(theta);
   *pLat = z * sin(theta);
   return 0;
}
 
// WGS84=>GCJ02 地球坐标系=>火星坐标系
int wgs2gcj(double lat, double lon, double* pLat, double* pLon) {
    if (outOfChina(lat,lon))
    {
        *pLat = lat;
        *pLon = lon;
        return 0;
    }
   double dLat = transformLat(lon - 105.0, lat - 35.0);
   double dLon = transformLon(lon - 105.0, lat - 35.0);
   double radLat = lat / 180.0 * pi;
   double magic = sin(radLat);
   magic = 1 - ee * magic * magic;
   double sqrtMagic = sqrt(magic);
   dLat = (dLat * 180.0) / ((a * (1 - ee)) / (magic * sqrtMagic) * pi);
   dLon = (dLon * 180.0) / (a / sqrtMagic * cos(radLat) * pi);
   *pLat = lat + dLat;
   *pLon = lon + dLon;
   return 0;
}
 
// GCJ02=>WGS84 火星坐标系=>地球坐标系(粗略)
int gcj2wgs(double lat, double lon, double* pLat, double* pLon) {
    if (outOfChina(lat,lon))
    {
        *pLat = lat;
        *pLon = lon;
        return 0;
    }
	double *offset;
	offset = OffSet(lat,lon);
	*pLat = lat - offset[0];
 	*pLon = lon - offset[1];
	return 0;
}
 
// GCJ02=>WGS84 火星坐标系=>地球坐标系（精确）
int gcj2wgs_Exactly(double gcjlat, double gcjlon, double* wgs_Lat, double* wgs_lon) {
    if (outOfChina(gcjlat,gcjlon))
    {
        *wgs_Lat = gcjlat;
        *wgs_lon = gcjlon;
        return 0;
    }
	double initDelta = 0.01;
	double threshold = 0.000000001;
	double dLat = initDelta, dLon = initDelta;
	double mLat = gcjlat - dLat, mLon = gcjlon - dLon;
	double pLat = gcjlat + dLat, pLon = gcjlon + dLon;
	double wgsLat = 0.0, wgslon = 0.0, i = 0.0 ,newgcjlat = 0.0,newgcjlon = 0.0;
 
	while (1) {
		wgsLat = (mLat + pLat) / 2;
		wgslon = (mLon + pLon) / 2;		
		wgs2gcj(wgsLat,wgslon,&newgcjlat,&newgcjlon);
		dLon = newgcjlon - gcjlon;
		dLat = newgcjlat - gcjlat;
		if ((fabs(dLat) < threshold) && (fabs(dLon) < threshold))
			break;
							
		if (dLat > 0)
			pLat = wgsLat;
		else
			mLat = wgsLat;
		if (dLon > 0)
			pLon = wgslon;
		else
			mLon = wgslon;
 
		if (++i > 10000)
			break;
	}
	*wgs_Lat = wgsLat;
	*wgs_lon = wgslon;
	return 0;
}
 
// BD09=>WGS84 百度坐标系=>地球坐标系(粗略)
int bd2wgs(double lat, double lon, double* pLat, double* pLon) {
	double lat_ = 0.0, lon_ = 0.0;
 	bd2gcj(lat, lon, &lat_, &lon_);
 	gcj2wgs(lat_, lon_,  pLat, pLon);
 	return 0;
}
 
// BD09=>WGS84 百度坐标系=>地球坐标系(精确)
int bd2wgs_Exactly(double lat, double lon, double* pLat, double* pLon) {
	double lat_ = 0.0, lon_ = 0.0;
 	bd2gcj(lat, lon, &lat_, &lon_);
 	gcj2wgs_Exactly(lat_, lon_,  pLat, pLon);
 	return 0;
}
 
 
 
// 偏移量
double *OffSet(double lat, double lon) {
        double Latlon[2] = {0.0,0.0};
        double dLat = transformLat(lon - 105.0, lat - 35.0);
        double dLon = transformLon(lon - 105.0, lat - 35.0);
        double radLat = lat / 180.0 * pi;
        double magic = sin(radLat);
        magic = 1 - ee * magic * magic;
        double sqrtMagic = sqrt(magic);
        dLat = (dLat * 180.0) / ((a * (1 - ee)) / (magic * sqrtMagic) * pi);
        dLon = (dLon * 180.0) / (a / sqrtMagic * cos(radLat) * pi);
		Latlon[0] = dLat;
		Latlon[1] = dLon;
		return Latlon;
	}
 
// 纬度偏移量
double transformLat(double x, double y) {
       double ret = 0.0;
       ret = -100.0 + 2.0 * x + 3.0 * y + 0.2 * y * y + 0.1 * x * y + 0.2 * sqrt(fabs(x));
       ret += (20.0 * sin(6.0 * x * pi) + 20.0 * sin(2.0 * x * pi)) * 2.0 / 3.0;
       ret += (20.0 * sin(y * pi) + 40.0 * sin(y / 3.0 * pi)) * 2.0 / 3.0;
       ret += (160.0 * sin(y / 12.0 * pi) + 320 * sin(y * pi  / 30.0)) * 2.0 / 3.0;
       return ret;
}
 
// 经度偏移量
double transformLon(double x, double y) {
       double ret = 0.0;
       ret = 300.0 + x + 2.0 * y + 0.1 * x * x + 0.1 * x * y + 0.1 * sqrt(fabs(x));
       ret += (20.0 * sin(6.0 * x * pi) + 20.0 * sin(2.0 * x * pi)) * 2.0 / 3.0;
       ret += (20.0 * sin(x * pi) + 40.0 * sin(x / 3.0 * pi)) * 2.0 / 3.0;
       ret += (150.0 * sin(x / 12.0 * pi) + 300.0 * sin(x / 30.0 * pi)) * 2.0 / 3.0;
       return ret;
}
	
char outOfChina(double lat, double lon)
{
	if((3.51<lat && lat<53.33) && (73.33<lon && lon<135.05))
		return 0;
	else 
		return 1;
}


/*** GPS_NMEA数据处理 ***/
/* 获取GGA字段 */
void nmeagga(char *data ,uint8_t buf[],uint16_t Len)
{
	char * gngga_adr=NULL;
	memset(buf,0,MAXRAWLEN);
	
	gngga_adr = strstr(data ,"$GNGGA");
	if(gngga_adr != NULL)
	{
		uint8_t cnt=0;
		do{	
			buf[cnt++] = *gngga_adr++;
		}while(*gngga_adr != '*');//一个字段结束后会有一个回车换行0x0d,0x0a,然后是下一字段的起始符'$'
		/* *后面跟两位校验码 */
		buf[cnt++] = *gngga_adr++;// '*'
		buf[cnt++] = *gngga_adr++;
		buf[cnt++] = *gngga_adr++;
	}

}
/* GGA字段数据拆分 */
char decode_nmea(nmea_raw_t *raw)
{
		char *p,*q,*val[MAXFIELD], sum, sum2;
    int n=0;
	
    /* 校验和 $....*之间的内容  */
    for ( q = (char*)(raw->buf + 1),sum=0; *q && *q != '*';q++) sum^=*q;
					q++; sum2 = (int)strtol(q, NULL, 16);
    
    if(sum != sum2)
    {
        //NL_TRACE("nmea checksum failed %02X %02X\n",sum, sum2);
        return 1;
    }
		
		   /* 解析字段 */
    for (p=(char*)raw->buf ; *p && n<MAXFIELD; p=q+1) 
		{
        if ((q=strchr(p,',')) != NULL ||(q=strchr(p,'*')) != NULL)
					{
            val[n++]=p; 
						*q='\0';    //截断字符串
					}
        else break;
    }
		if (n<1) {	return 1;}
	
		/* GGA各字段数据解析 */
		uint8_t resault = decode_nmeagga(raw , val+1, n-1);
		return resault;
}

/* 度分格式转换函数 */
static double dmm2deg(double dmm)
{
    return floor(dmm/100.0)+fmod(dmm,100.0)/60.0;
}

static const int nmea_solq[]={  /* NMEA GPS quality indicator [1] */
    /* 0=Fix not available or invalidi */
    /* 1=GPS SPS Mode, fix valid */
    /* 2=Differential GPS, SPS Mode, fix valid */
    /* 3=GPS PPS Mode, fix valid */
    /* 4=Real Time Kinematic. System used in RTK mode with fixed integers */
    /* 5=Float RTK. Satellite system used in RTK mode, floating integers */
    /* 6=Estimated (dead reckoning) Mode */
    /* 7=Manual Input Mode */
    /* 8=Simulation Mode */
    SOLQ_NONE ,SOLQ_SINGLE, SOLQ_DGPS, SOLQ_PPP , SOLQ_FIX,
    SOLQ_FLOAT,SOLQ_DR    , SOLQ_NONE, SOLQ_NONE, SOLQ_NONE
};

/* GGA各字段数据格式处理 */
int decode_nmeagga( nmea_raw_t *raw, char **val ,int n)
{
		volatile double tod=0.0,lat=0.0,lon=0.0,hdop=0.0,alt=0.0,msl=0.0;
    volatile double age=0.0;
    volatile char ns='N',ew='E',ua=' ',um=' ';
    int i,solq=0,nrcv=0;
    
    for (i=0;i<n;i++) {
        switch (i) {
            case  0: tod =atof(val[i]); break; /* UTC of position (hhmmss) */
            case  1: lat =atof(val[i]); break; /* Latitude (ddmm.mmm) */
            case  2: ns  =*val[i];      break; /* N=north,S=south */
            case  3: lon =atof(val[i]); break; /* Longitude (dddmm.mmm) */
            case  4: ew  =*val[i];      break; /* E=east,W=west */
            case  5: solq=atoi(val[i]); break; /* GPS quality indicator */
            case  6: nrcv=atoi(val[i]); break; /* # of satellites in use */
            case  7: hdop=atof(val[i]); break; /* HDOP */
            case  8: alt =atof(val[i]); break; /* Altitude MSL */
            case  9: ua  =*val[i];      break; /* unit (M) */
            case 10: msl =atof(val[i]); break; /* Geoid separation */
            case 11: um  =*val[i];      break; /* unit (M) */
            case 12: age =atof(val[i]); break; /* Age of differential */
        }
    }
    if ((ns!='N'&&ns!='S')||(ew!='E'&&ew!='W')) {
        //NL_TRACE("invalid nmea gga format\r\n");
        return 1;
    }
		
		/* 此处经纬度数据为百度地图的度格式经纬度，可直接在百度地图寻址 */
    raw->lat=(ns=='N'?1.0:-1.0)*dmm2deg(lat);
    raw->lon=(ew=='E'?1.0:-1.0)*dmm2deg(lon);
    raw->alt=alt+msl;
    raw->nrcv = nrcv;
    raw->solq = nmea_solq[solq];
    return 0;
}

//rtcm数据校验处理


/* RTCM协议CRC_24校验
*	 return  成功:1  失败:0
*
*
*/
int RTCM_CRC(uint8_t* rtcm,uint16_t size)
{
	int crc=0,i;
	uint8_t buf[3];

	for(i=0;i<size-3;i++)
	{
			crc=((crc<<8)&0xFFFFFF)^rtcm_crc_table[(crc>>16)^(*(rtcm++)&0xff)];
	}
	buf[0]=((crc & 0xff0000) >> 16);
	buf[1]=((crc & 0xff00) >> 8);
	buf[2]=(crc&0xff);
	//printf("校验计算结果 %x,%x,%x\r\n",buf[0],buf[1],buf[2]);
	for(uint8_t i=0; i<3; i++)
	{
		//校验结果判断
		if(buf[i]== *(rtcm++));
		else return 1;
	}
	return 0;
}


/*RTCM数据循环判断
*return   成功:1   失败:0
*
*/
int RTCM_Parse(uint8_t *data,  uint16_t data_len)
{
	uint8_t finsh=0;//数据处理完成标志位
	uint16_t len=0,total_len=0;
	uint8_t buf[500]={0};

	while(!finsh && data_len>0)
	{
		while(total_len<data_len)
		{
			/* 检测同步码 */
			if(data[total_len] == 0xD3)
			{
				/* 获取长度,高6位保留 */
				if( (data[total_len+1] & 0xFC) == 0)
					len = (((data[total_len+1] << 8) | data[total_len+2] )+ 6 );//加6表示(3数据头+3CRC校验)
				else //格式错误
					return finsh;
				memcpy(buf, data+total_len, len);
				/* 校验 */
				if( RTCM_CRC( buf,len) != 0 )return finsh;
				total_len += len;
			} 
			else 
				return finsh;
		}
		/* 全部字段解析 */
		finsh = 1;
	}
	return finsh;
}


