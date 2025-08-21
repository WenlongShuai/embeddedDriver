//=====================================================================================================
// MadgwickAHRS.h
//=====================================================================================================
//
// Implementation of Madgwick's IMU and AHRS algorithms.
// See: http://www.x-io.co.uk/node/8#open_source_ahrs_and_imu_algorithms
//
// Date			Author          Notes
// 29/09/2011	SOH Madgwick    Initial release
// 02/10/2011	SOH Madgwick	Optimised for reduced CPU load
//
//=====================================================================================================
#ifndef MadgwickAHRS_h
#define MadgwickAHRS_h

// ���峣��
#define SENSOR_DIM 9        // ����������ά�� [ax, ay, az, gx, gy, gz, mx, my, mz]

// �������˲����ṹ��
typedef struct {
    float x[SENSOR_DIM];           // ״̬�������˲������ݣ�
    float P[SENSOR_DIM][SENSOR_DIM]; // ״̬Э�������
    float Q[SENSOR_DIM][SENSOR_DIM]; // ��������Э�������
    float R[SENSOR_DIM][SENSOR_DIM]; // ��������Э�������
} KalmanFilter;

//----------------------------------------------------------------------------------------------------
// Variable declaration

extern volatile float beta;				// algorithm gain
extern volatile float q0, q1, q2, q3;	// quaternion of sensor frame relative to auxiliary frame
extern volatile float pitch, roll, yaw; //euler angle
//---------------------------------------------------------------------------------------------------
// Function declarations

void MadgwickAHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);
void MadgwickAHRSupdateIMU(float gx, float gy, float gz, float ax, float ay, float az);
void computeAngles(void);

void matrix_multiply(float A[SENSOR_DIM][SENSOR_DIM], float B[SENSOR_DIM][SENSOR_DIM], 
                     float C[SENSOR_DIM][SENSOR_DIM]);

void matrix_add(float A[SENSOR_DIM][SENSOR_DIM], float B[SENSOR_DIM][SENSOR_DIM], 
                float C[SENSOR_DIM][SENSOR_DIM]);

void matrix_subtract(float A[SENSOR_DIM][SENSOR_DIM], float B[SENSOR_DIM][SENSOR_DIM], 
                     float C[SENSOR_DIM][SENSOR_DIM]);

void matrix_inverse(float A[SENSOR_DIM][SENSOR_DIM], float B[SENSOR_DIM][SENSOR_DIM]);

void KalmanFilter_Init(KalmanFilter *kf);

void KalmanFilter_Predict(KalmanFilter *kf, float dt);

void KalmanFilter_Update(KalmanFilter *kf, float z[SENSOR_DIM]);

void KalmanFilter_GetFilteredData(KalmanFilter *kf, float filtered_data[SENSOR_DIM]);
#endif
//=====================================================================================================
// End of file
//=====================================================================================================
