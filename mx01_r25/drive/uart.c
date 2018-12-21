#include <stdio.h>
#include "stm32f0xx.h"
#include "uart.h"

void USART_Configuration(void)//串口初始化函数
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE );

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_1);
	//GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_1);        
	/*
	*  USART1_TX -> PA9 , USART1_RX ->        PA10
	*/                                
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9/*|GPIO_Pin_10*/;                 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);        

	USART_InitStructure.USART_BaudRate = 115200;//设置串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//设置数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//设置停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//设置效验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//设置流控制
	USART_InitStructure.USART_Mode = /*USART_Mode_Rx |*/ USART_Mode_Tx;//设置工作模式
	USART_Init(USART1, &USART_InitStructure); //配置入结构体

	USART_Cmd(USART1, ENABLE);//使能串口1
}	

int ffputc(uint8_t ch)
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
	USART_SendData(USART1, (uint8_t) ch);

	/* Loop until the end of transmission */
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}

	return ch;
}

void ffputs(uint8_t str[])
{
	int i = 0, len = strlen(str);

	while (i < len) {
		ffputc(str[i]);
		i++;
	}	
}


int fputc(int ch, FILE* stream)
{
    /* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
	USART_SendData(USART1, (uint8_t) ch);

	/* Loop until the end of transmission */
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}

	return ch;
}

