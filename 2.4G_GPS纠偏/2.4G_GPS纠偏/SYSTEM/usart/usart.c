#include "sys.h"
#include "usart.h"	  

uint8_t Buff_USART1_RX[UART_RX1_SIZE]={0};//���ջ�����
uint8_t Buff_USART1_TX[UART_TX1_SIZE]={0};//���ͻ�����
uint8_t rxBuff[rxBuff_SIZE];


uint8_t Flag_DMA_RX1=0;
uint8_t Flag_DMA_TX1=0;
int  rx1Data_CNT = 0;//DMA_RX1���ݰ��ܸ���


#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos ʹ��	  
#endif


#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0){} ;//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*ʹ��microLib�ķ���*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
////u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  
  
void uart_init(u32 bound){
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
		 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);	//ʹ��USART1��GPIOAʱ���Լ����ù���ʱ��
     //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

   //Usart1 NVIC ����

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
		NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
		
		 //USART ��ʼ������

		USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
		USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

    USART_Init(USART1, &USART_InitStructure); //��ʼ������
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//�����ж�
    USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 

}

//���ô��ڽ���DMA DMA1_CH5
void DMA_RX1Init(void)
{

	NVIC_InitTypeDef NVIC_DMA_InitStruc;
	DMA_InitTypeDef DMA_USART1_InitStruc;//DMA��ʼ���ṹ�壨DMA_USART2_RX��
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 ,ENABLE);//DMA1ʱ�ӿ���
	DMA_Cmd(DMA1_Channel5,DISABLE);
	DMA_DeInit(DMA1_Channel5);//USART2_RX DMAӳ��DMA1_Channel6��ʼ��
	
	NVIC_DMA_InitStruc.NVIC_IRQChannel=DMA1_Channel5_IRQn;
	NVIC_DMA_InitStruc.NVIC_IRQChannelCmd=ENABLE;
	NVIC_DMA_InitStruc.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_DMA_InitStruc.NVIC_IRQChannelSubPriority=2;
	NVIC_Init(&NVIC_DMA_InitStruc);
	
	DMA_USART1_InitStruc.DMA_DIR= DMA_DIR_PeripheralSRC;//���䷽�����ã�������Ϊ���ݴ���Ŀ�ĵ�or��Դ��������Դ��ַ��	
//	DMA_USART1_InitStruc.DMA_BufferSize= UART_RX_SIZE;//ָ��DMAͨ�������С
	DMA_USART1_InitStruc.DMA_PeripheralBaseAddr= (uint32_t)&USART1->DR;//�������ַ
	DMA_USART1_InitStruc.DMA_PeripheralInc= DMA_PeripheralInc_Disable;//�趨�����ַ�Ĵ����������
	DMA_USART1_InitStruc.DMA_PeripheralDataSize= DMA_PeripheralDataSize_Byte;
	
	DMA_USART1_InitStruc.DMA_MemoryBaseAddr= (uint32_t)&Buff_USART1_RX;//���ݴ�Ż���ַ
	DMA_USART1_InitStruc.DMA_MemoryInc= DMA_MemoryInc_Enable;//�趨�ڴ��ַ�Ĵ����������
	DMA_USART1_InitStruc.DMA_MemoryDataSize= DMA_MemoryDataSize_Byte;	
	DMA_USART1_InitStruc.DMA_Mode= DMA_Mode_Normal;
	DMA_USART1_InitStruc.DMA_Priority= DMA_Priority_Medium;//DMAͨ�����ȼ�
	DMA_USART1_InitStruc.DMA_M2M= DMA_M2M_Disable;//ʹ�� DMA ͨ�����ڴ浽�ڴ洫��
	DMA_Init(DMA1_Channel5,&DMA_USART1_InitStruc);
	
//	DMA_ITConfig(DMA1_Channel5,DMA1_IT_TC5,ENABLE);
	
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);//USART2_RX DMA����ʹ��
//	DMA_ClearFlag(DMA1_FLAG_TC5);//���DMA1_Channel6ȫ�ֱ�־λ
//	DMA_Cmd(DMA1_Channel5,ENABLE);
}

//����һ��DMA����
void DMA_RX1(DMA_Channel_TypeDef *DMA_Streamx,u16 ndtr)     //ʹ��DMA
{
 
    DMA_Cmd(DMA_Streamx, DISABLE);                      //�ر�DMA���� 
    
    //while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}    //ȷ��DMA���Ա�����  
        
    DMA_SetCurrDataCounter(DMA_Streamx,ndtr);          //���ݴ�����  
	
	//DMA_ClearFlag(DMA1_FLAG_GL6);//���DMA1_Channel6ȫ�ֱ�־λ
    DMA_Cmd(DMA_Streamx, ENABLE);                      //����DMA���� 
//		while (DMA_GetFlagStatus(DMA1_FLAG_TC5) == RESET);
//		DMA_ClearFlag(DMA1_FLAG_TC5);
}     


//���ô��ڽ���DMA DMA1_CH4
void DMA_TX1Init(void)
{
	NVIC_InitTypeDef NVIC_DMA_InitStruc;
	DMA_InitTypeDef DMA_USART1_InitStruc;//DMA��ʼ���ṹ�壨DMA_USART2_RX��
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 ,ENABLE);//DMA1ʱ�ӿ���
	DMA_Cmd(DMA1_Channel4,DISABLE);
	DMA_DeInit(DMA1_Channel4);//USART2_RX DMAӳ��DMA1_Channel6��ʼ��
	
	NVIC_DMA_InitStruc.NVIC_IRQChannel=DMA1_Channel4_IRQn;
	NVIC_DMA_InitStruc.NVIC_IRQChannelCmd=ENABLE;
	NVIC_DMA_InitStruc.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_DMA_InitStruc.NVIC_IRQChannelSubPriority=2;
	NVIC_Init(&NVIC_DMA_InitStruc);
	
	DMA_USART1_InitStruc.DMA_DIR= DMA_DIR_PeripheralDST;//���䷽�����ã�������Ϊ���ݴ���Ŀ�ĵ�or��Դ��������Դ��ַ��	
//	DMA_USART1_InitStruc.DMA_BufferSize= UART_TX_SIZE;//ָ��DMAͨ�������С
	DMA_USART1_InitStruc.DMA_PeripheralBaseAddr= (uint32_t)&USART1->DR;//�������ַ
	DMA_USART1_InitStruc.DMA_PeripheralInc= DMA_PeripheralInc_Disable;//�趨�����ַ�Ĵ����������
	DMA_USART1_InitStruc.DMA_PeripheralDataSize= DMA_PeripheralDataSize_Byte;
	
	DMA_USART1_InitStruc.DMA_MemoryBaseAddr= (uint32_t)&Buff_USART1_TX;//���ݴ�Ż���ַ
	DMA_USART1_InitStruc.DMA_MemoryInc= DMA_MemoryInc_Enable;//�趨�ڴ��ַ�Ĵ����������
	DMA_USART1_InitStruc.DMA_MemoryDataSize= DMA_MemoryDataSize_Byte;	
	DMA_USART1_InitStruc.DMA_Mode= DMA_Mode_Normal;
	DMA_USART1_InitStruc.DMA_Priority= DMA_Priority_Medium;//DMAͨ�����ȼ�
	DMA_USART1_InitStruc.DMA_M2M= DMA_M2M_Disable;//ʹ�� DMA ͨ�����ڴ浽�ڴ洫��
	DMA_Init(DMA1_Channel4,&DMA_USART1_InitStruc);
	
//	DMA_ITConfig(DMA1_Channel4,DMA1_IT_TC4,ENABLE);
	
	DMA1_Channel4->CCR |= 0x02;//����DMA_CH4 TC�ж�
	
	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);//UART DMA����ʹ��
//	DMA_ClearFlag(DMA1_FLAG_TC4);//���DMA1_Channelȫ�ֱ�־λ
//	DMA_Cmd(DMA1_Channel4,ENABLE);
}

//����һ��DMA����
void DMA_TX1(DMA_Channel_TypeDef *DMA_Streamx,u16 ndtr)     //ʹ��DMA_
{
 
    DMA_Cmd(DMA_Streamx, DISABLE);                      //�ر�DMA���� 
    
    //while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}    //ȷ��DMA���Ա�����  
        
    DMA_SetCurrDataCounter(DMA_Streamx,ndtr);          //�������ݴ�����
  
		DMA_ClearFlag(DMA1_FLAG_TC4);											 //���DMA1_Channel��־λ
    DMA_Cmd(DMA_Streamx, ENABLE);                      //����DMA���� 
//		while (DMA_GetFlagStatus(DMA1_FLAG_TC4) == RESET);
//		DMA_ClearFlag(DMA1_FLAG_TC4);
}   


/*
void USART1_IRQHandler(void)                	//����1�жϷ������
	{
	u8 Res;
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{
		Res =USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ�������
		
		if((USART_RX_STA&0x8000)==0)//����δ���
			{
			if(USART_RX_STA&0x4000)//���յ���0x0d
				{
				if(Res!=0x0a)USART_RX_STA=0;//���մ���,���¿�ʼ
				else USART_RX_STA|=0x8000;	//��������� 
				}
			else //��û�յ�0X0D
				{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
					{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
					}		 
				}
			}   		 
     } 
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntExit();  											 
#endif
} 
*/
#endif	


char* custom_str_find(const char* array, const char* str, size_t length) {  
    size_t str_len = 0;  

    // ����Ҫ���ҵ��ַ����ĳ���  
    while (str[str_len] != '\0') {  
        str_len++;  
    }  

    // ���������е�ÿ���ַ�  
    for (size_t i = 0; i <= length - str_len; i++) {  
        size_t j = 0;  

        // ��鵱ǰλ���Ƿ�ƥ���ַ���  
        while (j < str_len && array[i + j] == str[j]) {  
            j++;  
        }  

        // ����ҵ���ָ���ַ���  
        if (j == str_len) {  
            return (char*)&array[i]; // �����ҵ���λ��  
        }  
    }  
    
    return NULL; // û���ҵ�  
}


