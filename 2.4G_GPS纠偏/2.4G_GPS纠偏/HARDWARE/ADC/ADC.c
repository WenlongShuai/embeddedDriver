#include "stm32f10x.h"                  // Device header
#include "adc.h"
uint16_t AD_Value[150];


/*
	转换时间：TCONV=采样时间（设置采样周期）+12.5个周期(采集12位AD固定时间)
	T = 55.5+12.5=68T*(1/12) = 5.666ms
	
*/
void AD_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//12Mhz
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_55Cycles5);
		
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	
	
	NVIC_InitTypeDef NVIC_DMA_InitSttuc;
	NVIC_DMA_InitSttuc.NVIC_IRQChannel=DMA1_Channel1_IRQn;
	NVIC_DMA_InitSttuc.NVIC_IRQChannelCmd=ENABLE;
	NVIC_DMA_InitSttuc.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_DMA_InitSttuc.NVIC_IRQChannelSubPriority=2;
	NVIC_Init(&NVIC_DMA_InitSttuc);
	
	
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)AD_Value;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 150;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
	DMA_Cmd(DMA1_Channel1, ENABLE);
	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);
	
	DMA_ITConfig(DMA1_Channel1,DMA1_IT_TC1,ENABLE);//配置接收完成中断
	
	ADC_ResetCalibration(ADC1);
	
//	while (DMA_GetFlagStatus(DMA1_FLAG_TC1) == RESET);
//	DMA_ClearFlag(DMA1_FLAG_TC1);
	
	while (ADC_GetResetCalibrationStatus(ADC1) == SET);
	ADC_StartCalibration(ADC1);
	while (ADC_GetCalibrationStatus(ADC1) == SET);
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}





uint32_t Advalue_SUM(uint16_t *advalue,uint8_t num)
{

	uint32_t SUM=0;
	for(uint8_t i=0;i<num;i++)
	{
		SUM+=*advalue++;
	}
	return SUM;
}