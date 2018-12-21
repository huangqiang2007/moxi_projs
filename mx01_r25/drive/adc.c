
#include "adc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define ADC1_DR_Address                0x40012440
#define ADC_USE_WDT

//-->define in C/C++ Preprocessor Symbols Define
#ifdef ADC_USE_DMA
#define DMA_DATA_LEN	4
__IO uint16_t RegularConvData_Tab[DMA_DATA_LEN];
#endif
//old board
//PA4 -- TEMP_DECT           adc_in4
//PB0--  LV_DETECT          adc_in8
//PB1--  LOAD_DETECT      adc_in9
//PA6 -- BAT TEMP		 adc_in6

//new board
//PA0 -- BAT_TEMP            adc_in0
//PA1--  BAT_VOL          	adc_in1
//PA2--  LOAD_DETECT      adc_in2
//PA3 -- MB_TEMP		 adc_in3



int adc_temp =0;	//-->
int adc_lv=0;
int adc_load=0;
int adc_innertemp=0;
//void ADC1_DMA_Init(void)
//{
//  ADC_InitTypeDef     ADC_InitStruct;
//  DMA_InitTypeDef     DMA_InitStruct;
//	GPIO_InitTypeDef    GPIO_InitStruct;
//
//
//	/* Enable  GPIOA clock */
//	  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
//  /* ADC1 Periph clock enable */
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
//
//  /* Configure PA.01  as analog input */
//  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
//  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
//  GPIO_Init(GPIOA, &GPIO_InitStruct);				// PC1,输入时不用设置速率
//  /* ADC1 DeInit */
//  ADC_DeInit(ADC1);
//
//  /* Initialize ADC structure */
//  ADC_StructInit(&ADC_InitStruct);
//
//  /* Configure the ADC1 in continous mode withe a resolutuion equal to 12 bits  */
//  ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;
//  ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
//  ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
//  ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
//  ADC_InitStruct.ADC_ScanDirection = ADC_ScanDirection_Backward;
//  ADC_Init(ADC1, &ADC_InitStruct);
//
//  /* Convert the ADC1 Channel 11 with 239.5 Cycles as sampling time */
//#ifdef USE_STM320518_EVAL
//  ADC_ChannelConfig(ADC1, ADC_Channel_11 , ADC_SampleTime_55_5Cycles);
//#else
//  ADC_ChannelConfig(ADC1, ADC_Channel_0 , ADC_SampleTime_55_5Cycles);
//#endif /* USE_STM320518_EVAL */
//
//  /* Convert the ADC1 temperature sensor  with 55.5 Cycles as sampling time */
//  ADC_ChannelConfig(ADC1, ADC_Channel_TempSensor , ADC_SampleTime_55_5Cycles);
//  ADC_TempSensorCmd(ENABLE);
//
//  /* Convert the ADC1 Vref  with 55.5 Cycles as sampling time */
//  ADC_ChannelConfig(ADC1, ADC_Channel_Vrefint , ADC_SampleTime_55_5Cycles);
//  ADC_VrefintCmd(ENABLE);
//
//  /* Convert the ADC1 Vbat with 55.5 Cycles as sampling time */
//  ADC_ChannelConfig(ADC1, ADC_Channel_Vbat , ADC_SampleTime_55_5Cycles);
//  ADC_VbatCmd(ENABLE);
//
//  /* ADC Calibration */
//  ADC_GetCalibrationFactor(ADC1);
//
//  /* ADC DMA request in circular mode */
//  ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);
//
//  /* Enable ADC_DMA */
//  ADC_DMACmd(ADC1, ENABLE);
//
//  /* Enable the ADC peripheral */
//  ADC_Cmd(ADC1, ENABLE);
//
//  /* Wait the ADRDY flag */
//  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY));
//
//
//}

/**
  * @brief  DMA channel1 configuration
  * @param  None
  * @retval None
  */
void DMA_Config(void)
{
  
    DMA_InitTypeDef DMA_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

  /* DMA1 clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE);

  //ADC_DMA_IRQ define in C/C++ Preprocessor Symbols Define   
#ifdef ADC_DMA_IRQ
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;	
	//NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif

  /* DMA1 Channel1 Config */
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)RegularConvData_Tab;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = DMA_DATA_LEN;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
#ifdef ADC_DMA_IRQ
  DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);//配置DMA1通道1传输完成中断   
#endif
  /* DMA1 Channel1 enable */
  DMA_Cmd(DMA1_Channel1, ENABLE);
}

void ADC_Config(void)
{
  ADC_InitTypeDef     ADC_InitStructure;
  GPIO_InitTypeDef    GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
  /* GPIOC Periph clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  
  /* ADC1 Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  
  /* Configure ADC Channel11 as analog input */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ;//ch0
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ;//ch1
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 ;//ch2
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 ;//ch3
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* ADCs DeInit */  
  ADC_DeInit(ADC1);
  
  /* Initialize ADC structure */
  ADC_StructInit(&ADC_InitStructure);
  
  /* Configure the ADC1 in continuous mode with a resolution equal to 12 bits  */  
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; 
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Backward;
  ADC_Init(ADC1, &ADC_InitStructure);
  
  ADC_ChannelConfig(ADC1, ADC_Channel_0 , ADC_SampleTime_239_5Cycles);//For PA0
  ADC_ChannelConfig(ADC1, ADC_Channel_1 , ADC_SampleTime_239_5Cycles);//For PA1
  ADC_ChannelConfig(ADC1, ADC_Channel_2 , ADC_SampleTime_239_5Cycles);//For PA2
  ADC_ChannelConfig(ADC1, ADC_Channel_3 , ADC_SampleTime_239_5Cycles);//For PA3
/*ADC BAT ETC...*/
    /* Convert the ADC1 temperature sensor  with 55.5 Cycles as sampling time */ 
//  ADC_ChannelConfig(ADC1, ADC_Channel_TempSensor , ADC_SampleTime_55_5Cycles);
//  ADC_TempSensorCmd(ENABLE);
//  /* Convert the ADC1 Vref  with 55.5 Cycles as sampling time */
//  ADC_ChannelConfig(ADC1, ADC_Channel_Vrefint , ADC_SampleTime_55_5Cycles);
//  ADC_VrefintCmd(ENABLE);
//
//  /* Convert the ADC1 Vbat with 55.5 Cycles as sampling time */
//  ADC_ChannelConfig(ADC1, ADC_Channel_Vbat , ADC_SampleTime_55_5Cycles);
//  ADC_VbatCmd(ENABLE);

#ifdef ADC_USE_WDT
  /* Analog watchdog config ******************************************/
  /* Configure the ADC Thresholds between 1.5V and 2.95V (1861, 3102) */
  //ADC_AnalogWatchdogThresholdsConfig(ADC1, 4026, 2047);
  //ADC detect pin can directly detect up to 3.0V voltage,that is 0xFFF value,here use two resistances to devider voltage,
  //RUP is 5.6k,RDOWN is 10k,so this adc can detect voltage range is below 3*15.6/10=4.68v
  //when vbat is 4.68v,adc value shall be 0xFFF(4095) ,Vbat=3.4v ,adc value 0xba0(2976),Vbat=4.2v ,adc value 0xe6b(3691),Vbat=3.6v ,adc value 0xc57(3159),
  //because the ADC sample value has fluctuation in a range(=-0.1V),so we may set the LowThreshold lower than 3.4V,here we set to 3.3v -->0xb50(2896)
  ADC_AnalogWatchdogThresholdsConfig(ADC1, 4094, 2896);

  /* Enable the ADC1 single channel  */
  ADC_AnalogWatchdogSingleChannelCmd(ADC1, ENABLE);
  
  ADC_OverrunModeCmd(ADC1, ENABLE);
  /* Enable the ADC1 analog watchdog */
  ADC_AnalogWatchdogCmd(ADC1,ENABLE);
  
  /* Select a single ADC1 channel 1 */
  ADC_AnalogWatchdogSingleChannelConfig(ADC1, ADC_AnalogWatchdog_Channel_1);

  /* Enable AWD interrupt */
  ADC_ITConfig(ADC1, ADC_IT_AWD, ENABLE);
  
  /* Configure and enable ADC1 interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = ADC1_COMP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif
  /* ADC Calibration */
  ADC_GetCalibrationFactor(ADC1);

#ifdef ADC_USE_DMA
  /* ADC DMA request in circular mode */
  ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);
  
  /* Enable ADC_DMA */
  ADC_DMACmd(ADC1, ENABLE);  
#endif
  
  /* Enable the ADC peripheral */
  ADC_Cmd(ADC1, ENABLE);
  
  /* Wait the ADRDY flag */
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY)); 
  
  /* ADC1 regular Software Start Conv */ 
  ADC_StartOfConversion(ADC1);
  
}

void adc_it_close(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	/* Enable AWD interrupt */
	ADC_ITConfig(ADC1, ADC_IT_AWD, DISABLE);

	/* Configure and enable ADC1 interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = ADC1_COMP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void adc_it_open(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	/* Enable AWD interrupt */
	ADC_ITConfig(ADC1, ADC_IT_AWD, ENABLE);

	/* Configure and enable ADC1 interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = ADC1_COMP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

int adctemp_result()
{
	return adc_temp;
}

int adclv_result()
{
	return adc_lv;
}

int adcload_result()
{
	return adc_load;
}

int adc_result(void)
{
	//uint16_t ADC1ConvertedValue=0;
	uint16_t ADC1ConvertedVoltage=0;
	uint8_t index;
	//uint16_t sensor,vbat,temp,vol;
#ifdef ADC_USE_DMA


    /* Test DMA1 TC flag */
    while((DMA_GetFlagStatus(DMA1_FLAG_TC1)) == RESET );     
    /* Clear DMA TC flag */
    DMA_ClearFlag(DMA1_FLAG_TC1);


  for(index=0;index<4;index++)
  {
		switch(index)
		{
			case 0:
				adc_innertemp= RegularConvData_Tab[index]* 3000 / 0xFFF;
				break;
			case 1:
				adc_load= RegularConvData_Tab[index]* 3000 / 0xFFF;
				break;
			case 2:
				adc_lv= RegularConvData_Tab[index]* 3000 / 0xFFF;
				break;
			case 3:
				adc_temp= RegularConvData_Tab[index]* 3000 / 0xFFF;
				break;
			default:
				break;
		}
	}

#else
    /* Test EOC flag */
    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
    
    /* Get ADC1 converted data */
    ADC1ConvertedValue =ADC_GetConversionValue(ADC1);
    
    /* Compute the voltage */
    ADC1ConvertedVoltage = (ADC1ConvertedValue *3000)/0xFFF;
#endif

	return ADC1ConvertedVoltage;
}
