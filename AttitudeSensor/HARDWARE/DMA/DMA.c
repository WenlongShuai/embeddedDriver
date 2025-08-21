#include "DMA.h"


/* ���������� */
uint8_t I2C1_TxBuffer[256];
uint8_t I2C1_RxBuffer[256];
uint8_t I2C1_TransferComplete = 0;
uint8_t I2C1_TransferError = 0;

/**
 * ��ʼ��I2C1�����DMAͨ��
 */
void I2C1_DMA_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    I2C_InitTypeDef I2C_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    /* ʹ�����ʱ�� */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_DMA1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    
    /* ����I2C1����: PB6-SCL, PB7-SDA */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /* ��PB6��PB7���ӵ�I2C1 */
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);
    
    /* ����I2C1 */
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;  // ��Ϊ���豸ʱ����Ҫ���������ַ
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 100000;  // 100kHz��׼ģʽ
    I2C_Init(I2C1, &I2C_InitStructure);
    
    /* ����DMA����ͨ�� - DMA1 Stream6 Channel1 */
    DMA_DeInit(DMA1_Stream6);
    DMA_InitStructure.DMA_Channel = DMA_Channel_1;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&I2C1->DR;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)I2C1_TxBuffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;  // �ڴ浽����
    DMA_InitStructure.DMA_BufferSize = 0;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  // ����ģʽ����ѭ��
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA1_Stream6, &DMA_InitStructure);
    
    /* ����DMA����ͨ�� - DMA1 Stream0 Channel1 */
    DMA_DeInit(DMA1_Stream0);
    DMA_InitStructure.DMA_Channel = DMA_Channel_1;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&I2C1->DR;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)I2C1_RxBuffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;  // ���赽�ڴ�
    DMA_InitStructure.DMA_BufferSize = 0;
    DMA_Init(DMA1_Stream0, &DMA_InitStructure);
    
    /* ʹ��DMA�ж� */
    DMA_ITConfig(DMA1_Stream6, DMA_IT_TC, ENABLE);  // ��������ж�
    DMA_ITConfig(DMA1_Stream0, DMA_IT_TC, ENABLE);  // ��������ж�
    DMA_ITConfig(DMA1_Stream6, DMA_IT_TE, ENABLE);  // ���ʹ����ж�
    DMA_ITConfig(DMA1_Stream0, DMA_IT_TE, ENABLE);  // ���մ����ж�
    
    /* ����DMA�ж����ȼ� */
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream0_IRQn;
    NVIC_Init(&NVIC_InitStructure);
    
    /* ʹ��I2C1��DMA���� */
    I2C_DMACmd(I2C1, ENABLE);
    
    /* ʹ��I2C1 */
    I2C_Cmd(I2C1, ENABLE);
}

/**
 * ���I2C����״̬���ڱ�Ҫʱ�ָ�
 * @return 0��ʾ����������1��ʾ��Ҫ�ָ�
 */
uint8_t I2C1_CheckBusState(void)
{
    uint32_t timeout = 10000;
    
    /* ���I2C�����Ƿ�æ */
    if(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
    {
        /* ���Է���ֹͣ�ź� */
        I2C_GenerateSTOP(I2C1, ENABLE);
        
        /* �ȴ����߿��� */
        timeout = 10000;
        while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
        {
            if(timeout-- == 0)
            {
                /* ���߻ָ�ʧ�ܣ�������Ҫ����I2C���� */
                return 1;
            }
        }
    }
    
    return 0;
}

/**
 * ����I2C����
 */
void I2C1_Reset(void)
{
    /* ����I2C1 */
    I2C_Cmd(I2C1, DISABLE);
    
    /* ����DMAͨ�� */
    DMA_Cmd(DMA1_Stream6, DISABLE);
    DMA_Cmd(DMA1_Stream0, DISABLE);
    
    /* ����I2C���� */
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);
    
    /* ���³�ʼ��I2C��DMA */
    I2C1_DMA_Init();
}

/**
 * ʹ��DMA�������ݵ�I2C���豸
 * @param slaveAddr �ӻ���ַ(7λ)
 * @param data Ҫ���͵����ݻ�����
 * @param size Ҫ���͵������ֽ���
 * @return 0��ʾ�ɹ�����0��ʾ����
 */
uint8_t I2C1_DMA_SendData(uint8_t slaveAddr, uint8_t *data, uint16_t size)
{
    uint32_t timeout = 10000;
    
    /* �������״̬���ڱ�Ҫʱ�ָ� */
    if(I2C1_CheckBusState() != 0)
    {
        /* ���߻ָ�ʧ�ܣ�����I2C���� */
        I2C1_Reset();
        return 6;  // ���߻ָ�ʧ��
    }
    
    /* �������ݵ����ͻ����� */
    for(uint16_t i = 0; i < size; i++)
    {
        I2C1_TxBuffer[i] = data[i];
    }
    
    /* ����DMA���ͻ�������С */
    DMA1_Stream6->NDTR = size;
    
    /* �������״̬��־ */
    I2C1_TransferComplete = 0;
    I2C1_TransferError = 0;
    
    /* ������ʼ�ź� */
    I2C_GenerateSTART(I2C1, ENABLE);
    
    /* �ȴ�������ʼ�ź���� */
    timeout = 10000;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(timeout-- == 0)
        {
            /* ����ֹͣ�ź� */
            I2C_GenerateSTOP(I2C1, ENABLE);
            return 2;  // ��ʼ�źų�ʱ
        }
    }
    
    /* ���ʹӻ���ַ+д���� */
    I2C_Send7bitAddress(I2C1, slaveAddr, I2C_Direction_Transmitter);
    
    /* �ȴ���ַ������� */
    timeout = 10000;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if(timeout-- == 0)
        {
            /* ����ֹͣ�ź� */
            I2C_GenerateSTOP(I2C1, ENABLE);
            return 3;  // ��ַ���ͳ�ʱ��������ACK
        }
    }
    
    /* ʹ��DMA���� */
    DMA_Cmd(DMA1_Stream6, ENABLE);
    
    /* �ȴ�DMA������� */
    timeout = 100000;
    while(!I2C1_TransferComplete && !I2C1_TransferError)
    {
        if(timeout-- == 0)
        {
            /* ������ʱ��ֹͣ���� */
            DMA_Cmd(DMA1_Stream6, DISABLE);
            I2C_GenerateSTOP(I2C1, ENABLE);
            return 4;  // DMA���䳬ʱ
        }
    }
    
    if(I2C1_TransferError)
    {
        return 5;  // DMA�������
    }
    
    return 0;  // ����ɹ�
}

/**
 * ʹ��DMA��I2C���豸��������
 * @param slaveAddr �ӻ���ַ(7λ)
 * @param data �洢�������ݵĻ�����
 * @param size Ҫ���յ������ֽ���
 * @return 0��ʾ�ɹ�����0��ʾ����
 */
uint8_t I2C1_DMA_ReceiveData(uint8_t slaveAddr, uint8_t regAddress, uint8_t *data, uint16_t size)
{
    uint32_t timeout = 10000;
    
    /* �������״̬���ڱ�Ҫʱ�ָ� */
    if(I2C1_CheckBusState() != 0)
    {
        /* ���߻ָ�ʧ�ܣ�����I2C���� */
        I2C1_Reset();
        return 6;  // ���߻ָ�ʧ��
    }
    
    /* ����DMA���ջ�������С */
    DMA1_Stream0->NDTR = size;
    
    /* �������״̬��־ */
    I2C1_TransferComplete = 0;
    I2C1_TransferError = 0;
    
    /* ������ʼ�ź� */
    I2C_GenerateSTART(I2C1, ENABLE);
    
    /* �ȴ�������ʼ�ź���� */
    timeout = 10000;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(timeout-- == 0)
        {
            /* ����ֹͣ�ź� */
            I2C_GenerateSTOP(I2C1, ENABLE);
            return 2;  // ��ʼ�źų�ʱ
        }
    }
    
    /* ���ʹӻ���ַ+������ */
    I2C_Send7bitAddress(I2C1, slaveAddr, I2C_Direction_Receiver);
    
    /* �ȴ���ַ������� */
    timeout = 10000;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        if(timeout-- == 0)
        {
            /* ����ֹͣ�ź� */
            I2C_GenerateSTOP(I2C1, ENABLE);
            return 3;  // ��ַ���ͳ�ʱ��������ACK
        }
    }
		
		I2C_SendData(I2C1, regAddress);
		
		/* �ȴ���ַ������� */
    timeout = 10000;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        if(timeout-- == 0)
        {
            /* ����ֹͣ�ź� */
            I2C_GenerateSTOP(I2C1, ENABLE);
            return 3;  // ��ַ���ͳ�ʱ��������ACK
        }
    }
    
    /* ʹ��DMA���� */
    DMA_Cmd(DMA1_Stream0, ENABLE);
    
    /* �ȴ�DMA������� */
    timeout = 100000;
    while(!I2C1_TransferComplete && !I2C1_TransferError)
    {
        if(timeout-- == 0)
        {
            /* ������ʱ��ֹͣ���� */
            DMA_Cmd(DMA1_Stream0, DISABLE);
            I2C_GenerateSTOP(I2C1, ENABLE);
            return 4;  // DMA���䳬ʱ
        }
    }
    
    if(I2C1_TransferError)
    {
        return 5;  // DMA�������
    }
    
    /* ���ƽ��յ������ݵ��û������� */
    for(uint16_t i = 0; i < size; i++)
    {
        data[i] = I2C1_RxBuffer[i];
    }
    
    return 0;  // ����ɹ�
}

/**
 * ʹ��DMA�ȷ��������ٽ������ݣ���ϲ�����
 * @param slaveAddr �ӻ���ַ(7λ)
 * @param txData Ҫ���͵����ݻ�����
 * @param txSize Ҫ���͵������ֽ���
 * @param rxData �洢�������ݵĻ�����
 * @param rxSize Ҫ���յ������ֽ���
 * @return 0��ʾ�ɹ�����0��ʾ����
 */
uint8_t I2C1_DMA_SendThenReceive(uint8_t slaveAddr, uint8_t *txData, uint16_t txSize, uint8_t *rxData, uint16_t rxSize)
{
    uint32_t timeout = 10000;
    uint8_t status;
    
    /* �������״̬���ڱ�Ҫʱ�ָ� */
    if(I2C1_CheckBusState() != 0)
    {
        /* ���߻ָ�ʧ�ܣ�����I2C���� */
        I2C1_Reset();
        return 6;  // ���߻ָ�ʧ��
    }
    
    /* ���Ʒ������ݵ������� */
    for(uint16_t i = 0; i < txSize; i++)
    {
        I2C1_TxBuffer[i] = txData[i];
    }
    
    /* ����DMA���ͻ�������С */
    DMA1_Stream6->NDTR = txSize;
    
    /* �������״̬��־ */
    I2C1_TransferComplete = 0;
    I2C1_TransferError = 0;
    
    /* ������ʼ�ź� */
    I2C_GenerateSTART(I2C1, ENABLE);
    
    /* �ȴ�������ʼ�ź���� */
    timeout = 10000;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(timeout-- == 0)
        {
            /* ����ֹͣ�ź� */
            I2C_GenerateSTOP(I2C1, ENABLE);
            return 2;  // ��ʼ�źų�ʱ
        }
    }
    
    /* ���ʹӻ���ַ+д���� */
    I2C_Send7bitAddress(I2C1, slaveAddr, I2C_Direction_Transmitter);
    
    /* �ȴ���ַ������� */
    timeout = 10000;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if(timeout-- == 0)
        {
            /* ����ֹͣ�ź� */
            I2C_GenerateSTOP(I2C1, ENABLE);
            return 3;  // ��ַ���ͳ�ʱ��������ACK
        }
    }
    
    /* ʹ��DMA���� */
    DMA_Cmd(DMA1_Stream6, ENABLE);
    
    /* �ȴ�DMA������� */
    timeout = 100000;
    while(!I2C1_TransferComplete && !I2C1_TransferError)
    {
        if(timeout-- == 0)
        {
            /* ������ʱ��ֹͣ���� */
            DMA_Cmd(DMA1_Stream6, DISABLE);
            I2C_GenerateSTOP(I2C1, ENABLE);
            return 4;  // DMA���䳬ʱ
        }
    }
    
    if(I2C1_TransferError)
    {
        return 5;  // DMA�������
    }
    
    /* ���ô���״̬��־��׼������ */
    I2C1_TransferComplete = 0;
    I2C1_TransferError = 0;
    
    /* ����DMA���ջ�������С */
    DMA1_Stream0->NDTR = rxSize;
    
    /* �����ظ���ʼ�ź� */
    I2C_GenerateSTART(I2C1, ENABLE);
    
    /* �ȴ�������ʼ�ź���� */
    timeout = 10000;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(timeout-- == 0)
        {
            /* ����ֹͣ�ź� */
            I2C_GenerateSTOP(I2C1, ENABLE);
            return 7;  // �ظ���ʼ�źų�ʱ
        }
    }
    
    /* ���ʹӻ���ַ+������ */
    I2C_Send7bitAddress(I2C1, slaveAddr, I2C_Direction_Receiver);
    
    /* �ȴ���ַ������� */
    timeout = 10000;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        if(timeout-- == 0)
        {
            /* ����ֹͣ�ź� */
            I2C_GenerateSTOP(I2C1, ENABLE);
            return 8;  // ����ַ���ͳ�ʱ
        }
    }
    
    /* ʹ��DMA���� */
    DMA_Cmd(DMA1_Stream0, ENABLE);
    
    /* �ȴ�DMA������� */
    timeout = 100000;
    while(!I2C1_TransferComplete && !I2C1_TransferError)
    {
        if(timeout-- == 0)
        {
            /* ������ʱ��ֹͣ���� */
            DMA_Cmd(DMA1_Stream0, DISABLE);
            I2C_GenerateSTOP(I2C1, ENABLE);
            return 9;  // DMA���ճ�ʱ
        }
    }
    
    if(I2C1_TransferError)
    {
        return 10;  // DMA���մ���
    }
    
    /* ���ƽ��յ������ݵ��û������� */
    for(uint16_t i = 0; i < rxSize; i++)
    {
        rxData[i] = I2C1_RxBuffer[i];
    }
    
    return 0;  // ����ɹ�
}

/**
 * DMA1 Stream6 �жϷ����� - I2C1�������
 */
void DMA1_Stream6_IRQHandler(void)
{
    /* ���DMA��������жϱ�־ */
    if(DMA_GetITStatus(DMA1_Stream6, DMA_IT_TCIF6))
    {
        /* ����жϱ�־ */
        DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_TCIF6);
        
        /* ����DMA���� */
        DMA_Cmd(DMA1_Stream6, DISABLE);
        
        /* ���÷�����ɱ�־ */
        I2C1_TransferComplete = 1;
    }
    
    /* ���DMA��������жϱ�־ */
    if(DMA_GetITStatus(DMA1_Stream6, DMA_IT_TEIF6))
    {
        /* ����жϱ�־ */
        DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_TEIF6);
        
        /* ����DMA���� */
        DMA_Cmd(DMA1_Stream6, DISABLE);
        
        /* ����ֹͣ�ź� */
        I2C_GenerateSTOP(I2C1, ENABLE);
        
        /* ���ô����־ */
        I2C1_TransferError = 1;
    }
}

/**
 * DMA1 Stream0 �жϷ����� - I2C1�������
 */
void DMA1_Stream0_IRQHandler(void)
{
    /* ���DMA��������жϱ�־ */
    if(DMA_GetITStatus(DMA1_Stream0, DMA_IT_TCIF0))
    {
        /* ����жϱ�־ */
        DMA_ClearITPendingBit(DMA1_Stream0, DMA_IT_TCIF0);
        
        /* ����DMA���� */
        DMA_Cmd(DMA1_Stream0, DISABLE);
        
        /* ����ֹͣ�ź� */
        I2C_GenerateSTOP(I2C1, ENABLE);
        
        /* ���ý�����ɱ�־ */
        I2C1_TransferComplete = 1;
    }
    
    /* ���DMA��������жϱ�־ */
    if(DMA_GetITStatus(DMA1_Stream0, DMA_IT_TEIF0))
    {
        /* ����жϱ�־ */
        DMA_ClearITPendingBit(DMA1_Stream0, DMA_IT_TEIF0);
        
        /* ����DMA���� */
        DMA_Cmd(DMA1_Stream0, DISABLE);
        
        /* ����ֹͣ�ź� */
        I2C_GenerateSTOP(I2C1, ENABLE);
        
        /* ���ô����־ */
        I2C1_TransferError = 1;
    }
}

/**
 * ���I2C1����״̬
 */
uint8_t I2C1_CheckConnection(uint8_t slaveAddr)
{
    uint32_t timeout = 10000;
    
    /* �������״̬���ڱ�Ҫʱ�ָ� */
    if(I2C1_CheckBusState() != 0)
    {
        /* ���߻ָ�ʧ�ܣ�����I2C���� */
        I2C1_Reset();
        return 1;  // ���߻ָ�ʧ��
    }
    
    /* ������ʼ�ź� */
    I2C_GenerateSTART(I2C1, ENABLE);
    
    /* �ȴ���ʼ�źŷ������ */
    timeout = 10000;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(timeout-- == 0)
            return 2;  // ��ʼ�źų�ʱ
    }
    
    /* ���ʹӻ���ַ+д���� */
    I2C_Send7bitAddress(I2C1, slaveAddr, I2C_Direction_Transmitter);
    
    /* �ȴ���ַ������� */
    timeout = 10000;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if(timeout-- == 0)
        {
            /* ����ֹͣ�ź� */
            I2C_GenerateSTOP(I2C1, ENABLE);
            return 3;  // ��ַ���ͳ�ʱ��������ACK
        }
    }
    
    /* ����ֹͣ�ź� */
    I2C_GenerateSTOP(I2C1, ENABLE);
    
    return 0;  // ���ӳɹ�
}
