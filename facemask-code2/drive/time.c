#include "time.h"
#include "led.h"
#include "global.h"
#include "uart.h"
#include "24cxx.h"
#include "delay.h"
#include "key.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;

//for TIM1
//uint16_t temp1=0;
//static uint16_t TimerCounter = 0;
//extern uint8_t System_ON_OFF; // 0: system off; 1: system on.
//extern uint8_t Timing_flag; // 0: 10 minutes(defaults); 1: 15 minutes; 2: 20 minutes.

//for TIM3
	uint16_t TimerPeriod = 0;

	uint32_t _24c02_total_time = 0; //24c02的总时间长度

	uint16_t CURRENT_DUTY_PULSE = 0;  //当前占空比值
	uint16_t TimerCounter = 0;
	uint8_t Timing_flag = 0; 
	uint8_t TimeOut_flag = 0; 
	uint8_t No_connect_loader_close_flag = 0;
	uint8_t Output_Voltage_exception_flag = 0;
	char IS_FACE_EYE = 2; //0: is face mask;  1: is eys mask. 2：other
	float POWER_RANK[3];
	float CURRENT_POWER = -1; //current power value including face and eye mask.
	uint32_t VOUT_counter_value ; //产品的计算输出电压值
	uint32_t VOUT_sample_value ; //产品采样输出电压值
	uint8_t TIM1_ADRRESS_flag = 0;
	
// global variables from other file. 
extern char ADC_ON_OFF; //from adc.c
extern uint32_t WRITE_24C02_ADDR; // from 24cxx.c
//extern uint16_t TimerCounter;  //from main.c
extern uint8_t System_ON_OFF; // from main.c
//extern uint8_t Timing_flag;  // from main.c
extern uint8_t Power_change_flag; // from key.c
extern uint8_t R_value_Old;
extern uint8_t R_value_New;

extern uint8_t No_connect_loader_flag;
extern uint8_t No_connect_loader_counter;

extern uint32_t AD_Data[4];      //AD采集值
extern uint32_t AD_Vol_Data[4];  //AD采集
extern uint8_t Power_rank_flag;  //from main.c


//TIM1 配置
void TIM_INT_Config(void)
{
   NVIC_InitTypeDef NVIC_InitStructure;

  /* TIM1 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

  /*  TIM1 中断嵌套设计*/
  //NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_BRK_UP_TRG_COM_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;          /////自定义
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void TIM_OUT_Config(void)
{
	/* -----------------------------------------------------------------------
    TIM1
    
    TIM1输入时钟(TIM1CLK) 被设为APB2 时钟 (PCLK2),  
      => TIM1CLK = PCLK2 = SystemCoreClock = 48 MHz
          
    时间公式计算：每次进入中断服务程序间隔时间1秒
     ((1+TIM_Prescaler)/48M)*(1+TIM_Period)=((1+9599)/48M)*(1+9999)=1秒
  ----------------------------------------------------------------------- */   

  /* Time 定时器基础设置 */
	TIM_TimeBaseStructure.TIM_Period = 10000-1;
  //TIM_TimeBaseStructure.TIM_Prescaler = 9600-1;
	TIM_TimeBaseStructure.TIM_Prescaler = 4800-1;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  TIM_ClearFlag(TIM1,TIM_FLAG_Update);
	TIM_ARRPreloadConfig(TIM1,ENABLE);
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);
  /* TIM1 关闭*/
  TIM_Cmd(TIM1, DISABLE);
	}

void TIM1_Enable(void)
{
	/* TIM1 使能*/
  TIM_Cmd(TIM1, ENABLE);
}

void TIM1_Disable(void)
{
	/* TIM1 关闭*/
  TIM_Cmd(TIM1, DISABLE);
}


void TIM_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* 使能GPIO时钟 */
  //RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOB|RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOB, ENABLE);
  
  /* 配置GPIO管脚复用*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_InitStructure.GPIO_PuPd =GPIO_PuPd_DOWN;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_1);
	
	////////////////////////////////
	 /* 配置GPIO管脚复用*/
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
//  GPIO_Init(GPIOA, &GPIO_InitStructure);
//  
//  GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_1);
//  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_1);
  
}

void TIM_PWM_Config(void)
{
	/* TIM1 的配置 ---------------------------------------------------
   TIM1 输入时钟(TIM1CLK) 设置为 APB2 时钟 (PCLK2)    
    => TIM1CLK = PCLK2 = SystemCoreClock
   TIM1CLK = SystemCoreClock, Prescaler = 0, TIM1 counter clock = SystemCoreClock
   SystemCoreClock 为48 MHz 
   
   我们的目标产生 4 路PWM 信号在17.57 KHz:
     - TIM1_Period = (SystemCoreClock / 17570) - 1
   信道1设置的占空比为 50%
   信道2设置的占空比为 37.5%
   信道3设置的占空比为 25%
   信道4设置的占空比为 12.5%
   定时器脉冲的计算方式如下:
     - ChannelxPulse = DutyCycle * (TIM1_Period - 1) / 100
	*/  
    /*计算预定表的值，也就是多少个时钟计数为一个周期*/
  TimerPeriod = (SystemCoreClock / 17570 ) - 1;
//	/*计算CCR1 跳转值 在占空比为0%时*/
//  Channel0Pulse = (uint16_t) (((uint32_t) 100 * (TimerPeriod - 1)) / 100);
//	/*计算CCR1 跳转值 在占空比为25%时*/
//  Channel25Pulse = (uint16_t) (((uint32_t) 75 * (TimerPeriod - 1)) / 100);
//  /*计算CCR1 跳转值 在占空比为50%时*/
//  Channel50Pulse = (uint16_t) (((uint32_t) 5 * (TimerPeriod - 1)) / 10);
// /*计算CCR2 跳转值 在占空比为62.5%时*/
//  Channel625Pulse = (uint16_t) (((uint32_t) 375 * (TimerPeriod - 1)) / 1000);
//  /*计算CCR3 跳转值 在占空比为75%时*/
//  Channel75Pulse = (uint16_t) (((uint32_t) 25 * (TimerPeriod - 1)) / 100);
// /*计算CCR4跳转值 在占空比为87.5%时*/
//  Channel875Pulse = (uint16_t) (((uint32_t) 125 * (TimerPeriod- 1)) / 1000);
//	/*计算CCR4跳转值 在占空比为100%时*/
//  Channel100Pulse = (uint16_t) (((uint32_t) 0 * (TimerPeriod- 1)) / 1000);

  /* TIM3 时钟使能 */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);
  
  /* Time 定时基础设置*/
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  /* Time 定时设置为上升沿计算模式*/
  TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* 频道1，2，3，4的PWM 模式设置 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

//  TIM_OCInitStructure.TIM_Pulse = Channel25Pulse;//使能频道4配置
//  TIM_OC4Init(TIM3, &TIM_OCInitStructure);
	
	////////////////////////////////
//	TIM_OCInitStructure.TIM_Pulse = Channel4Pulse;//使能频道1配置
//  TIM_OC1Init(TIM3, &TIM_OCInitStructure);

//  TIM_OCInitStructure.TIM_Pulse = Channel1Pulse;//使能频道2配置
//  TIM_OC2Init(TIM3, &TIM_OCInitStructure);

	
  /* TIM3 计算器使能*/
//  TIM_Cmd(TIM3, ENABLE);
	
  /* TIM3 主输出使能 */
//  TIM_CtrlPWMOutputs(TIM3, ENABLE);	
	}

void PWM_Channel4_Disable(void)//关闭频道4
{
	/* TIM3 计算器关闭*/
  TIM_Cmd(TIM3, DISABLE);
	/* TIM3 主输出关闭 */
  TIM_CtrlPWMOutputs(TIM3, DISABLE);
	booter_Close();
}

void PWM_Channel_Init(void)
{
	//PWM_Channel4_Enable(Channel75Pulse);
	PWM_Channel4_Disable();
}

void PWM_Channel4_Enable(uint16_t ChannelXPulse)//使能频道4配置
{
	TIM_OCInitStructure.TIM_Pulse = ChannelXPulse;
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);
	
	/* TIM3 计算器使能*/
  TIM_Cmd(TIM3, ENABLE);
	/* TIM3 主输出使能 */
  TIM_CtrlPWMOutputs(TIM3, ENABLE);	
	
	booter_Open();
}

uint16_t PWM_Compute_Pulse(uint16_t duty)
{
	uint16_t Channel_Pulse = 0;
	/*计算预定表的值，也就是多少个时钟计数为一个周期*/
  TimerPeriod = (SystemCoreClock / 17570 ) - 1;
//	/*计算CCR2 跳转值 在占空比为62.5%时*/
//  Channel_Pulse = (uint16_t) (((uint32_t) 375 * (TimerPeriod - 1)) / 1000);
	Channel_Pulse = (uint16_t) (((uint32_t) (DUTY_ENLAR_FACTOR - duty) * (TimerPeriod - 1)) / DUTY_ENLAR_FACTOR);
	return Channel_Pulse;
}
	
void PWM_ADJUST(uint16_t VOUT_Sample_value,uint16_t VOUT_Counter_value,int Res_equal)
{
	uint16_t duty = 0;
	//int16_t v_error_duty = 0; //电压误差（设定值与实测值之间）
	
//	sprintf(Tx_Buffer,"\r\n  [INFO]  VOUT_counter_value is %d,  VOUT_sample_value is %d.Res_equal =% d (time.c-362 line) \r\n",VOUT_counter_value,VOUT_sample_value,Res_equal);
//	UART_Send( Tx_Buffer, (uint32_t) strlen(Tx_Buffer));
	
	//if(Res_equal>1 || Res_equal<-1 || Power_change_flag == 1)//上次测量产品电阻值等于当前测量产品电阻值；
	if(Res_equal !=0 || Power_change_flag == 1)//上次测量产品电阻值等于当前测量产品电阻值；
	{
		if (R_value_New > FACE_R_MAX)/// eye mask
		{
			if (VOUT_Counter_value > V_MAX *VOLTAGE_ENLARGEMENT_FACTOR)
			{
				CURRENT_DUTY_PULSE = DUTY_ENLAR_FACTOR;
			}
			else{
				//CURRENT_DUTY_PULSE = (((6*(VOUT_counter_value+650)/VOLTAGE_ENLARGEMENT_FACTOR)/27.951-1)/5)*DUTY_ENLAR_FACTOR; // 占空比放大了1000倍
//				CURRENT_DUTY_PULSE = (uint16_t) (((VOUT_Counter_value+550)*DUTY_ENLAR_FACTOR/VOLTAGE_ENLARGEMENT_FACTOR-27.951*DUTY_ENLAR_FACTOR/6)/(27.951*5/6));
				CURRENT_DUTY_PULSE = (uint16_t) (((VOUT_Counter_value-200)*DUTY_ENLAR_FACTOR/VOLTAGE_ENLARGEMENT_FACTOR-27.951*DUTY_ENLAR_FACTOR/6)/(27.951*5/6));
				if (CURRENT_DUTY_PULSE > DUTY_ENLAR_FACTOR) CURRENT_DUTY_PULSE = DUTY_ENLAR_FACTOR;
			}
//			sprintf(Tx_Buffer,"\r\n  [INFO] VOUT_counter_value+480 (time.c-362 line) \r\n");
//			UART_Send( Tx_Buffer, (uint32_t) strlen(Tx_Buffer));
		}
		else{   //face mask
			if (VOUT_Counter_value > V_MAX *VOLTAGE_ENLARGEMENT_FACTOR)
			{
				CURRENT_DUTY_PULSE = DUTY_ENLAR_FACTOR;
			}
			else{
//			CURRENT_DUTY_PULSE = (((6*(VOUT_counter_value+510)/VOLTAGE_ENLARGEMENT_FACTOR)/27.951-1)/5)*DUTY_ENLAR_FACTOR; // 占空比放大了1000倍
//				CURRENT_DUTY_PULSE = (uint16_t)(((VOUT_Counter_value+200)*DUTY_ENLAR_FACTOR/VOLTAGE_ENLARGEMENT_FACTOR-27.951*DUTY_ENLAR_FACTOR/6)/(27.951*5/6));//
				CURRENT_DUTY_PULSE = (uint16_t)(((VOUT_Counter_value-300)*DUTY_ENLAR_FACTOR/VOLTAGE_ENLARGEMENT_FACTOR-27.951*DUTY_ENLAR_FACTOR/6)/(27.951*5/6));//
				if (CURRENT_DUTY_PULSE > DUTY_ENLAR_FACTOR) CURRENT_DUTY_PULSE = DUTY_ENLAR_FACTOR;
				//if (CURRENT_DUTY_PULSE < 0) CURRENT_DUTY_PULSE = 0;
			}
//			sprintf(Tx_Buffer,"\r\n  [INFO] VOUT_counter_value+510 (time.c-362 line) \r\n");
//			UART_Send( Tx_Buffer, (uint32_t) strlen(Tx_Buffer));
		}
		//if (VOUT_counter_value>28*VOLTAGE_ENLARGEMENT_FACTOR) CURRENT_DUTY_PULSE = 1000;
		duty = PWM_Compute_Pulse(CURRENT_DUTY_PULSE);
//		sprintf(Tx_Buffer,"\r\n  [INFO]  This is first, duty is %d. (time.c-371 line)  \r\n",duty);
//		//sprintf(Tx_Buffer,"\r\n  [INFO]  This is first, CURRENT_DUTY_PULSE = %d (time.c-371 line)  \r\n",CURRENT_DUTY_PULSE);
//		UART_Send( Tx_Buffer, (uint32_t) strlen(Tx_Buffer));
//		sprintf(Tx_Buffer,"\r\n  [INFO]  VOUT_counter_value is %d,  VOUT_sample_value is %d. (time.c-381 line) \r\n",VOUT_counter_value,VOUT_sample_value);
//		UART_Send( Tx_Buffer, (uint32_t) strlen(Tx_Buffer));
		PWM_Channel4_Enable(duty);
		R_value_Old = R_value_New;
		Power_change_flag = 0;
	}
}

void TIM1_ADRRESS(void)
{
//	uint32_t counter1 = 0;
	
	if(TIM1_ADRRESS_flag==1)
{	
//	printf("\r\n [INFO]  TIM1_ADRRESS_flag = %d .(time.c-315 line)   \r\n",TIM1_ADRRESS_flag);
	TimerCounter++;
	TIM1_ADRRESS_flag = 0;
	if(No_connect_loader_flag ==1 )
		{
			
			if (System_ON_OFF == 1)
			{
				PWM_Channel4_Disable(); //关闭PWM
				if(No_connect_loader_counter % 2 == 0) //闪烁红灯
				{
					Timer_One_LED_Open(GPIOA,GPIO_Pin_11);
					Temp_One_LED_Open(GPIOA,GPIO_Pin_6);
				}
				else
					All_LED_Close();
			}
			
			No_connect_loader_counter++;
			if(No_connect_loader_counter == NO_CONNECT_LOADER_TIME)
			{
				No_connect_loader_close_flag = 1;
				No_connect_loader_counter = 0;	
			}
		}
		
		if (TimerCounter == TIME_MIN && Timing_flag==0) //定时10分钟到
		//if (TimerCounter == 10 && Timing_flag==0) //定时5秒到
		{
			TimerCounter = 0;
//			sprintf(Tx_Buffer,"\r\n [INFO]   定时10分钟到...(time.c-114 line) \r\n");
//			UART_Send( Tx_Buffer, (uint32_t) strlen(Tx_Buffer));
			TimeOut_flag = 1;//CLOSE_SYSTEM();
			
		}
		if (TimerCounter == TIME_MID && Timing_flag==1) //定时15分钟到
		//if (TimerCounter == 10 && Timing_flag==1) //定时10秒到
		{
			TimerCounter = 0;
//			sprintf(Tx_Buffer,"\r\n [INFO]   定时15分钟到...(time.c-129 line) \r\n");
//			UART_Send( Tx_Buffer, (uint32_t) strlen(Tx_Buffer));
			TimeOut_flag = 1;//CLOSE_SYSTEM();
			
		}
		if (TimerCounter == TIME_MAX && Timing_flag==2) //定时20分钟到
		//if (TimerCounter == 15 && Timing_flag==2) //定时15秒到
		{
			TimerCounter = 0;
//			sprintf(Tx_Buffer,"\r\n [INFO]   定时20分钟到...(time.c-141 line) \r\n");
//			UART_Send( Tx_Buffer, (uint32_t) strlen(Tx_Buffer));
			TimeOut_flag = 1;	//CLOSE_SYSTEM();
		
		}	
		if (TimerCounter > 1200 ) //定时异常处理
		{
			TimerCounter = 0;
		}

///////////PWM调节//////////////

//		AD_Vol_Data[2] = AD_Data[2]*ADC_REF_convert;
//		AD_Vol_Data[3] = AD_Data[1]*ADC_REF_convert;
//		AD_Vol_Data[1] = AD_Data[3]*ADC_REF_convert;
		
//	AD_Vol_Data[2] = ADC_REF_VOLTAGE *VOLTAGE_ENLARGEMENT_FACTOR;
//	AD_Vol_Data[2] = AD_Vol_Data[2] *AD_Data[2];
//	AD_Vol_Data[2] = AD_Vol_Data[2]/ADC_PRECISION;
//	
//	AD_Vol_Data[3] = ADC_REF_VOLTAGE *VOLTAGE_ENLARGEMENT_FACTOR;
//	AD_Vol_Data[3] = AD_Vol_Data[3] *AD_Data[1];
//	AD_Vol_Data[3] = AD_Vol_Data[3]/ADC_PRECISION;
//		
//	AD_Vol_Data[1] = ADC_REF_VOLTAGE *VOLTAGE_ENLARGEMENT_FACTOR;
//	AD_Vol_Data[1] = AD_Vol_Data[1] *AD_Data[3];
//	AD_Vol_Data[1] = AD_Vol_Data[1]/ADC_PRECISION;	
		
//	AD_Vol_Data[1] = (AD_Data[3]*ADC_REF_VOLTAGE*VOLTAGE_ENLARGEMENT_FACTOR)/ADC_PRECISION;  //AD_Vol_Data[1]->VOUT_DETECT,放大1000倍
//	AD_Vol_Data[2] = (AD_Data[2]*ADC_REF_VOLTAGE*VOLTAGE_ENLARGEMENT_FACTOR)/ADC_PRECISION;  //AD_Vol_Data[2]->LOAD_DETECT,放大1000倍
//	AD_Vol_Data[3] = (AD_Data[1]*ADC_REF_VOLTAGE*VOLTAGE_ENLARGEMENT_FACTOR)/ADC_PRECISION;  //AD_Vol_Data[3]->VIN_DETECT,放大1000倍	
	if (AD_Vol_Data[1] < VOUT_DETECT_MIN*VOLTAGE_ENLARGEMENT_FACTOR || AD_Vol_Data[1] > VOUT_DETECT_MAX*VOLTAGE_ENLARGEMENT_FACTOR)
		{			
//			sprintf(Tx_Buffer,"\r\n [ERROR]  Output voltage is exception.VOUT_DETECT = %d(adc.c-365 line)  \r\n",AD_Vol_Data[1]);
//			UART_Send( Tx_Buffer, (uint32_t) strlen(Tx_Buffer));
			printf("\r\n  [ERROR]   Output voltage is exception.VOUT_DETECT = %d,AD_Data[3] = %d  (time.c-378 line)  \r\n",AD_Vol_Data[1],AD_Data[3]);
			Output_Voltage_exception_flag = 1;
			//CLOSE_SYSTEM();
		}
		else{
			if (AD_Vol_Data[2] <= LOAD_DETECT*VOLTAGE_ENLARGEMENT_FACTOR && AD_Vol_Data[2]>10)
			{
				R_value_New  = R36_RATIO*AD_Vol_Data[1]/AD_Vol_Data[2]; //计算产品阻值；阻值精度为取整。

				if (R_value_New <45) 
				{
					R_value_New = R_value_New +2; //面罩阻值加3
//					if(R_value_New>26) R_value_New++;
				}
				else 
					R_value_New = R_value_New +3; //眼罩阻值加5
				printf("\r\n  [INFO] R_value_New=%d.(time.c-414 line)  \r\n",R_value_New);
					
				if (R_value_New < FACE_R_MIN) // loader(product) is not connected.
				{
					IS_FACE_EYE = 2; //neither it is face mask, nor it is eye mask. it is error.
//					sprintf(Tx_Buffer,"\r\n  [ERROR]  Resistance value of Product is 0....(adc.c-365 line)  \r\n");
//					UART_Send( Tx_Buffer, (uint32_t) strlen(Tx_Buffer));
//					sprintf(Tx_Buffer,"\r\n  [ERROR]  R_value_New = %d (adc.c-416 line)  \r\n",R_value_New);
//					UART_Send( Tx_Buffer, (uint32_t) strlen(Tx_Buffer));
					R_value_Old = 0;
					No_connect_loader_flag = 1;					
				}
				else 
				{
//					printf("\r\n [INFO]  R_value_New =%d .(time.c-423 line)   \r\n",R_value_New);
					if (No_connect_loader_flag == 1)//从未连接负载返回有负载连接
					{
						No_connect_loader_flag = 0;
						No_connect_loader_counter = 0;
						
						Power_rank_flag = 0;
						Temp_One_LED_Open(GPIOA,GPIO_Pin_4);  //点亮功率第一档灯。
						CURRENT_POWER = POWER_RANK[0];
						Power_change_flag = 1;
						
						Timing_flag = 0;
						Timer_One_LED_Open(GPIOA,GPIO_Pin_7); //点亮定时10分钟灯	
					}
					
					if (R_value_New != R_value_Old)  //产品阻值发生变化；重新进行产品初始化
					{
						if (R_value_New > EYE_R_MIN && R_value_New < EYE_R_MAX )
						{
							IS_FACE_EYE = 1; //大于55欧姆，为眼罩//0: is face mask;  1: is eys mask. 2：other
							POWER_RANK[0] = P_EYE_MIN;
							POWER_RANK[1] = P_EYE_MID;
							POWER_RANK[2] = P_EYE_MAX;
							if (Power_rank_flag ==0 ) CURRENT_POWER = POWER_RANK[0];
							if (Power_rank_flag ==1 ) CURRENT_POWER = POWER_RANK[1];
							if (Power_rank_flag ==2 ) CURRENT_POWER = POWER_RANK[2];
						}
						else if  (R_value_New > FACE_R_MIN && R_value_New < FACE_R_MAX)
						{
							IS_FACE_EYE = 0; // 小于45欧姆，为面罩//0: is face mask;  1: is eys mask. 2：other
							POWER_RANK[0] = P_FACE_MIN;
							POWER_RANK[1] = P_FACE_MID;
							POWER_RANK[2] = P_FACE_MAX;
							if (Power_rank_flag ==0 ) CURRENT_POWER = POWER_RANK[0];
							if (Power_rank_flag ==1 ) CURRENT_POWER = POWER_RANK[1];
							if (Power_rank_flag ==2 ) CURRENT_POWER = POWER_RANK[2];
						}
						else 
						{
//							IS_FACE_EYE = 2; //neither it is face mask, nor it is eye mask. it is error.
//							sprintf(Tx_Buffer,"\r\n  [ERROR]  Resistance value of Product is error.R_value_New =%d (adc.c-382 line)  \r\n",R_value_New);
//							UART_Send( Tx_Buffer, (uint32_t) strlen(Tx_Buffer));
//							Res_Value_Exception_flag = 1;
							//CLOSE_SYSTEM(); // CLOSE SYSTEM.		
						}
					}
				
				if(R_value_New > FACE_R_MIN && R_value_New < EYE_R_MAX)
				{
					if(CURRENT_POWER >= P_EYE_MIN && CURRENT_POWER <=P_FACE_MAX)//
					{
//						counter1 = sqrt(R_value_New * CURRENT_POWER);
//						counter1 = counter1 *1000;
//						VOUT_counter_value = counter1;
						VOUT_counter_value = sqrt(R_value_New * CURRENT_POWER)* VOLTAGE_ENLARGEMENT_FACTOR; //计算输出电压值,放大100倍（保留小数点后两位）（设定值）
//						printf("\r\n [INFO]  VOUT_counter_value =%d .(time.c-423 line)   \r\n",VOUT_counter_value);
						VOUT_sample_value  = AD_Vol_Data[1] * VOUT_VOUT_DETECT_RATIO; //计算采样输出电压值（实测值）
						
							PWM_ADJUST(VOUT_sample_value,VOUT_counter_value,R_value_Old - R_value_New);
							
					}
				}
			}
		}
	else
	{
	 No_connect_loader_flag = 1;
//	sprintf(Tx_Buffer,"\r\n [INFO] LOAD_DETECT is %d .(adc.c-499 line)  \r\n",AD_Vol_Data[2]);
//	UART_Send( Tx_Buffer, (uint32_t) strlen(Tx_Buffer));		
	}
}


////// write 24c02///////////////
		if(TimerCounter > 0 && TimerCounter % WRITE_24C02_PERIOD == 0)
		{
			if(1 == AT24CXX_Check())//检测24c02;//返回1:检测失败 //返回0:检测成功
			{
//				sprintf(Tx_Buffer,"\r\n [ERORR]  24C02 Check Failed!  Please Check! (time.c-154 line)\r\n");
//				UART_Send( Tx_Buffer, (uint32_t) strlen(Tx_Buffer));
				
			}
			else{
				_24c02_total_time = AT24CXX_ReadLenByte(WRITE_24C02_ADDR,4);//读出总时长
				

				if((_24c02_total_time & 0x80000000) > 0)
				{
					AT24CXX_WriteLenByte(WRITE_24C02_ADDR,1,4);	//第一次使用
//					sprintf(Tx_Buffer,"\r\n [INFO]   The first use of 24C02.........(time.c-191 line) \r\n");
//					UART_Send( Tx_Buffer, (uint32_t) strlen(Tx_Buffer));
				}
				else
				{	
					if(_24c02_total_time % _24C02_ERASE_MAX == 0)//用新单元存储总时长
					{
						WRITE_24C02_ADDR = WRITE_24C02_ADDR + 4;
						if(WRITE_24C02_ADDR == 256) WRITE_24C02_ADDR = 0;
						AT24CXX_WriteLenByte(0,WRITE_24C02_ADDR,4);	//写地址
						_24c02_total_time++;
						AT24CXX_WriteLenByte(WRITE_24C02_ADDR,_24c02_total_time,4);	//写总时长
//						sprintf(Tx_Buffer,"\r\n [INFO] WRITE_24C02_ADDR = %d, _24c02_total_time = %d .........(time.c-203 line) \r\n",WRITE_24C02_ADDR,_24c02_total_time);
//						UART_Send( Tx_Buffer, (uint32_t) strlen(Tx_Buffer));
					}
					else
					{
						_24c02_total_time++;
						AT24CXX_WriteLenByte(WRITE_24C02_ADDR,_24c02_total_time,4);	//写总时长
//						sprintf(Tx_Buffer,"\r\n [INFO] WRITE_24C02_ADDR = %d, _24c02_total_time = %d .........(time.c-210 line) \r\n",WRITE_24C02_ADDR,_24c02_total_time);
//						UART_Send( Tx_Buffer, (uint32_t) strlen(Tx_Buffer));
						printf("\r\n [INFO]  WRITE_24C02_ADDR = %d, _24c02_total_time = %d .........(time.c-537 line)    \r\n",WRITE_24C02_ADDR,_24c02_total_time);
					}
				}
			}
		}

	}
	
}

		//中断服务程序1秒进入一次
void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM1,TIM_IT_Update)!=RESET)
	{
		TIM_ClearITPendingBit(TIM1,TIM_FLAG_Update);
		
		TIM1_ADRRESS_flag = 1;
//		sprintf(Tx_Buffer,"\r\n [INFO]   in the timer...(time.c-107 line) \r\n");
//		UART_Send( Tx_Buffer, (uint32_t) strlen(Tx_Buffer));
		
	}
}




