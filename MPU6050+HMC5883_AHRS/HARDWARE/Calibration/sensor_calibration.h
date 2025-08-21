#ifndef __SENSOR_CALIBRATION_H
#define __SENSOR_CALIBRATION_H

#include "sys.h"


#define ACC_SAMPLE_COUNT  100  // ���ٶȼƲ�������
#define GYRO_SAMPLE_COUNT 100  // �����ǲ�������
#define MAG_SAMPLE_COUNT  500  // �����Ʋ�������

// ������У׼�����ṹ��
typedef struct {
    // ���ٶȼ�У׼����
    float acc_bias[3];       // ��ƫ
    float acc_scale[3];      // ��������
    
    // ������У׼����
    float gyro_bias[3];      // ��ƫ
		
    
    // ������У׼����
    float mag_bias[3];       // Ӳ��ƫ��
//    float mag_scale[3][3];   // ��ű任����
	float mag_scale[3];
} SensorCalibData;

// У׼״̬ö��
typedef enum {
    CALIB_IDLE,             // ����״̬
    CALIB_ACC_COLLECTING,   // ���ٶȼ����ݲɼ�
    CALIB_GYRO_COLLECTING,  // ���������ݲɼ�
    CALIB_MAG_COLLECTING,   // ���������ݲɼ�
    CALIB_MAG_PROCESSING,   // ���������ݴ���
    CALIB_COMPLETED         // У׼���
} CalibrationState;

// ��������
void SensorCalibration_Init(void);
void SensorCalibration_Process(int16_t acc_raw[3], int16_t gyro_raw[3], int16_t mag_raw[3]);
CalibrationState SensorCalibration_GetState(void);
void SensorCalibration_GetData(SensorCalibData *calib_data);
void SensorCalibration_Apply(int16_t acc_raw[3], int16_t gyro_raw[3], int16_t mag_raw[3], 
                             int16_t acc_cal[3], int16_t gyro_cal[3], int16_t mag_cal[3]);
                             
void SensorCalibration_StartAccCalibration(void);
void SensorCalibration_StartGyroCalibration(void);
void SensorCalibration_StartMagCalibration(void);

#endif   /* __SENSOR_CALIBRATION_H */
