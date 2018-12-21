#include "key.h"
#include "stm32f0xx.h"

void KEY_Init(void)
{	
	GPIO_InitTypeDef GPIO_InitStruct;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	GPIO_InitStruct.GPIO_Pin = PWR_EN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_1;		
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;		
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStruct);	
	GPIO_SetBits(GPIOB,PWR_EN);	//Pull up to disable
}
	
void Set_ABC(int num)
{	
	
}
