/******************** (C) COPYRIGHT 2011 青风电子 ********************
 * 文件名  ：led.c
 * 描述    ：         
 * 实验平台：青风stm32f051开发板
 * 描述    ：led驱动函数
 * 作者    ：青风
 * 店铺    ：qfv5.taobao.com
**********************************************************************************/

#include "led.h"
#include "delay.h"
#include "uart.h"
#include "global.h"
#include "string.h"

uint8_t Enter_StopMode_flag = 0;

/**********************************************/
/* 函数功能； led灯 初始化                     */
/* 入口参数：无                               */
/**********************************************/
void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	//PA4:TEMP_BLUE;PA5:TMEP_GREEN;PA6:TEMP_RED;PA7:TIME_BLUE;PA8:TIME_GREEN;PA11:TIME_RED;(PA15:OUT_CTRL)
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_11|GPIO_Pin_15 ;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed =GPIO_Speed_Level_3;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
	// close all leds. 
	GPIO_SetBits(GPIOA, GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_11 );
	GPIO_SetBits(GPIOA, GPIO_Pin_15 );//close CMOS
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 |GPIO_Pin_1;//PB0:PWR_EN;PB1:POWER_CTRL
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed =GPIO_Speed_Level_3;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_SetBits(GPIOB, GPIO_Pin_0 );// close booster
	GPIO_ResetBits(GPIOB, GPIO_Pin_1 );//Pwm置低 0% 

}

void Exti_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource4);

 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;   //??
 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  //??
 GPIO_Init(GPIOB, &GPIO_InitStructure);
	
 EXTI_InitStructure.EXTI_Line = EXTI_Line4;  //???3
 EXTI_InitStructure.EXTI_LineCmd = ENABLE;
 EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
 //EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;  //????EXTI_Trigger_Falling
 EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
 EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;  //??????????
 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
 NVIC_InitStructure.NVIC_IRQChannelPriority = 0x00;  //?????
 NVIC_Init(&NVIC_InitStructure);
 
}


void EXTI4_15_IRQHandler()  //中断服务程序入口
{
			EXTI_ClearITPendingBit(EXTI_Line4);
}
/**********************************************/
/* 函数功能；pwm置高 100%                      */
/* 入口参数：无                               */
/**********************************************/
void pwm_100(void)
{
	 GPIO_ResetBits(GPIOB, GPIO_Pin_0 );
	//GPIO_SetBits(GPIOB, GPIO_Pin_0 );
	 GPIO_SetBits(GPIOB, GPIO_Pin_1 );
	}

	/**********************************************/
/* 函数功能；pwm置低 0%                      */
/* 入口参数：无                               */
/**********************************************/
void pwm_0(void)
{
	 GPIO_ResetBits(GPIOB, GPIO_Pin_0 );
	//GPIO_SetBits(GPIOB, GPIO_Pin_0 );
	 GPIO_ResetBits(GPIOB, GPIO_Pin_1 );
	}

void booter_Open(void)
{
	 GPIO_ResetBits(GPIOB, GPIO_Pin_0 );
	}	
	
void booter_Close(void)
{
	 GPIO_SetBits(GPIOB, GPIO_Pin_0 );
	}		
/**********************************************/
/* 函数功能；打开led灯                        */
/* 入口参数：无                               */
/**********************************************/
void LED_Open(void)
{
	GPIO_ResetBits(LED1_PORT, LED1_PIN );
	GPIO_ResetBits(LED2_PORT, LED2_PIN );
	}

void Temp_One_LED_Open(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin)
{
	GPIO_SetBits(GPIOx, GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6 );
	GPIO_ResetBits(GPIOx, GPIO_Pin );
	}	

void Timer_One_LED_Open(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin)
{
	GPIO_SetBits(GPIOx, GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_11 );
	GPIO_ResetBits(GPIOx, GPIO_Pin );
	}	

/**********************************************/
/* 函数功能；关掉所有LED灯                      */
/* 入口参数：无                               */
/**********************************************/
void All_LED_Close(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_11 );
}

/**********************************************/
/* 函数功能；关掉CMOS管,无电流输出                      */
/* 入口参数：无                               */
/**********************************************/
void Close_CMOS(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_15 );//close CMOS
}

/**********************************************/
/* 函数功能；打开CMOS管，电流通过产品                    */
/* 入口参数：无                               */
/**********************************************/
void Open_CMOS(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_15 );//open CMOS
}

void Enter_stopmode(void)
{
	Enter_StopMode_flag = 1;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);  //enable clock
	PWR_EnterSTOPMode(PWR_Regulator_LowPower,PWR_STOPEntry_WFI);
	SystemInit();
}

/**********************************************/
/* 函数功能；led翻转                          */
/* 入口参数：无                               */
/**********************************************/
void LED1_Toggle(void)
{
//	GPIOA->ODR ^=GPIO_Pin_11;
  GPIO_WriteBit(GPIOA, GPIO_Pin_8, 
		               (BitAction)((1-GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_8))));
}

void LED2_Toggle(void)
{
GPIO_WriteBit(GPIOA, GPIO_Pin_9, 
		               (BitAction)((1-GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_9))));
}
