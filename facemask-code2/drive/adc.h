#ifndef __ADC_H
#define	__ADC_H

#include "stm32f0xx.h"

void ADC1_DMA_Init(void);

void Open_ADC(void);
void Close_ADC(void);


void ADC_Filter(void);
void Computer_Results(void);

void DMA1_ADRRESS(void);
#endif /* __ADC_H */
