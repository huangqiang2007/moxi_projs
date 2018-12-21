#ifndef __ADC_H
#define	__ADC_H

#include "stm32f0xx.h"
int adc_result(void);
void ADC_Config(void);
void DMA_Config(void);
void ADC1_DMA_Init(void);
void adc_it_open(void);
void adc_it_close(void);
extern int adc_temp;
extern int adc_lv;
extern int adc_load;
extern int adc_innertemp;
extern __IO uint16_t RegularConvData_Tab[4];

#endif /* __ADC_H */
