#ifndef __LED_H
#define	__LED_H

#include "stm32f0xx.h"
#define LED1_PIN      GPIO_Pin_9
#define LED2_PIN       GPIO_Pin_10
#define LED1_PORT       GPIOA	
#define LED2_PORT       GPIOA
void LED_Init(void);
void Exti_Init(void);
void LED_Open(void);
void All_LED_Close(void);
void LED1_Toggle(void);
void LED2_Toggle(void);
void Temp_One_LED_Open(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin);
void Timer_One_LED_Open(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin);

void pwm_100(void);
void pwm_0(void);

void Open_CMOS(void);
void Close_CMOS(void);

void booter_Open(void);
void booter_Close(void);

void Enter_stopmode(void);
#endif /* __LED_H */

