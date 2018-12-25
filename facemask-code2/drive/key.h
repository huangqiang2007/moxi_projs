#ifndef __KEY_H
#define	__KEY_H

#include "stm32f0xx.h"

void KEY_Init(void);
uint8_t KEY_Down(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin);
uint8_t KEY_System_ON_OFF_Down(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin);
void KEY_ADDRESS(void);
void CLOSE_SYSTEM(void);
void START_SYSTEM(void);

#endif /* __KEY_H */

