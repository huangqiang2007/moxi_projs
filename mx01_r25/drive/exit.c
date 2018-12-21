#include "exit.h"

//pb0 pb1 as key interrupt
void EXIT_KEY_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStruct; 
		EXTI_InitTypeDef EXTI_InitStruct;
	  NVIC_InitTypeDef NVIC_InitStruct;
	  /* config the extiline(PB0) clock and AFIO clock */
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	  //RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

 	
	  	/* EXTI line gpio config(PF7) */
		GPIO_InitStruct.GPIO_Pin = KEY_TEMPDN|KEY_TIME|KEY_TEMPUP; 
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_2;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP; // 上拉输入
		GPIO_Init(KEY_PORT, &GPIO_InitStruct);

		GPIO_InitStruct.GPIO_Pin = USB_DET;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_2;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL; // 上拉输入
		GPIO_Init(USBDET_PORT, &GPIO_InitStruct);
		/* EXTI line(PB0) mode config */
		//config KEY_TEMPDN KEY_TEMPUP KEY_TIME USB_DET exti source
	    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource3);
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource4);
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource5);
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource15);

		EXTI_InitStruct.EXTI_Line = EXTI_Line3;
		EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿中断
		EXTI_InitStruct.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStruct);

//		EXTI_InitStruct.EXTI_Line = EXTI_Line1;
//		EXTI_Init(&EXTI_InitStruct);

//		EXTI_InitStruct.EXTI_Line = EXTI_Line6;
//		EXTI_Init(&EXTI_InitStruct);
//
//		EXTI_InitStruct.EXTI_Line = EXTI_Line7;
//		EXTI_Init(&EXTI_InitStruct);

		EXTI_InitStruct.EXTI_Line = EXTI_Line4;
		EXTI_Init(&EXTI_InitStruct);

		EXTI_InitStruct.EXTI_Line = EXTI_Line5;
		EXTI_Init(&EXTI_InitStruct);


		EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
		EXTI_InitStruct.EXTI_Line = EXTI_Line15;
		EXTI_Init(&EXTI_InitStruct);

    /* Configyre P[A|B|C|D|E]0  NIVC  */
		//NVIC_InitStruct.NVIC_IRQChannel = EXTI0_1_IRQn;
		NVIC_InitStruct.NVIC_IRQChannel =EXTI2_3_IRQn;
		NVIC_InitStruct.NVIC_IRQChannelPriority = 0x00;
		NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStruct);

		NVIC_InitStruct.NVIC_IRQChannel =EXTI4_15_IRQn;
		NVIC_Init(&NVIC_InitStruct);
	}
