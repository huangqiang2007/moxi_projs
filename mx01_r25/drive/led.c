/******************** (C) COPYRIGHT 2011 ������ ********************
 * �ļ���  ��led.c
 * ����    ��         
 * ʵ��ƽ̨�����stm32f051������
 * ����    ��led��������
 * ����    �����
 * ����    ��qfv5.taobao.com
**********************************************************************************/

#include "led.h"
/**********************************************/
/* �������ܣ� led�� ��ʼ��                     */
/* ��ڲ�������                               */
/**********************************************/
int wrn=0;


void LED_Init(void)
{
 	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	GPIO_InitStruct.GPIO_Pin = LED_TEMP1/*|LED_TEMP2*/|LED_TEMP3|LED_TEMP4|LED_TEMP5|LED_TIME1|LED_TIME2|LED_TIME3|LED_ERROR;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_1;		
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;		
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(LED_PORT, &GPIO_InitStruct);
	GPIO_SetBits(LED_PORT,LED_TEMP1/*|LED_TEMP2*/|LED_TEMP3|LED_TEMP4|LED_TEMP5|LED_TIME1|LED_TIME2|LED_TIME3|LED_ERROR);
	//GPIO_SetBits(LED_PORT, LED_TEMP1);
	
}

//close all green led(led1-led5)
void LevelLED_OffAll(void)
{
	GPIO_SetBits(LED_PORT,LED_TEMP1/*|LED_TEMP2*/|LED_TEMP3|LED_TEMP4|LED_TEMP5);
}

/**********************************************/
/* �������ܣ���led��                        */
/* ��ڲ�����led_num from 0-5                               */
/**********************************************/
void LevelLED_On(int num)
{
	//d3 -- p04 d4--p02 d5--p03 d6--p00 d7--p01
	switch(num)
	{
		case 0:
			LevelLED_OffAll();			
			break;
		case 1:
			GPIO_SetBits(LED_PORT,/*|LED_TEMP2*/LED_TEMP3|LED_TEMP4|LED_TEMP5);
			GPIO_ResetBits(LED_PORT, LED_TEMP1);
			break;
		case 2:
			GPIO_SetBits(LED_PORT,LED_TEMP1|LED_TEMP3|LED_TEMP4|LED_TEMP5);
			//GPIO_ResetBits(LED_PORT, LED_TEMP2);
			break;
		case 3:
			GPIO_SetBits(LED_PORT,LED_TEMP1/*|LED_TEMP2*/|LED_TEMP4|LED_TEMP5);
			GPIO_ResetBits(LED_PORT, LED_TEMP3);
			break;
		case 4:
			GPIO_SetBits(LED_PORT,LED_TEMP1/*|LED_TEMP2*/|LED_TEMP3|LED_TEMP5);
			GPIO_ResetBits(LED_PORT, LED_TEMP4);
			break;
		case 5:
			GPIO_SetBits(LED_PORT,LED_TEMP1/*|LED_TEMP2*/|LED_TEMP3|LED_TEMP4);
			GPIO_ResetBits(LED_PORT, LED_TEMP5);
			break;
		default:
			break;
	}
}


/**********************************************/
/* �������ܣ� �ص�led��                        */
/* ��ڲ�����led_num from 1-5	                          */
/**********************************************/
void LevelLED_Off(int num)
{

}

void TimerLED_Off(void)
{
	GPIO_SetBits(LED_PORT,LED_TIME1|LED_TIME2|LED_TIME3);	
}

extern volatile int mode_led_flag;
void Mode_led_Blinking(int on)
{
	if(on)
	{
		if(mode_led_flag)
			GPIO_ResetBits(LED_PORT,LED_TIME3);		
		else
	  		GPIO_SetBits(LED_PORT,LED_TIME3);
   }
	else
		GPIO_SetBits(LED_PORT,LED_TIME3);	
}
//led_num 0-1 
void TimerLED_On(int led_num)
{
	switch(led_num)
	{
		case 0:
			TimerLED_Off();	
			break;
		case 1:
			GPIO_SetBits(LED_PORT,LED_TIME2|LED_TIME3);
			GPIO_ResetBits(LED_PORT, LED_TIME1);
			break;
		case 2:
			GPIO_SetBits(LED_PORT,LED_TIME1|LED_TIME3);
			GPIO_ResetBits(LED_PORT, LED_TIME2);
			break;
		case 3:
			GPIO_SetBits(LED_PORT,LED_TIME1|LED_TIME2);
			GPIO_ResetBits(LED_PORT, LED_TIME3);
		default:
			break;
	}
}

void turnoff_all_LED(void)
{
	GPIO_SetBits(LED_PORT,LED_TEMP1/*|LED_TEMP2*/|LED_TEMP3|LED_TEMP4|LED_TEMP5|LED_TIME1|LED_TIME2|LED_TIME3|LED_ERROR);
}

void WARNLED_ON(void)
{
	GPIO_ResetBits(LED_PORT, LED_ERROR);

}

void WARNLED_OFF(void)
{
	GPIO_SetBits(LED_PORT, LED_ERROR);
}

void WARNLED_Toggle(void)
{
	if((wrn++)%2)
		WARNLED_ON();
	else
		WARNLED_OFF();
}


//
///**********************************************/
///* �������ܣ�led��ת                          */
///* ��ڲ�����led_num from 0-4                   */
///**********************************************/
//void LevelLED_Toggle(int led_num)
//{
//	GPIO_WriteBit(GPIOA, GPIO_Pin_11, (BitAction)((1-GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_11))));
//}
