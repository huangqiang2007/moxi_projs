#ifndef __KEY_H
#define	__KEY_H

#include "stm32f0xx.h"
#define KEY0			GPIO_Pin_3	//PB3
#define KEY1			GPIO_Pin_4	//PB4
#define KEY2			GPIO_Pin_5	//PB5

#define KEY_TEMPUP    KEY2
#define KEY_TEMPDN	  KEY0
#define KEY_TIME      KEY1

#define SETA			GPIO_Pin_0
#define SETB			GPIO_Pin_1
#define SETC			GPIO_Pin_2
									
#define PWR_CTL			GPIO_Pin_1  //PB1
#define PWR_EN			GPIO_Pin_0	//PB0

#define KEY_PORT       GPIOB

#define USB_DET			GPIO_Pin_15
#define USBDET_PORT       GPIOA


#define CHG_STDBY		GPIO_Pin_6 //PB6
#define CHG_CHARGE		GPIO_Pin_7 //PB7


void KEY_Init(void);
void Set_ABC(int num);
uint8_t KEY_Down(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin);

#endif /* __KEY_H */

