#ifndef __KEY_H
#define __KEY_H

#include "sys.h"

// ����״̬����
typedef enum {
    KEY_STATE_IDLE,        // ����״̬
    KEY_STATE_PRESSED,     // ����״̬
    KEY_STATE_SHORT_RELEASED, // �̰��ͷ�״̬
    KEY_STATE_WAIT_DOUBLE,   // �ȴ�˫��״̬
		KEY_STATE_DOUBLE_PRESSED, //����˫�������еڶ��ΰ���״̬
    KEY_STATE_LONG_PRESS,    // ����״̬
} KeyState_t;

// �����¼�����
typedef enum {
    KEY_EVENT_NONE,        // ���¼�
    KEY_EVENT_CLICK,       // �����¼�
    KEY_EVENT_DOUBLE_CLICK,// ˫���¼�
    KEY_EVENT_LONG_PRESS,  // �����¼�
} KeyEvent_t;

// �����ṹ��
typedef struct {
    KeyState_t state;      // ��ǰ״̬
    KeyEvent_t event;      // �����¼�
    uint32_t pressTime;    // ����ʱ��
    uint32_t releaseTime;  // �ͷ�ʱ��
    uint8_t pressed;       // ������ǰ�Ƿ񱻰���
} Key_t;

// �������Ŷ���  KEY0
#define KEY0_GPIO_PORT     GPIOE
#define KEY0_GPIO_PIN      GPIO_Pin_4
#define KEY0_GPIO_CLK      RCC_AHB1Periph_GPIOE

// �������Ŷ���  KEY1
#define KEY1_GPIO_PORT     GPIOE
#define KEY1_GPIO_PIN      GPIO_Pin_3
#define KEY1_GPIO_CLK      RCC_AHB1Periph_GPIOE

void KEY_Init(void);
void Key_Process_K0(void);


#endif /* __KEY_H */
