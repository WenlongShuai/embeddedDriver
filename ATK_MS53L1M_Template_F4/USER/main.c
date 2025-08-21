#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "ATK_MS53L1M.h"

static void demo_run(void);
static void show_mesg(void);

VL53L1_Dev_t dev;

int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init(168);
	uart_init(115200);
		
	
	show_mesg();
	demo_run();                                                 /* ����ʾ������ */
}



/**
 * @brief       ��ʾʵ����Ϣ
 * @param       ��
 * @retval      ��
 */
static void show_mesg(void)
{
    /* �������ʵ����Ϣ */
    printf("\n");
    printf("********************************\r\n");
    printf("STM32\r\n");
    printf("ATK-MS53L1M\r\n");
    printf("ATOM@ALIENTEK\r\n");
    printf("********************************\r\n");
    printf("\r\n");
}

/**
 * @brief       ��ʾ�豸��ַ
 * @param       ��
 * @retval      ��
 */
static void demo_show_id(uint8_t dir, uint16_t id)
{
    char buf[23];
    
    sprintf(buf, "ATK-MS53L1M [%d] ID: 0x%04x",dir, id);
    
    printf("%s\r\n", buf);
}

/**
 * @brief       ����豸
 * @param       dev     : �豸
 *              iic_addr: �豸IICͨѶ��ַ
 * @retval      0: �豸����
 *              1: �豸����
 */
static void demo_detect_device(uint8_t dir)
{
    uint16_t module_id = 0;
    
    /* ��ȡ�豸ģ��ID */
    VL53L1_RdWord(dir, &dev, 0x010F, &module_id);
    if (module_id != ATK_MS53L1M_MODULE_ID)
    {
        printf("ATK-MS53L1M [%d] Detect Failed!\r\n",dir);
        while (1)
        {
            delay_ms(200);
        }
    }
    
    demo_show_id(dir,   module_id);
}

/**
 * @brief       �����豸
 * @param       dev: �豸
 * @retval      0: ���óɹ�
 *              1: ����ʧ��
 */
static uint8_t demo_config_device(uint8_t dir)
{
    VL53L1_Error ret = VL53L1_ERROR_NONE;
    
    /* �豸������� */ 
    ret = VL53L1_WaitDeviceBooted(dir, &dev);
    
    /* �豸һ���Գ�ʼ�� */
    ret = VL53L1_DataInit(dir, &dev);
    
    /* �豸������ʼ�� */
    ret = VL53L1_StaticInit(&dev);
    
    /* �����豸����ģʽ */
    ret = VL53L1_SetDistanceMode(&dev, VL53L1_DISTANCEMODE_LONG);
    
    /* �趨��������ʱ�� */
    ret = VL53L1_SetMeasurementTimingBudgetMicroSeconds(&dev, 140 * 1000);
    
    /* PER��Χ[1ms, 1000ms] */
    ret = VL53L1_SetInterMeasurementPeriodMilliSeconds(&dev, 150);
    
    /* ֹͣ���� */
    ret = VL53L1_StopMeasurement(dir, &dev);
    
    /* ��ʼ���� */
    ret = VL53L1_StartMeasurement(dir, &dev);
    
    return ret;
}

/**
 * @brief       ������ʾ��ں���
 * @param       ��
 * @retval      ��
 */
static void demo_run(void)
{
	uint8_t num = 0;
	int status;
	uint16_t data = 0;
	uint8_t data_ready = 0;
	
	VL53L1_RangingMeasurementData_t vl53l1_data;
	
	for(int i = 1;i <= ATK_MS53L1M_NUM;i++)
	{
		atk_ms53l1m_init(i);                        /* ģ���ʼ�� */
		demo_detect_device(i);                      /* ����豸 */
		demo_config_device(i);                      /* �����豸 */
		printf("ATK-MS53L1M [%d] Config Succedded!\r\n",i);
	}
	
	while (1)
	{
		status = VL53L1_GetMeasurementDataReady((num % ATK_MS53L1M_NUM)+1, &dev, &data_ready);
		
		if (data_ready)
		{
				status = VL53L1_GetRangingMeasurementData((num % ATK_MS53L1M_NUM)+1, &dev, &vl53l1_data);
				
				if (status == 0)
				{
						data = vl53l1_data.RangeMilliMeter;
						printf("[%d] Distance: %d mm\r\n",(num % ATK_MS53L1M_NUM)+1, data);
				}
				
				VL53L1_ClearInterruptAndStartMeasurement((num % ATK_MS53L1M_NUM)+1, &dev);
		}
		num++;
		
		delay_ms(200);
	}
}
