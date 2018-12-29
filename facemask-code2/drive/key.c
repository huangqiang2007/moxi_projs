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
	/* ����Ƿ��а������� */
	if(GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == 0 ) {
		/*��ʱ����*/
		Key_Delay(10000);
		if(GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == 0 ) {
			/*�ȴ������ͷ� */
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

	/* ����Ƿ��а������� */
	if(GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == 0 ) {
		/* ��ʱ���� */
		Key_Delay(10000);
		if(GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == 0 )	{
			/* �ȴ������ͷ� */
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

	Timing_flag = 0 ; /* ��ʱ10���� */
	TimerCounter = 0;  /* ��ʱ���������� */
	Power_rank_flag = 0; /* ���ʵ�λ����Ϊ��һ�� */
	ADC_END = 0; /* ����ADCת��δ���� */
	TimeOut_flag = 0; /* ��ʱʱ�䵽��ʶ */
	Res_Value_Exception_flag = 0; /* ��Ʒ����ֵ�����ʶ */
	Output_Voltage_exception_flag = 0; /* �����ѹ�쳣��ʶ */
	LOAD_DETECT_exception_flag = 0;
	VIN_DETECT_exception_flag = 0;
	No_connect_loader_close_flag = 0;
	No_connect_loader_start_flag = 0;
	printf("\r\n [INFO]  R_value_New=%d , R_value_Old =%d (key.c-185 line)  \r\n", R_value_New,R_value_Old);

	Open_CMOS(); //open cmos
	Open_ADC(); //��ADC

	for(i = 0; i < 7; i++) { /* ȡǰ7�ε�ת����� */
		while(ADC_END == 0); /* �ȴ�ADCת����� */
		DMA1_ADRRESS();
		ADC_END = 0;
	}

	if(LOAD_DETECT_exception_flag == 1) {
		Close_CMOS(); //close cmos
	} else {
		delay_ms(50);
		No_connect_loader_start_counter = 0;
		connect_loader_start_counter = 0;

		for(i = 0; i < 7; i++) { /* ȡǰ7�ε�ת����� */
			while(ADC_END == 0);  /* �ȴ�ADCת����� */
			DMA1_ADRRESS();
			ADC_END = 0;
		}

		if (connect_loader_start_counter >= No_connect_loader_start_counter)
			No_connect_loader_start_flag = 0;
		else
			No_connect_loader_start_flag = 1;

		//printf("\r\n [INFO]  No_connect_loader_start_counter=%d , connect_loader_start_counter =%d (key.c-167 line)  \r\n",
		//	No_connect_loader_start_counter,connect_loader_start_counter);

		TIM1_Enable();  /* �򿪶�ʱ������ʼ��ʱ */

		if (No_connect_loader_start_flag == 0) {
			Temp_One_LED_Open(GPIOA, GPIO_Pin_4);  /* �������ʵ�һ���� */
			Timer_One_LED_Open(GPIOA, GPIO_Pin_7); /* ������ʱ10���ӵ� */
			No_connect_loader_start_flag = 1;
		} else { /* δ���Ӹ��أ�����ƴ� */
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

	if( KEY_System_ON_OFF_Down(GPIOB, KEY_SYSTEM_ONOFF_POWER_PIN) == 0) { //�ж�ϵͳ���ػ��ʼ��Ƿ���
		if (Long_Press_Flag == 1) { /* �������� */
			printf("\r\n [INFO] ��������(key.c-263 line)\r\n");
			Long_Press_Flag = 0;
			System_ON_OFF = (System_ON_OFF+1)%2;

			if (System_ON_OFF == 0) { /* ϵͳ�ػ� */
				All_LED_Close(); //close all led lights.

				/* �ȴ������ͷ� */
				while(GPIO_ReadInputDataBit(GPIOB, KEY_SYSTEM_ONOFF_POWER_PIN) == 0);
				CLOSE_SYSTEM();
			} else { /* ϵͳ���� */
				START_SYSTEM();
				printf("\r\n [INFO] START_SYSTEM  (key.c-279 line)  \r\n");
				Enter_StopMode_flag =0;
				//printf(Tx_Buffer,"\r\n [INFO]  ϵͳ������ɡ�(key.c-201 line)\r\n");

				/* �ȴ������ͷ� */
				while(GPIO_ReadInputDataBit(GPIOB,KEY_SYSTEM_ONOFF_POWER_PIN) == 0);
			}
		} else { /* �̰�����ϵͳ���ʼ����� */
			if (Enter_StopMode_flag == 1) {
				//printf(Tx_Buffer,"\r\n [INFO]  Enter stop mode.(key.c-215 line)\r\n");
				CLOSE_SYSTEM();
			} else {
				if(No_connect_loader_flag ==0) {
					//printf(Tx_Buffer,"\r\n [INFO]  �̰�����(key.c-209 line)\r\n");
					if(System_ON_OFF == 1) { //���ڿ���״̬ʱ
						if(Power_rank_flag == 0) {
							Power_rank_flag++;
							Temp_One_LED_Open(GPIOA, GPIO_Pin_5);  //�������ʵڶ����ơ�-
							CURRENT_POWER = POWER_RANK[1];
							Power_change_flag = 1;
							//printf(Tx_Buffer,"\r\n [INFO]  �������ʵڶ����ơ�(key.c-219 line)\r\n");
						} else if (Power_rank_flag == 1) {
							Power_rank_flag++;
							Temp_One_LED_Open(GPIOA, GPIO_Pin_6);  //�������ʵ������ơ�
							CURRENT_POWER = POWER_RANK[2];
							Power_change_flag = 1;
							//printf(Tx_Buffer,"\r\n [INFO]  �������ʵ������ơ�(key.c-228 line)\r\n");
						} else if (Power_rank_flag == 2) {
							Power_rank_flag = 0;
							Temp_One_LED_Open(GPIOA, GPIO_Pin_4);  //�������ʵ�һ���ơ�
							CURRENT_POWER = POWER_RANK[0];
							Power_change_flag = 1;
							//printf(Tx_Buffer,"\r\n [INFO]  �������ʵ�һ���ơ�(key.c-237 line)\r\n");
						}
					}
				}
			}
		}
	}

	if(KEY_Down(GPIOB, KEY_TIME_SELECT_PIN) == 0 &&
		System_ON_OFF == 1 &&No_connect_loader_flag == 0) { /* �ж���ʱѡ�񰴼��Ƿ��� */
		//printf(Tx_Buffer,"\r\n [INFO]  ��ʱ�������� (key.c-247 line)\r\n");
		TIM1_Disable(); /* ��ʱ��ֹͣ���� */
		TimerCounter = 0;  /* ��ʱ���������� */
		if(Timing_flag == 0) {
			Timing_flag++;
			Timer_One_LED_Open(GPIOA, GPIO_Pin_8); /* ������ʱ15���ӵ� */
			//printf(Tx_Buffer,"\r\n [INFO]  ������ʱ15���ӵ�  (key.c-255 line)\r\n");
		} else if(Timing_flag == 1) {
			Timing_flag++;
			Timer_One_LED_Open(GPIOA, GPIO_Pin_11); /* ������ʱ20���ӵ� */
			//printf(Tx_Buffer,"\r\n [INFO]  ������ʱ20���ӵ�  (key.c-262 line)\r\n");
		} else if(Timing_flag == 2 ) {
			Timing_flag = 0;
			Timer_One_LED_Open(GPIOA, GPIO_Pin_7); /* ������ʱ10���ӵ� */
			//printf(Tx_Buffer,"\r\n [INFO]  ������ʱ10���ӵ�  (key.c-269 line)  \r\n");
		}

		TIM1_Enable();  /* �򿪶�ʱ������ʼ��ʱ */
	}
}
