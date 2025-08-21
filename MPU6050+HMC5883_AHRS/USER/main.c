#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "timing_fun.h"
#include "MPU6050.h"
#include "HMC5883L.h"
#include "sensor_calibration.h"
#include "MadgwickAHRS.h"

extern float  angle;
extern KalmanFilter kf;

static void demo_niming_report_status(int16_t rol, int16_t pit, int16_t yaw, uint32_t alt, uint8_t fly_mode, uint8_t armed);
static void demo_niming_report_senser(  int16_t  acc_x, int16_t  acc_y, int16_t  acc_z,
                                        int16_t gyro_x, int16_t gyro_y, int16_t gyro_z,
                                        int16_t  mag_x, int16_t  mag_y, int16_t  mag_z);
static void demo_usart1_niming_report(uint8_t fun, uint8_t *dat, uint8_t len);
void Usart_SendString(uint8_t *str, uint32_t len);

void NormalizeVector(float *x, float *y, float *z);

extern volatile float pitch, roll, yaw;


//int16_t acc_raw[3], gyro_raw[3], mag_raw[3];
//int16_t acc_cal[3], gyro_cal[3], mag_cal[3];

int main(void)
{	
	uint8_t ret = 0;
	
//	uart_init(500000);
	uart_init(115200);
	delay_init();                                   //��ʼ����ʱ����
	
	Timing_Fun_Init(10000-1, 72-1);  //100Hz  10ms
	
	Init_HMC5883L();

	/* ��ʼ��MPU6050 */
	ret = mpu6050_init();
	if (ret != 0)
		printf("MPU6050 init failed!\r\n");
	
	// ��ʼ��У׼ϵͳ
	SensorCalibration_Init();
	
	KalmanFilter_Init(&kf);
	
	
	// ����У׼��ʵ��Ӧ����Ӧͨ���û�����������
//	SensorCalibration_StartAccCalibration();
//	printf("���豸����X+\r\n");

//	SensorCalibration_StartGyroCalibration();
//	SensorCalibration_StartMagCalibration();
	// ģ�⴫�������ݣ�δ�˲���

	
	while(1)
	{
		#if 0
		//У׼
		READ_HMC5883L(&mag_raw[0], &mag_raw[1], &mag_raw[2]);
//		mpu6050_get_temperature(&temperature);
		mpu6050_get_gyroscope(&gyro_raw[0], &gyro_raw[1], &gyro_raw[2]);
		mpu6050_get_accelerometer(&acc_raw[0], &acc_raw[1], &acc_raw[2]);
		
		
		// ����У׼����
		SensorCalibration_Process(acc_raw, gyro_raw, mag_raw);
		
		printf("acc_x: %d, acc_y: %d, acc_z: %d\r\n", acc_raw[0], acc_raw[1], acc_raw[2]);
		printf("gyr_x: %d, gyr_y: %d, gyr_z: %d\r\n", gyro_raw[0], gyro_raw[1], gyro_raw[2]);
		printf("mag_x: %d, mag_y: %d, mag_z: %d\r\n", mag_raw[0], mag_raw[1], mag_raw[2]);
		
		if(SensorCalibration_GetState() == CALIB_COMPLETED)
		{
			printf("У׼���\r\n");
			// Ӧ��У׼����
			SensorCalibration_Apply(acc_raw, gyro_raw, mag_raw, 
														 acc_cal, gyro_cal, mag_cal);
			
			sensor_data[0] = acc_cal[0] / 1.0;
			sensor_data[1] = acc_cal[1] / 1.0;
			sensor_data[2] = acc_cal[2] / 1.0;
			sensor_data[3] = gyro_cal[0] / 1.0;
			sensor_data[4] = gyro_cal[1] / 1.0;
			sensor_data[5] = gyro_cal[2] / 1.0;
			sensor_data[6] = mag_cal[0] / 1.0;
			sensor_data[7] = mag_cal[1] / 1.0;
			sensor_data[8] = mag_cal[2] / 1.0;			
			
			
			// ִ��һ���˲�����
			KalmanFilter_Predict(&kf, dt);
			KalmanFilter_Update(&kf, sensor_data);

			// ��ȡ�˲��������
			float filtered_data[SENSOR_DIM];
			KalmanFilter_GetFilteredData(&kf, filtered_data);

			/* �ϴ����������Ϣ�����ڵ������� */
	//		printf("pit: %.2f, rol: %.2f, yaw: %.2f\r\n", pit, rol, yaw);
//			printf("acc_x_cal: %d, acc_y_cal: %d, acc_z_cal: %d\r\n", acc_cal[0], acc_cal[1], acc_cal[2]);
//			printf("gyr_x_cal: %d, gyr_y_cal: %d, gyr_z_cal: %d\r\n", gyro_cal[0], gyro_cal[1], gyro_cal[2]);
//			printf("mag_x_cal: %d, mag_y_cal: %d, mag_z_cal: %d\r\n", mag_cal[0], mag_cal[1], mag_cal[2]);
			
			
			 // ��ӡ�˲�ǰ�������
			printf("ԭʼ����:\n");
			printf("���ٶ�: [%.4f, %.4f, %.4f]\n", sensor_data[0], sensor_data[1], sensor_data[2]);
			printf("������: [%.4f, %.4f, %.4f]\n", sensor_data[3], sensor_data[4], sensor_data[5]);
			printf("������: [%.4f, %.4f, %.4f]\n", sensor_data[6], sensor_data[7], sensor_data[8]);
			
			printf("\n�˲�������:\n");
			printf("���ٶ�: [%.4f, %.4f, %.4f]\n", filtered_data[0], filtered_data[1], filtered_data[2]);
			printf("������: [%.4f, %.4f, %.4f]\n", filtered_data[3], filtered_data[4], filtered_data[5]);
			printf("������: [%.4f, %.4f, %.4f]\n", filtered_data[6], filtered_data[7], filtered_data[8]);
		}
		delay_ms(50);
	#endif
	
				/* �ϴ�״̬֡�ʹ�����֡����������վV4 */
//			demo_niming_report_status((int16_t)(roll * 100), (int16_t)((pitch) * 100), (int16_t)(yaw * 100), 0, 0, 0);
//			demo_niming_report_senser(acc_cal[0], acc_cal[1], acc_cal[2], gyro_cal[0], gyro_cal[1], gyro_cal[2], mag_cal[0], mag_cal[1], mag_cal[2]);
	}
}


/**
 * @brief       ����״̬֡����������վV4
 * @param       rol     : �����
 *              pit     : ������
 *              yaw     : �����
 *              alt     : ���и߶ȣ���λ��cm
 *              fly_mode: ����ģʽ
 *              armed   : ����״̬��0xA0������ 0xA1������
 * @retval      ��
 */
static void demo_niming_report_status(int16_t rol, int16_t pit, int16_t yaw, uint32_t alt, uint8_t fly_mode, uint8_t armed)
{
    uint8_t send_buf[12];
    
    /* ����� */
    send_buf[0] = (rol >> 8) & 0xFF;
    send_buf[1] = rol & 0xFF;
    /* ������ */
    send_buf[2] = (pit >> 8) & 0xFF;
    send_buf[3] = pit & 0xFF;
    /* ����� */
    send_buf[4] = (yaw >> 8) & 0xFF;
    send_buf[5] = yaw & 0xFF;
    /* ���и߶� */
    send_buf[6] = (alt >> 24) & 0xFF;
    send_buf[7] = (alt >> 16) & 0xFF;
    send_buf[8] = (alt >> 8) & 0xFF;
    send_buf[9] = alt & 0xFF;
    /* ����ģʽ */
    send_buf[10] = fly_mode;
    /* ����״̬ */
    send_buf[11] = armed;
    
    /* ״̬֡�Ĺ�����Ϊ0x01 */
    demo_usart1_niming_report(0x01, send_buf, 12);
}


/**
 * @brief       ���ʹ�����֡����������վV4
 * @param       acc_x : x���ϵļ��ٶ�ֵ
 *              acc_y : y���ϵļ��ٶ�ֵ
 *              acc_z : z���ϵļ��ٶ�ֵ
 *              gyro_x: x���ϵ�������ֵ
 *              gyro_y: y���ϵ�������ֵ
 *              gyro_z: z���ϵ�������ֵ
 *              mag_x : x���ϵĴ�����ֵ��ATK-MS6050��֧�֣�
 *              mag_y : y���ϵĴ�����ֵ��ATK-MS6050��֧�֣�
 *              mag_z : z���ϵĴ�����ֵ��ATK-MS6050��֧�֣�
 * @retval      ��
 */
static void demo_niming_report_senser(  int16_t  acc_x, int16_t  acc_y, int16_t  acc_z,
                                        int16_t gyro_x, int16_t gyro_y, int16_t gyro_z,
                                        int16_t  mag_x, int16_t  mag_y, int16_t  mag_z)
{
    uint8_t send_buf[18];
    
    /* x���ϵļ��ٶ�ֵ */
    send_buf[0] = (acc_x >> 8) & 0xFF;
    send_buf[1] = acc_x & 0xFF;
    /* y���ϵļ��ٶ�ֵ */
    send_buf[2] = (acc_y >> 8) & 0xFF;
    send_buf[3] = acc_y & 0xFF;
    /* z���ϵļ��ٶ�ֵ */
    send_buf[4] = (acc_z >> 8) & 0xFF;
    send_buf[5] = acc_z & 0xFF;
    /* x���ϵ�������ֵ */
    send_buf[6] = (gyro_x >> 8) & 0xFF;
    send_buf[7] = gyro_x & 0xFF;
    /* y���ϵ�������ֵ */
    send_buf[8] = (gyro_y >> 8) & 0xFF;
    send_buf[9] = gyro_y & 0xFF;
    /* z���ϵ�������ֵ */
    send_buf[10] = (gyro_z >> 8) & 0xFF;
    send_buf[11] = gyro_z & 0xFF;
    /* x���ϵĴ�����ֵ */
    send_buf[12] = (mag_x >> 8) & 0xFF;
    send_buf[13] = mag_x & 0xFF;
    /* y���ϵĴ�����ֵ */
    send_buf[14] = (mag_y >> 8) & 0xFF;
    send_buf[15] = mag_y & 0xFF;
    /* z���ϵĴ�����ֵ */
    send_buf[16] = (mag_z >> 8) & 0xFF;
    send_buf[17] = mag_z & 0xFF;
    
    /* �������Ĺ�����Ϊ0x02 */
    demo_usart1_niming_report(0x02, send_buf, 18);
}

/**
 * @brief       ͨ������1������������������վV4
 * @param       fun: ������
 *              dat: �����͵����ݣ����28�ֽڣ�
 *              len: dat���ݵ���Чλ��
 * @retval      ��
 */
static void demo_usart1_niming_report(uint8_t fun, uint8_t *dat, uint8_t len)
{
    uint8_t send_buf[32];
    uint8_t i;
    
    if (len > 28)
    {
        return;
    }
    
    send_buf[len+4] = 0;            /* У��λ���� */
    send_buf[0] = 0xAA;             /* ֡ͷΪ0xAAAA */
    send_buf[1] = 0xAA;             /* ֡ͷΪ0xAAAA */
    send_buf[2] = fun;              /* ������ */
    send_buf[3] = len;              /* ���ݳ��� */
    for (i=0; i<len; i++)           /* �������� */
    {
        send_buf[4 + i] = dat[i];
    }
    for (i=0; i<(len + 4); i++)     /* ����У��� */
    {
        send_buf[len + 4] += send_buf[i];
    }
    
    /* �������� */
    Usart_SendString(send_buf, len + 5);
		
		
}

/*****************  �����ַ��� **********************/
void Usart_SendString(uint8_t *str, uint32_t len)
{	
	uint16_t i;
    
	for(i = 0; i < len; i++)
	{
			// �ȴ����ͻ�����Ϊ��
			while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
			
			// ��������
			USART_SendData(USART1, str[i]);
	}
	
	// �ȴ��������
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
}



