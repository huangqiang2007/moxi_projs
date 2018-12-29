/******************** (C) COPYRIGHT 2018 ����ī���Ƽ����޹�˾********************
 * �ļ���  ��main
 * ʵ��ƽ̨��MOXI_PW1.5_20180702
 * ����    ��1.5���߿�ϵͳ
 * ����    �����
 *��λ     ������ī���Ƽ����޹�˾
**********************************************************************/

#include "stm32f0xx.h"
#include "adc.h"
#include "systick.h"
#include "uart.h"
#include <stdio.h>
#include "24cxx.h"
#include "delay.h"
#include "myiic.h"
#include "time.h"
#include "led.h"
#include "key.h"
#include "global.h"
#include "string.h"

int main(void)
{
	SystemInit();
	LED_Init();
	Close_CMOS();
	KEY_Init();
	Exti_Init();

	/*timer1 is for timer*/
	TIM_INT_Config();
	TIM_OUT_Config();
	TIM1_Disable();

	/* USART1 config 115200 8-N-1 */
	USART_Configuration();		
	delay_init();

	/* IIC��ʼ�� */
	AT24CXX_Init();

	/* PWM ��ʼ�� */
	PWM_Channel_Init();

	/* 24c02�׵�ַ��ʼ�� */
	AT24CXX_Addr_Init();

	/* timer3 is for PWM */
	TIM_Config();
	TIM_PWM_Config();

	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4) == 0 ) { /* �жϿ���ʱ���������Ƿ񱻰��� */
		/* ������������ */
	} else { /* ������δ���� */
		printf("\r\n [INFO]  waiting for button is pressed .(MAIN.c-52 line)   \r\n");
		Enter_stopmode(); //enter stop mode;
	}

	while(1) {
		DMA1_ADRRESS();
		KEY_ADDRESS();
		TIM1_ADRRESS();
		delay_ms(50);
	}
}
