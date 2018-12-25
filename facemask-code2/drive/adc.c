#include "adc.h"
#include "global.h"
#include <stdio.h>
#include <math.h>
#include "uart.h"
#include "key.h"
#include "time.h"
#include "led.h"
#include "delay.h"
#include <string.h>
#include <stdlib.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ADC1_DR_Address                0x40012440   //ADC1的ADC_DR数据寄存器地址
#define AD_Count   100			//AD滤波采样次数	
//#define R_sample_count 10  //电阻值采样次数
//#define Vout_Vload_sample_count 20 
//#define R_ave_count 20

uint16_t ADCresults[4][AD_Count];//AD采集值
uint32_t AD_Data[4];      //AD采集值
uint32_t AD_Vol_Data[4];  //AD采集
//uint16_t VOUT_counter_value ; //产品的计算输出电压值
//uint16_t VOUT_sample_value ; //产品采样输出电压值
__IO uint16_t ADC_ConvertedValue[AD_Count][4];

//uint8_t R_Data[R_sample_count] = {0,0,0,0,0,0,0,0,0,0}; //电阻采集值
//uint8_t Order_R_Data[R_sample_count] = {0,0,0,0,0,0,0,0,0,0}; //电阻采集值(从小到大)

//uint16_t VOUT[Vout_Vload_sample_count];
//uint16_t VLOAD[Vout_Vload_sample_count];

//uint8_t R_count[R_ave_count];

//__IO uint16_t RegularConvData_Tab;
//extern uint8_t flag_ADC;

//extern uint8_t Power_rank_flag;

/*some exception flags*/
uint8_t Res_Value_Exception_flag = 0;
//uint8_t Output_Voltage_exception_flag = 0;
uint8_t LOAD_DETECT_exception_flag = 0;
uint8_t VIN_DETECT_exception_flag = 0;

volatile uint8_t No_connect_loader_flag = 0;
uint8_t No_connect_loader_counter = 0;

uint8_t Computer_Results_flage = 0;

uint8_t No_connect_loader_start_flag = 0;

uint8_t Alarm_flage = 0;

uint8_t DMA1_ADDRESS_flag = 0;

char ADC_ON_OFF = 2; //0 : ADC is ON ; 1: ADC is OFF; 2: other. it is used for extern file key.c 
char ADC_END = 0; //0 : ADC is not finished; 1: ADC is finished. it is used for extern file key.c 
//float POWER_RANK[3];
//char IS_FACE_EYE = 2; //0: is face mask;  1: is eys mask. 2：other
//float CURRENT_POWER = -1; //current power value including face and eye mask.
uint8_t R_value_New = 0; //  current resistance value of product.(default)
uint8_t R_value_Old = 0; //  last resistance value of product.(default)
uint8_t R_sample_current_count =0;
uint8_t VOUT_VLOAD_sample_count = 0;
uint8_t R_all_count = 0;

int No_connect_loader_start_counter = 0;
int connect_loader_start_counter = 0;

// global variables from other file. 
//extern char ADC_ON_OFF; // from adc.c
//extern char ADC_END;    // from adc.c
extern uint8_t Power_rank_flag;  //from main.c
extern uint16_t TimerCounter;
extern uint8_t Power_change_flag;
extern uint8_t Timing_flag; 
extern uint8_t System_ON_OFF;

uint16_t TimerCounter1 =0;


void ADC1_DMA_Init(void)
{
  ADC_InitTypeDef     ADC_InitStruct;
  DMA_InitTypeDef     DMA_InitStruct;
	GPIO_InitTypeDef    GPIO_InitStruct;
	NVIC_InitTypeDef  NVIC_InitStructure;
	  /* ADC1 DeInit */  
  ADC_DeInit(ADC1);

	/* Enable  GPIOA clock */
	  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  /* ADC1 Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  /* DMA1 clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE);

  /* Configure PA.00 and PA.01 and PA.02 and PA.03  as analog input */
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_InitStruct);				// PA1,输入时不用设置速率

 
  /* DMA1 Channel1 Config */
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_Address;
  DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValue;
  DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStruct.DMA_BufferSize =4 * AD_Count;
  DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStruct.DMA_Priority = DMA_Priority_High;
  DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStruct);
	
	/*清除一次DMA中断标志*/
	DMA_ClearITPendingBit(DMA1_IT_TC1);                                 
	
	/*使能DMA传输完成中断*/
	DMA_ITConfig(DMA1_Channel1, DMA1_IT_TC1, ENABLE); 
	
	/*选择DMA1通道中断*/
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;  

	/*中断使能*/ 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;   
	
	/*优先级设为0 */
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;  //自定义
	
	/*使能 DMA 中断*/ 
	NVIC_Init(&NVIC_InitStructure); 
  
  /* DMA1 Channel1 enable */
  DMA_Cmd(DMA1_Channel1, ENABLE);
  
//   /* ADC DMA request in circular mode */
  ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);
  
  /* Enable ADC_DMA */
  ADC_DMACmd(ADC1, ENABLE);  
  
  /* Initialize ADC structure */
  ADC_StructInit(&ADC_InitStruct);
  
  /* Configure the ADC1 in continous mode withe a resolutuion equal to 12 bits  */
  ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStruct.ADC_ContinuousConvMode = ENABLE; 
  ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStruct.ADC_ScanDirection = ADC_ScanDirection_Backward;
  ADC_Init(ADC1, &ADC_InitStruct); 
 
//   /* Convert the ADC1 temperature sensor  with 55.5 Cycles as sampling time */ 
//   ADC_ChannelConfig(ADC1, ADC_Channel_TempSensor , ADC_SampleTime_55_5Cycles);  
//   ADC_TempSensorCmd(ENABLE);

  ADC1->CHSELR = 0; //手动清除通道寄存器
  /* Convert the ADC1 Vref  with 55.5 Cycles as sampling time */ 
  ADC_ChannelConfig(ADC1, ADC_Channel_0 | ADC_Channel_1 | ADC_Channel_2| ADC_Channel_3 , ADC_SampleTime_239_5Cycles); 
	
//   ADC_VrefintCmd(ENABLE);
  
  /* ADC Calibration */
  ADC_GetCalibrationFactor(ADC1);
	/* 循环模式下的 ADC DMA 请求 */
	ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);
   ADC_DMACmd(ADC1, ENABLE);
	 
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);     
  
  /* Wait the ADCEN falg */
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADEN)); 
  
  /* ADC1 regular Software Start Conv */ 
  ADC_StartOfConversion(ADC1);
	
	ADC_ON_OFF = 1;
}

void Open_ADC(void)
{
	ADC1_DMA_Init();
}

void Close_ADC(void)
{
	/* Disable ADC1 */
	ADC_DMACmd(ADC1, DISABLE);
  ADC_Cmd(ADC1, DISABLE);
	ADC_ON_OFF = 0; // close ADC
	ADC_END = 0;  //  the convertion of ADC is not over.
}

/**
  * @brief  ADC滤波
  * @param  无
  * @retval 无
  */
void ADC_Filter(void)
{
	uint8_t i, j;
//	uint32_t temp=0;
	/*从DMA缓存中取出AD数据*/
	for(i=0; i<4; i++)
	{
		for(j=0; j<AD_Count; j++)
		{
			ADCresults[i][j] = ADC_ConvertedValue[j][i];	
		}	
	}

	/*取值求和取平均*/
	for(i=0; i<4; i++)
	{
		AD_Data[i] = 0;
		for(j=0; j<AD_Count; j++)
		{
			AD_Data[i] += ADCresults[i][j];
		}		
		AD_Data[i] = AD_Data[i] / AD_Count;
//		AD_Vol_Data[i] = AD_Data[i] *8/100;
	}
//		temp = AD_Vol_Data[3];
//		AD_Vol_Data[3] = AD_Vol_Data[1];
//	AD_Vol_Data[1] = temp;
}


void DMA1_ADRRESS(void)
{
if(DMA1_ADDRESS_flag==1)
{	
	
	ADC_Filter();
	
//	AD_Vol_Data[2] = AD_Data[2]*ADC_REF_convert;
//	AD_Vol_Data[3] = AD_Data[1]*ADC_REF_convert;
//	AD_Vol_Data[1] = AD_Data[3]*ADC_REF_convert;
	
	AD_Vol_Data[2] = AD_Data[2]*ADC_REF_convert;
	AD_Vol_Data[3] = AD_Data[1]*ADC_REF_convert;
	AD_Vol_Data[1] = AD_Data[3]*ADC_REF_convert;
	
//	printf("\r\n  [INFO] VOUT_DETECT=%d, LOAD_DETECT=%d.(adc.c-247 line)  \r\n",AD_Vol_Data[1],AD_Vol_Data[2]);
	
	//	AD_Vol_Data[1] = (AD_Data[3]*ADC_REF_VOLTAGE*VOLTAGE_ENLARGEMENT_FACTOR)/ADC_PRECISION;  //AD_Vol_Data[1]->VOUT_DETECT,放大1000倍
//		AD_Vol_Data[2] = (AD_Data[2]*ADC_REF_VOLTAGE*VOLTAGE_ENLARGEMENT_FACTOR)/ADC_PRECISION;  //AD_Vol_Data[2]->LOAD_DETECT,放大1000倍
//		AD_Vol_Data[3] = (AD_Data[1]*ADC_REF_VOLTAGE*VOLTAGE_ENLARGEMENT_FACTOR)/ADC_PRECISION;  //AD_Vol_Data[3]->VIN_DETECT,放大1000倍
//		sprintf(Tx_Buffer,"\r\n [INFO]  Enter adc AD_Vol_Data[2] =%d (adc.c-312 line)  \r\n",AD_Vol_Data[2]);
//		UART_Send( Tx_Buffer, (uint32_t) strlen(Tx_Buffer));
		if (No_connect_loader_start_flag == 0)
		{
			if(AD_Vol_Data[2] < 10) //No loader is connected.
			{
				No_connect_loader_start_counter++;
			}
			else connect_loader_start_counter++;
		}
		
		if (AD_Vol_Data[2] > LOAD_DETECT*VOLTAGE_ENLARGEMENT_FACTOR )//
	//	if (AD_Vol_Data[2] > 2900 )//
		{
//			printf("\r\n  [ERROR]  voltage value of  VIN_DETECT is %d, AD_Data[2]=%d.(adc.c-267 line)  \r\n",AD_Vol_Data[2],AD_Data[2]);
			LOAD_DETECT_exception_flag = 1;
		}
		
//		if (AD_Vol_Data[3] < VIN_DETECT_MIN*VOLTAGE_ENLARGEMENT_FACTOR || AD_Vol_Data[3] > VIN_DETECT_MAX*VOLTAGE_ENLARGEMENT_FACTOR) //输入电压保护
//		{
////			sprintf(Tx_Buffer,"\r\n  [ERROR]  voltage value of  VIN_DETECT is %d.(adc.c-338 line)  \r\n",AD_Vol_Data[3]);
////			UART_Send( Tx_Buffer, (uint32_t) strlen(Tx_Buffer));
//			printf("\r\n  [ERROR]  voltage value of  VIN_DETECT is %d,AD_Data[1]=%d.(adc.c-270 line)  \r\n",AD_Vol_Data[3],AD_Data[1]);
//			VIN_DETECT_exception_flag = 1;
//		}
	if (AD_Vol_Data[3] < VIN_DETECT_MIN*VOLTAGE_ENLARGEMENT_FACTOR) //输入电压保护
	//			if (AD_Vol_Data[3] < 1500) //输入电压保护
		{
			printf("\r\n  [ERROR]  voltage value of  VIN_DETECT is %d, AD_Data[1]=%d.(adc.c-280 line)  \r\n",AD_Vol_Data[3],AD_Data[1]);
			VIN_DETECT_exception_flag = 1;
		}
			
		if ( AD_Vol_Data[3] > VIN_DETECT_MAX*VOLTAGE_ENLARGEMENT_FACTOR) //输入电压保护
//			if ( AD_Vol_Data[3] > 2650) //输入电压保护
		{
			printf("\r\n  [ERROR]  voltage value of  VIN_DETECT is %d, AD_Data[1]=%d.(adc.c-287 line)  \r\n",AD_Vol_Data[3],AD_Data[1]);
			VIN_DETECT_exception_flag = 1;
		}
		DMA1_ADDRESS_flag=0;
	}
}

/**
  * @brief  DMA1_Channel1中断服务函数
  * @param  无
  * @retval 无
  */
void DMA1_Channel1_IRQHandler()  
{  
	/*判断DMA传输完成中断*/ 
	if(DMA_GetITStatus(DMA1_IT_TC1) != RESET)                        
	{ 
		DMA1_ADDRESS_flag =1;
		ADC_END = 1;	//ADC is finished.
	}
//	ADC_END = 1;	//ADC is finished.	
	
	/*清除DMA中断标志位*/	
	DMA_ClearITPendingBit(DMA1_IT_TC1);                      
}  

