#ifndef _TIME_H_
#define _TIME_H_

#include "stm32f0xx.h"

void TIM_INT_Config(void);
void TIM_OUT_Config(void);
void TIM1_Enable(void);
void TIM1_Disable(void);

void TIM_Config(void);
void TIM_PWM_Config(void);
void PWM_Channel4_Enable(uint16_t ChannelXPulse);
void PWM_Channel4_Disable(void);
void PWM_Channel_Init(void);
void TIM1_ADRRESS(void);
uint16_t PWM_Compute_Pulse(uint16_t duty);
void PWM_ADJUST(uint16_t VOUT_sample_value,uint16_t VOUT_counter_value,int Res_equal);

#endif  /* _TIME_H_ */
