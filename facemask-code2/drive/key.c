#include "key.h"
#include "stm32f0xx.h"
#include "time.h"
#include "led.h"
#include "adc.h"
#include "global.h"
#include "uart.h"
#include "string.h"
#include "delay.h"
#include <stdio.h>


/* keys define */
#define KEY_SYSTEM_ONOFF_POWER_PIN     GPIO_Pin_4 
#define KEY_TIME_SELECT_PIN      GPIO_Pin_3

uint8_t Long_Press_Flag =0; // 0: No long press; 1: long press.
uint8_t System_ON_OFF = 0;
uint8_t Power_rank_flag = 0; //0: rank1 ; 1: rank2 ; 2: rank3
uint8_t Power_change_flag = 0; //0: no change ; 1: changed

/* global variables from other file. */
extern char ADC_ON_OFF; // from adc.c
extern char ADC_END;    // from adc.c
extern float POWER_RANK[]; // from adc.c
extern float CURRENT_POWER; //from adc.c
extern uint8_t R_value_New;  //from adc.c
extern uint8_t R_value_Old;   //from adc.c
extern uint16_t TimerCounter; //from main.c
extern uint8_t Timing_flag;  // from main.c
extern uint8_t Enter_StopMode_flag;

extern uint8_t TimeOut_flag;
extern uint8_t Res_Value_Exception_flag;
extern uint8_t Output_Voltage_exception_flag;
extern uint8_t LOAD_DETECT_exception_flag;
extern uint8_t VIN_DETECT_exception_flag;
extern uint8_t No_connect_loader_close_flag;
extern uint8_t No_connect_loader_flag;
extern uint8_t No_connect_loader_start_flag;

extern int No_connect_loader_start_counter;
extern int connect_loader_start_counter;

void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4 ;  //PB4:TEMP_SELECT; PB3:TIME_SELECT
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_3;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
}
	
void Key_Delay(uint32_t temp)
{
	for(; temp!= 0; temp--);
} 

uint8_t KEY_Down(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin)
{
	/* 检测是否有按键按下 */
	if(GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == 0 ) {
		/*延时消抖*/
		Key_Delay(10000);
		if(GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == 0 ) {
			/*等待按键释放 */
			while(GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == 0);
			return 0;
		} else
			return 1;
	} else
		return 1;
}

uint8_t KEY_System_ON_OFF_Down(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin)
{			
	uint32_t  Long_Press_Counter = 0;

	Long_Press_Counter = 0;

	/* 检测是否有按键按下 */
	if(GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == 0 ) {
		/* 延时消抖 */
		Key_Delay(10000);
		if(GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == 0 )	{
			/* 等待按键释放 */
			while(GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == 0) {
				if (Long_Press_Counter < 700000) {
					Long_Press_Counter++;
					//printf("\r\n [INFO]  Long_Press_Flag = 0 (key.c-120 line)\r\n");
				} else {
					Long_Press_Flag = 1;
					//printf("\r\n [INFO]  Long_Press_Flag = 1 (key.c-123 line)\r\n");
					break;
				}
			}

			return 0;
		} else
			return 1;
	}else
		return 1;
}

void CLOSE_SYSTEM(void)
{
	System_ON_OFF  = 0;
	Long_Press_Flag = 0;
	TimeOut_flag = 0;
	Res_Value_Exception_flag = 0;
	Output_Voltage_exception_flag = 0;
	LOAD_DETECT_exception_flag = 0;
	VIN_DETECT_exception_flag = 0;
	No_connect_loader_close_flag = 0;
	No_connect_loader_start_flag = 0;
	Close_CMOS(); //close cmos
	All_LED_Close(); //close all led lights.
	TIM1_Disable(); // close timer 1 
	Close_ADC();   //close adc
	PWM_Channel4_Disable(); // close PWM
	R_value_New = 255; //  current resistance value of product.(default)
	R_value_Old = 254;  //  last resistance value of product.(default)
	printf("\r\n [INFO]  CLOSE_SYSTEM (key.c-145 line)\r\n");
	printf("\r\n [INFO]  R_value_New=%d , R_value_Old =%d (key.c-162 line)\r\n", R_value_New,R_value_Old);
	delay_ms(50);
}

void START_SYSTEM(void)
{
	int i = 0;

	Timing_flag = 0 ; /* 定时10分钟 */
	TimerCounter = 0;  /* 定时计算器归零 */
	Power_rank_flag = 0; /* 功率挡位设置为第一档 */
	ADC_END = 0; /* 设置ADC转换未结束 */
	TimeOut_flag = 0; /* 定时时间到标识 */
	Res_Value_Exception_flag = 0; /* 产品电阻值错误标识 */
	Output_Voltage_exception_flag = 0; /* 输出电压异常标识 */
	LOAD_DETECT_exception_flag = 0;
	VIN_DETECT_exception_flag = 0;
	No_connect_loader_close_flag = 0;
	No_connect_loader_start_flag = 0;
	printf("\r\n [INFO]  R_value_New=%d , R_value_Old =%d (key.c-185 line)  \r\n", R_value_New,R_value_Old);

	Open_CMOS(); //open cmos
	Open_ADC(); //打开ADC

	for(i = 0; i < 7; i++) { /* 取前7次的转换结果 */
		while(ADC_END == 0); /* 等待ADC转换完成 */
		DMA1_ADRRESS();
		ADC_END = 0;
	}

	if(LOAD_DETECT_exception_flag == 1) {
		Close_CMOS(); //close cmos
	} else {
		delay_ms(50);
		No_connect_loader_start_counter = 0;
		connect_loader_start_counter = 0;

		for(i = 0; i < 7; i++) { /* 取前7次的转换结果 */
			while(ADC_END == 0);  /* 等待ADC转换完成 */
			DMA1_ADRRESS();
			ADC_END = 0;
		}

		if (connect_loader_start_counter >= No_connect_loader_start_counter)
			No_connect_loader_start_flag = 0;
		else
			No_connect_loader_start_flag = 1;

		//printf("\r\n [INFO]  No_connect_loader_start_counter=%d , connect_loader_start_counter =%d (key.c-167 line)  \r\n",
		//	No_connect_loader_start_counter,connect_loader_start_counter);

		TIM1_Enable();  /* 打开定时器，开始计时 */

		if (No_connect_loader_start_flag == 0) {
			Temp_One_LED_Open(GPIOA, GPIO_Pin_4);  /* 点亮功率第一档灯 */
			Timer_One_LED_Open(GPIOA, GPIO_Pin_7); /* 点亮定时10分钟灯 */
			No_connect_loader_start_flag = 1;
		} else { /* 未连接负载，两红灯打开 */
			Timer_One_LED_Open(GPIOA, GPIO_Pin_11);
			Temp_One_LED_Open(GPIOA, GPIO_Pin_6);
		}

		No_connect_loader_start_counter = 0;
		connect_loader_start_counter = 0;
	}
}

void KEY_ADDRESS(void)
{
	if(LOAD_DETECT_exception_flag == 1 || No_connect_loader_close_flag == 1 ||
		TimeOut_flag == 1 || Res_Value_Exception_flag == 1 ||
		Output_Voltage_exception_flag == 1 || VIN_DETECT_exception_flag == 1) {
		printf("\r\n [INFO] TimeOut_flag =%d,Output_Voltage_exception_flag =%d(key.c-368 line)\r\n",TimeOut_flag,Output_Voltage_exception_flag);
		printf("\r\n [INFO] No_connect_loader_close_flag =%d,Res_Value_Exception_flag =%d(key.c-368 line)  \r\n",No_connect_loader_close_flag,Res_Value_Exception_flag);
		printf("\r\n [INFO] LOAD_DETECT_exception_flag =%d,VIN_DETECT_exception_flag =%d(key.c-368 line)  \r\n",LOAD_DETECT_exception_flag,VIN_DETECT_exception_flag);
		CLOSE_SYSTEM();
	}

	if( KEY_System_ON_OFF_Down(GPIOB, KEY_SYSTEM_ONOFF_POWER_PIN) == 0) { //判定系统开关或功率键是否按下
		if (Long_Press_Flag == 1) { /* 长按处理 */
			printf("\r\n [INFO] 长按处理。(key.c-263 line)\r\n");
			Long_Press_Flag = 0;
			System_ON_OFF = (System_ON_OFF+1)%2;

			if (System_ON_OFF == 0) { /* 系统关机 */
				All_LED_Close(); //close all led lights.

				/* 等待按键释放 */
				while(GPIO_ReadInputDataBit(GPIOB, KEY_SYSTEM_ONOFF_POWER_PIN) == 0);
				CLOSE_SYSTEM();
			} else { /* 系统开机 */
				START_SYSTEM();
				printf("\r\n [INFO] START_SYSTEM  (key.c-279 line)  \r\n");
				Enter_StopMode_flag =0;
				//printf(Tx_Buffer,"\r\n [INFO]  系统开机完成。(key.c-201 line)\r\n");

				/* 等待按键释放 */
				while(GPIO_ReadInputDataBit(GPIOB,KEY_SYSTEM_ONOFF_POWER_PIN) == 0);
			}
		} else { /* 短按处理，系统功率键按下 */
			if (Enter_StopMode_flag == 1) {
				//printf(Tx_Buffer,"\r\n [INFO]  Enter stop mode.(key.c-215 line)\r\n");
				CLOSE_SYSTEM();
			} else {
				if(No_connect_loader_flag ==0) {
					//printf(Tx_Buffer,"\r\n [INFO]  短按处理。(key.c-209 line)\r\n");
					if(System_ON_OFF == 1) { //处于开机状态时
						if(Power_rank_flag == 0) {
							Power_rank_flag++;
							Temp_One_LED_Open(GPIOA, GPIO_Pin_5);  //点亮功率第二档灯。-
							CURRENT_POWER = POWER_RANK[1];
							Power_change_flag = 1;
							//printf(Tx_Buffer,"\r\n [INFO]  点亮功率第二档灯。(key.c-219 line)\r\n");
						} else if (Power_rank_flag == 1) {
							Power_rank_flag++;
							Temp_One_LED_Open(GPIOA, GPIO_Pin_6);  //点亮功率第三档灯。
							CURRENT_POWER = POWER_RANK[2];
							Power_change_flag = 1;
							//printf(Tx_Buffer,"\r\n [INFO]  点亮功率第三档灯。(key.c-228 line)\r\n");
						} else if (Power_rank_flag == 2) {
							Power_rank_flag = 0;
							Temp_One_LED_Open(GPIOA, GPIO_Pin_4);  //点亮功率第一档灯。
							CURRENT_POWER = POWER_RANK[0];
							Power_change_flag = 1;
							//printf(Tx_Buffer,"\r\n [INFO]  点亮功率第一档灯。(key.c-237 line)\r\n");
						}
					}
				}
			}
		}
	}

	if(KEY_Down(GPIOB, KEY_TIME_SELECT_PIN) == 0 &&
		System_ON_OFF == 1 &&No_connect_loader_flag == 0) { /* 判定定时选择按键是否按下 */
		//printf(Tx_Buffer,"\r\n [INFO]  定时键被按下 (key.c-247 line)\r\n");
		TIM1_Disable(); /* 定时器停止工作 */
		TimerCounter = 0;  /* 定时计算器归零 */
		if(Timing_flag == 0) {
			Timing_flag++;
			Timer_One_LED_Open(GPIOA, GPIO_Pin_8); /* 点亮定时15分钟灯 */
			//printf(Tx_Buffer,"\r\n [INFO]  点亮定时15分钟灯  (key.c-255 line)\r\n");
		} else if(Timing_flag == 1) {
			Timing_flag++;
			Timer_One_LED_Open(GPIOA, GPIO_Pin_11); /* 点亮定时20分钟灯 */
			//printf(Tx_Buffer,"\r\n [INFO]  点亮定时20分钟灯  (key.c-262 line)\r\n");
		} else if(Timing_flag == 2 ) {
			Timing_flag = 0;
			Timer_One_LED_Open(GPIOA, GPIO_Pin_7); /* 点亮定时10分钟灯 */
			//printf(Tx_Buffer,"\r\n [INFO]  点亮定时10分钟灯  (key.c-269 line)  \r\n");
		}

		TIM1_Enable();  /* 打开定时器，开始计时 */
	}
}
