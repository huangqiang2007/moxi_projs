/******************** (C) COPYRIGHT 2018 重庆墨西科技有限公司********************
 * 文件名  ：main
 * 实验平台：MOXI_PW1.5_20180702
 * 描述    ：1.5代线控系统
 * 作者    ：桂陈
 *单位     ：重庆墨西科技有限公司
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
//	Open_CMOS();  //open cmos
		KEY_Init();
		Exti_Init();
	
		/*timer1 is for timer*/
		TIM_INT_Config();
		TIM_OUT_Config();
		TIM1_Disable();
	
	  /* USART1 config 115200 8-N-1 */
		USART_Configuration();		
		delay_init();
		AT24CXX_Init();			//IIC初始化 
	
		PWM_Channel_Init(); //PWM初始化
		
		//24c02首地址初始化
		AT24CXX_Addr_Init();
		
		/*timer3 is for PWM*/
		TIM_Config();
		TIM_PWM_Config();
		
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4) == 0 )//判断开机时，开机键是否被按下。
		{
			//开机键被按下
		}
		else{ //开机键未按下
			printf("\r\n [INFO]  waiting for button is pressed .(MAIN.c-52 line)   \r\n");
			Enter_stopmode(); //enter stop mode;
		}
//		printf("\r\n [INFO]  woke up  .(MAIN.c-52 line)   \r\n");
//ADC1_DMA_Init();
//	TIM1_Enable();
//	Open_CMOS();  //open cmos
	  while(1)
		{	
//			counter++;
//			if (counter%100 ==0)
//			{
				//printf("\r\n [INFO]  alive  \r\n");
//				counter=0;
//			}
			DMA1_ADRRESS();
			KEY_ADDRESS();
			TIM1_ADRRESS();
//			printf("\r\n [INFO]  woke up 2 .(MAIN.c-52 line)   \r\n");
      delay_ms(50);
		}	
}
