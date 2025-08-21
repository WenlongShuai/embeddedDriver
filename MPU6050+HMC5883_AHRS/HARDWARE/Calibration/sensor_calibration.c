#include "sensor_calibration.h"
#include "delay.h"
#include <string.h>
#include <math.h>
#include <float.h>
#include "HMC5883L.h"

// ȫ�ֱ���
static SensorCalibData calib_data;
static CalibrationState calib_state = CALIB_IDLE;
static uint16_t sample_count = 0;
static uint8_t current_face = 0;
static float acc_samples[6][3][ACC_SAMPLE_COUNT];
static float gyro_samples[GYRO_SAMPLE_COUNT][3];
static float mag_samples[MAG_SAMPLE_COUNT][3];


// Flash�洢��ַ���壨�����ʵ�����������
#define CALIB_DATA_FLASH_ADDR 0x0807F000  // ���һҳFlash

// ��ʼ��У׼ϵͳ
void SensorCalibration_Init(void) 
{
    // ���ر����У׼����������У�
    //if (SensorCalibration_LoadFromFlash() != HAL_OK) {
        // ��ʼ��Ĭ��ֵ
        memset(&calib_data, 0, sizeof(SensorCalibData));
				calib_data.acc_bias[0] = -61.0f;
				calib_data.acc_bias[1] = -97.0f;
				calib_data.acc_bias[2] = -530.0f;
	
				calib_data.acc_scale[0] = 1.0f;
        calib_data.acc_scale[1] = 1.0f;
        calib_data.acc_scale[2] = 1.0f;
	
				calib_data.gyro_bias[0] = 22.04f;
				calib_data.gyro_bias[1] = 25.05f;
				calib_data.gyro_bias[2] = -4.0f;
        
//        calib_data.mag_scale[0][0] = 1.0f;
//        calib_data.mag_scale[1][1] = 1.0f;
//        calib_data.mag_scale[2][2] = 1.0f;
	
				calib_data.mag_bias[0] = 10.0f;
				calib_data.mag_bias[1] = -54.5f;
				calib_data.mag_bias[2] = -50.0f;
				
				calib_data.mag_scale[0] = 1.33f;
        calib_data.mag_scale[1] = 1.54f;
        calib_data.mag_scale[2] = 2.10f;
				
				calib_state = CALIB_COMPLETED;
    //}
}

// ������ٶȼ�У׼����
static void CalculateAccCalibration(void) 
{
	#if 0
    float sum[3] = {0};
    
    // ����ƽ��ֵ��Ϊ��ƫ
    for (int i = 0; i < ACC_SAMPLE_COUNT; i++) {
        sum[0] += acc_samples[i][0];
        sum[1] += acc_samples[i][1];
        sum[2] += acc_samples[i][2];
    }
    
    calib_data.acc_bias[0] = sum[0] / ACC_SAMPLE_COUNT;
    calib_data.acc_bias[1] = sum[1] / ACC_SAMPLE_COUNT;
    calib_data.acc_bias[2] = sum[2] / ACC_SAMPLE_COUNT;
    
    // �򻯵ı������Ӽ��㣨ʵ��Ӧ���н���ʹ��������ϣ�
    float gravity_magnitude = 9.81f;  // �������ٶ�
    float avg_magnitude = 0;
    
    for (int i = 0; i < ACC_SAMPLE_COUNT; i++) {
        float x = acc_samples[i][0] - calib_data.acc_bias[0];
        float y = acc_samples[i][1] - calib_data.acc_bias[1];
        float z = acc_samples[i][2] - calib_data.acc_bias[2];
        avg_magnitude += sqrtf(x*x + y*y + z*z);
    }
    
    avg_magnitude /= ACC_SAMPLE_COUNT;
    calib_data.acc_scale[0] = gravity_magnitude / avg_magnitude;
    calib_data.acc_scale[1] = gravity_magnitude / avg_magnitude;
    calib_data.acc_scale[2] = gravity_magnitude / avg_magnitude;
		#endif
    
    // �������ƽ��ֵ
    float avg[6][3];
    for (int face = 0; face < 6; face++) {
        for (int axis = 0; axis < 3; axis++) {
            avg[face][axis] = 0;
            for (int i = 0; i < 100; i++) {
                avg[face][axis] += acc_samples[face][axis][i];
            }
            avg[face][axis] /= 100.0f;
        }
    }
    
    // ������ƫ�����������������Գƣ�
    calib_data.acc_bias[0] = (avg[0][0] + avg[1][0]) / 2.0f;  // X+��X-���е�
    calib_data.acc_bias[1] = (avg[2][1] + avg[3][1]) / 2.0f;  // Y+��Y-���е�
    calib_data.acc_bias[2] = (avg[4][2] + avg[5][2]) / 2.0f;  // Z+��Z-���е�
    
    // ����������ӣ�����ֵΪ��16384 LSB/g @ ��2g���̣�
    float ideal = 16384.0f;
    calib_data.acc_scale[0] = ideal / ((avg[0][0] - avg[1][0]) / 2.0f);
    calib_data.acc_scale[1] = ideal / ((avg[2][1] - avg[3][1]) / 2.0f);
    calib_data.acc_scale[2] = ideal / ((avg[4][2] - avg[5][2]) / 2.0f); 
}

// ����������У׼����
static void CalculateGyroCalibration(void) {
    float sum[3] = {0};
    
    // ����ƽ��ֵ��Ϊ��ƫ
    for (int i = 0; i < GYRO_SAMPLE_COUNT; i++) {
        sum[0] += gyro_samples[i][0];
        sum[1] += gyro_samples[i][1];
        sum[2] += gyro_samples[i][2];
    }
    
    calib_data.gyro_bias[0] = sum[0] / GYRO_SAMPLE_COUNT;
    calib_data.gyro_bias[1] = sum[1] / GYRO_SAMPLE_COUNT;
    calib_data.gyro_bias[2] = sum[2] / GYRO_SAMPLE_COUNT;
}



//����Ӳ������ʼƫ�ƣ�
void calculate_hard_iron(void) 
{
    float min[3] = {INFINITY, INFINITY, INFINITY};
    float max[3] = {-INFINITY, -INFINITY, -INFINITY};    
    // Ѱ�Ҹ�������ֵ����Сֵ
    for (int i = 0; i < MAG_SAMPLE_COUNT; i++) {
        if (mag_samples[i][0] < min[0]) min[0] = mag_samples[i][0];
        if (mag_samples[i][1] < min[1]) min[1]= mag_samples[i][1];
        if (mag_samples[i][2] < min[2]) min[2]= mag_samples[i][2];
        
        if (mag_samples[i][0] > max[0]) max[0] = mag_samples[i][0];
        if (mag_samples[i][1]> max[1]) max[1] = mag_samples[i][1];
        if (mag_samples[i][2] > max[2]) max[2] = mag_samples[i][2];
    }
    
    // Ӳ�����Ϊ��ֵ��ƽ��ֵ
    calib_data.mag_bias[0] = (max[0] + min[0]) / 2.0f;
    calib_data.mag_bias[1] = (max[1] + min[1]) / 2.0f;
    calib_data.mag_bias[2] = (max[2] + min[2]) / 2.0f;
        
//    printf("Ӳ����� (bias): [%.3f, %.3f, %.3f]\n", calib_data.mag_bias[0], calib_data.mag_bias[1], calib_data.mag_bias[2]);
}

//����������������ϣ�
void calculate_soft_iron(void) 
{
    // 1. ȥӲ�����
    float centered_x[MAG_SAMPLE_COUNT], centered_y[MAG_SAMPLE_COUNT], centered_z[MAG_SAMPLE_COUNT];
    for (int i = 0; i < MAG_SAMPLE_COUNT; i++) {
        centered_x[i] = mag_samples[i][0] - calib_data.mag_bias[0];
        centered_y[i] = mag_samples[i][1] - calib_data.mag_bias[1];
        centered_z[i] = mag_samples[i][2] - calib_data.mag_bias[2];
    }
    
    // 2. ����Э������󣨼�Ϊ�ԽǾ�����ϣ�
    float sum_xx = 0, sum_yy = 0, sum_zz = 0;
    float sum_xy = 0, sum_xz = 0, sum_yz = 0;
    
    for (int i = 0; i < MAG_SAMPLE_COUNT; i++) {
        sum_xx += centered_x[i] * centered_x[i];
        sum_yy += centered_y[i] * centered_y[i];
        sum_zz += centered_z[i] * centered_z[i];
        sum_xy += centered_x[i] * centered_y[i];
        sum_xz += centered_x[i] * centered_z[i];
        sum_yz += centered_y[i] * centered_z[i];
    }
    
    sum_xx /= MAG_SAMPLE_COUNT;
    sum_yy /= MAG_SAMPLE_COUNT;
    sum_zz /= MAG_SAMPLE_COUNT;
    sum_xy /= MAG_SAMPLE_COUNT;
    sum_xz /= MAG_SAMPLE_COUNT;
    sum_yz /= MAG_SAMPLE_COUNT;
    
    // 3. ��������������Ϊ�ԽǾ��󣬺��Է������
    // ʵ��Ӧ������ͨ������ֵ�ֽ������������
    float avg_mag = 0;
    for (int i = 0; i < MAG_SAMPLE_COUNT; i++) {
        float mag = sqrtf(centered_x[i] * centered_x[i] + 
                          centered_y[i] * centered_y[i] + 
                          centered_z[i] * centered_z[i]);
        avg_mag += mag;
    }
    avg_mag /= MAG_SAMPLE_COUNT;
    
    // ����������ӣ�����Ϊ�ԽǾ��󣬼�kx=ky=kz=1ʱΪ���������
    calib_data.mag_scale[0] = avg_mag / sqrtf(sum_xx);
    calib_data.mag_scale[1] = avg_mag / sqrtf(sum_yy);
    calib_data.mag_scale[2] = avg_mag / sqrtf(sum_zz);
    
//    printf("������������ (scale): [%.6f, %.6f, %.6f]\n", 
//           calib_data.mag_scale[0], calib_data.mag_scale[1], calib_data.mag_scale[2]);
}

//������У׼����
void calibrate_magnetometer(int16_t raw_x, int16_t raw_y, int16_t raw_z, 
														int16_t* calib_x, int16_t* calib_y, int16_t* calib_z) {
    // 1. ��ȥӲ�����
    float centered_x = raw_x - calib_data.mag_bias[0];
    float centered_y = raw_y - calib_data.mag_bias[1];
    float centered_z = raw_z - calib_data.mag_bias[2];

    // 2. Ӧ������У׼����Ϊ�ԽǾ���˷���
    *calib_x = centered_x * calib_data.mag_bias[0];
    *calib_y = centered_y * calib_data.mag_bias[1];
    *calib_z = centered_z * calib_data.mag_bias[2];
															
															
		*calib_x = centered_x * calib_data.mag_scale[0];
    *calib_y = centered_y * calib_data.mag_scale[1];
    *calib_z = centered_z * calib_data.mag_scale[2];
}

// 5. У׼��֤
void validate_calibration(void) {
    float before_error = 0, after_error = 0;
    float earth_field = 0;
    
    // ����У׼ǰ�����
    for (int i = 0; i < MAG_SAMPLE_COUNT; i++) {
        float mag = sqrtf(mag_samples[i][0] * mag_samples[i][0] + 
                          mag_samples[i][1] * mag_samples[i][1] + 
                          mag_samples[i][2] * mag_samples[i][2]);
        before_error += fabsf(mag - earth_field);
        earth_field += mag;
    }
    earth_field /= MAG_SAMPLE_COUNT;
    before_error /= MAG_SAMPLE_COUNT;
    
    // ����У׼������
    int16_t calib_x, calib_y, calib_z;
    for (int i = 0; i < MAG_SAMPLE_COUNT; i++) {
			int16_t temp[3] = {0};
			temp[0] = mag_samples[i][0];
			temp[1] = mag_samples[i][1];
			temp[2] = mag_samples[i][2];
        calibrate_magnetometer(temp[0], temp[1], temp[2], &calib_x, &calib_y, &calib_z);
        float mag = sqrtf(calib_x * calib_x + calib_y * calib_y + calib_z * calib_z);
        after_error += fabsf(mag - earth_field);
    }
    after_error /= MAG_SAMPLE_COUNT;
    
//    printf("У׼ǰƽ�����: %.6f\n", before_error);
//    printf("У׼��ƽ�����: %.6f\n", after_error);
//    printf("��������: %.2f%%\n", (1 - after_error / before_error) * 100);
}


// ���������У׼�������򻯰棬������Ӳ��ƫ�ƣ�
static void CalculateMagCalibration(void) {
//    float min[3] = {FLT_MAX, FLT_MAX, FLT_MAX};
//    float max[3] = {FLT_MIN, FLT_MIN, FLT_MIN};
//    
//    // �ҳ����ֵ����Сֵ
//    for (int i = 0; i < MAG_SAMPLE_COUNT; i++) {
//        for (int j = 0; j < 3; j++) {
//            if (mag_samples[i][j] < min[j]) min[j] = mag_samples[i][j];
//            if (mag_samples[i][j] > max[j]) max[j] = mag_samples[i][j];
//        }
//    }
//    
//    // ����Ӳ��ƫ�ƣ��������ģ�
//    calib_data.mag_bias[0] = (max[0] + min[0]) / 2.0f;
//    calib_data.mag_bias[1] = (max[1] + min[1]) / 2.0f;
//    calib_data.mag_bias[2] = (max[2] + min[2]) / 2.0f;
//    
//    // �򻯵����У׼��������������ӣ�
//    float avg_radius = 0;
//    int valid_points = 0;
//    
//    for (int i = 0; i < MAG_SAMPLE_COUNT; i++) {
//        float x = mag_samples[i][0] - calib_data.mag_bias[0];
//        float y = mag_samples[i][1] - calib_data.mag_bias[1];
//        float z = mag_samples[i][2] - calib_data.mag_bias[2];
//        
//        // ������Ⱥ��
//        float r = sqrtf(x*x + y*y + z*z);
//        if (r < 2000 && r > 100) {  // ����ʵ�ʴ�����������Χ
//            avg_radius += r;
//            valid_points++;
//        }
//    }
//    
//    if (valid_points > 0) {
//        avg_radius /= valid_points;
//        
//        // ��������������
//        float scale_x = avg_radius / ((max[0] - min[0]) / 2.0f);
//        float scale_y = avg_radius / ((max[1] - min[1]) / 2.0f);
//        float scale_z = avg_radius / ((max[2] - min[2]) / 2.0f);
//        
//        // ������ű任���󣨶ԽǾ���
//        memset(calib_data.mag_scale, 0, sizeof(calib_data.mag_scale));
//        calib_data.mag_scale[0][0] = scale_x;
//        calib_data.mag_scale[1][1] = scale_y;
//        calib_data.mag_scale[2][2] = scale_z;
//    }

    //����Ӳ�����
    calculate_hard_iron();
    
    //�����������
    calculate_soft_iron();
    
    //��֤У׼Ч��
//    validate_calibration();
}

// У׼��������
void SensorCalibration_Process(int16_t acc_raw[3], int16_t gyro_raw[3], int16_t mag_raw[3]) 
{
    switch (calib_state) {
        case CALIB_IDLE:
            // �ȴ�У׼����
            break;
            
        case CALIB_ACC_COLLECTING:
            // �ɼ����ٶȼ�����
            if (sample_count < ACC_SAMPLE_COUNT) 
						{
								acc_samples[current_face][0][sample_count] = acc_raw[0];
								acc_samples[current_face][1][sample_count] = acc_raw[1];
								acc_samples[current_face][2][sample_count] = acc_raw[2];
                sample_count++;
            } 
						else 
						{
							current_face++;
							sample_count = 0;
							
							if(current_face < 6)
							{
								switch(current_face)
								{
									case 1:
										printf("���豸����X-\r\n");
										break;
									case 2:
										printf("���豸����Y+\r\n");
										break;
									case 3:
										printf("���豸����Y-\r\n");
										break;
									case 4:
										printf("���豸����Z+\r\n");
										break;
									case 5:
										printf("���豸����Z-\r\n");
										break;
								}
								
								for(int i = 0;i < 10;i++)
								{
									delay_ms(1000);
								}
								printf("��ʼ�ɼ�\r\n");
							}
							else
							{
								// ������ٶȼ�У׼����
                CalculateAccCalibration();
//                calib_state = CALIB_GYRO_COLLECTING;
								calib_state = CALIB_COMPLETED;
							}
            }
            break;
            
        case CALIB_GYRO_COLLECTING:
            // �ɼ�����������
            if (sample_count < GYRO_SAMPLE_COUNT) 
						{
								gyro_samples[sample_count][0] = gyro_raw[0];
								gyro_samples[sample_count][1] = gyro_raw[1];
								gyro_samples[sample_count][2] = gyro_raw[2];							
                sample_count++;
            } 
						else 
						{
                // ����������У׼����
                CalculateGyroCalibration();
                sample_count = 0;
//                calib_state = CALIB_MAG_COLLECTING;
							calib_state = CALIB_COMPLETED;
            }
            break;
            
        case CALIB_MAG_COLLECTING:
            // �ɼ����������ݣ���Ҫ�û�������8���˶���
            if (sample_count < MAG_SAMPLE_COUNT) 
						{
								mag_samples[sample_count][0] = mag_raw[0];
								mag_samples[sample_count][1] = mag_raw[1];
								mag_samples[sample_count][2] = mag_raw[2];
                sample_count++;
            } 
						else 
						{
							CalculateMagCalibration();
							sample_count = 0;
//                calib_state = CALIB_MAG_PROCESSING;
							calib_state = CALIB_COMPLETED;
            }
            break;
            
        case CALIB_MAG_PROCESSING:
            // ���������У׼����
//            CalculateMagCalibration();
//            calib_state = CALIB_COMPLETED;
            // ����У׼������Flash
//            SensorCalibration_SaveToFlash();
            break;
            
        case CALIB_COMPLETED:
            // У׼��ɣ��ȴ�������
            break;
    }
}

// ��ʼ���ٶȼ�У׼
void SensorCalibration_StartAccCalibration(void) {
    calib_state = CALIB_ACC_COLLECTING;
    sample_count = 0;
}

// ��ʼ������У׼
void SensorCalibration_StartGyroCalibration(void) {
    calib_state = CALIB_GYRO_COLLECTING;
    sample_count = 0;
}

// ��ʼ������У׼
void SensorCalibration_StartMagCalibration(void) {
    calib_state = CALIB_MAG_COLLECTING;
    sample_count = 0;
}


// Ӧ��У׼����
void SensorCalibration_Apply(int16_t acc_raw[3], int16_t gyro_raw[3], int16_t mag_raw[3], 
                             int16_t acc_cal[3], int16_t gyro_cal[3], int16_t mag_cal[3]) {
    // Ӧ�ü��ٶȼ�У׼
    acc_cal[0] = (acc_raw[0] - calib_data.acc_bias[0]) * calib_data.acc_scale[0];
    acc_cal[1] = (acc_raw[1] - calib_data.acc_bias[1]) * calib_data.acc_scale[1];
    acc_cal[2] = (acc_raw[2] - calib_data.acc_bias[2]) * calib_data.acc_scale[2];
//		printf("calib_data.acc_bias[0] ---> %.f\r\n",calib_data.acc_bias[0]);
//	 printf("calib_data.acc_bias[1] ---> %.f\r\n",calib_data.acc_bias[1]);
//	 printf("calib_data.acc_bias[2] ---> %.f\r\n",calib_data.acc_bias[2]);
//	 printf("calib_data.acc_scale[0] ---> %.f\r\n",calib_data.acc_scale[0]);
//	 printf("calib_data.acc_scale[1] ---> %.f\r\n",calib_data.acc_scale[1]);
//	 printf("calib_data.acc_scale[2] ---> %.f\r\n",calib_data.acc_scale[2]);
    
    // Ӧ��������У׼
    gyro_cal[0] = gyro_raw[0] - calib_data.gyro_bias[0];
    gyro_cal[1] = gyro_raw[1] - calib_data.gyro_bias[1];
    gyro_cal[2] = gyro_raw[2] - calib_data.gyro_bias[2];
															 
//	 printf("calib_data.gyro_bias[0] ---> %.2f\r\n",calib_data.gyro_bias[0]);
//	 printf("calib_data.gyro_bias[1] ---> %.2f\r\n",calib_data.gyro_bias[1]);
//	 printf("calib_data.gyro_bias[2] ---> %.2f\r\n",calib_data.gyro_bias[2]);
    
    // Ӧ�ô�����У׼			 
    calibrate_magnetometer(mag_raw[0], mag_raw[1], mag_raw[2], &mag_cal[0], &mag_cal[1], &mag_cal[2]);
//	printf("calib_data.mag_bias[0] ---> %.2f\r\n",calib_data.mag_bias[0]);
//	 printf("calib_data.mag_bias[1] ---> %.2f\r\n",calib_data.mag_bias[1]);
//	 printf("calib_data.mag_bias[2] ---> %.2f\r\n",calib_data.mag_bias[2]);
//	 printf("calib_data.mag_scale[0] ---> %.2f\r\n",calib_data.mag_scale[0]);
//	 printf("calib_data.mag_scale[1] ---> %.2f\r\n",calib_data.mag_scale[1]);
//	 printf("calib_data.mag_scale[2] ---> %.2f\r\n",calib_data.mag_scale[2]);
//    printf("\nԭʼ����: [%d, %d, %d]\n", mag_raw[0], mag_raw[1], mag_raw[2]);
//    printf("У׼������: [%d, %d, %d]\n", mag_cal[0], mag_cal[1], mag_cal[2]);
}

//// ����У׼������Flash
//HAL_StatusTypeDef SensorCalibration_SaveToFlash(void) {
//    // ����Flashҳ
//    HAL_FLASH_Unlock();
//    
//    FLASH_EraseInitTypeDef erase_init;
//    erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
//    erase_init.PageAddress = CALIB_DATA_FLASH_ADDR;
//    erase_init.NbPages = 1;
//    
//    uint32_t page_error = 0;
//    HAL_StatusTypeDef status = HAL_FLASHEx_Erase(&erase_init, &page_error);
//    
//    if (status == HAL_OK) {
//        // д��У׼����
//        uint32_t address = CALIB_DATA_FLASH_ADDR;
//        uint32_t *data_ptr = (uint32_t*)&calib_data;
//        uint32_t data_size = sizeof(SensorCalibData) / sizeof(uint32_t);
//        
//        for (uint32_t i = 0; i < data_size; i++) {
//            status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, data_ptr[i]);
//            if (status != HAL_OK) break;
//            address += 4;
//        }
//    }
//    
//    HAL_FLASH_Lock();
//    return status;
//}

//// ��Flash����У׼����
//HAL_StatusTypeDef SensorCalibration_LoadFromFlash(void) {
//    // ���Flash��ַ�Ƿ���Ч
//    if (*(uint32_t*)CALIB_DATA_FLASH_ADDR == 0xFFFFFFFF) {
//        return HAL_ERROR;  // ����Ч����
//    }
//    
//    // ��ȡУ׼����
//    memcpy(&calib_data, (void*)CALIB_DATA_FLASH_ADDR, sizeof(SensorCalibData));
//    return HAL_OK;
//}

// ��ȡУ׼״̬
CalibrationState SensorCalibration_GetState(void) {
    return calib_state;
}

// ��ȡУ׼����
void SensorCalibration_GetData(SensorCalibData *calib_data_ptr) {
    memcpy(calib_data_ptr, &calib_data, sizeof(SensorCalibData));
}    
