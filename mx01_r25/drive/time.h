#ifndef _TIME_H_
#define _TIME_H_

#include "stm32f0xx.h"
#include "key.h"

typedef enum {
	LOAD_FACE_MASK = 0,
	LOAD_WARM_PALACE_PO,
	LOAD_EYE_MASK,
} LOAD_TYPE;
void PWM_set_to_devicetype_level(LOAD_TYPE type,int level );
void TIM_Config(void);
void TIM_PWM_Config(int val);
void TIM_INT_Config(void);
void TIM_OUT_Config(int num);
void TIM_Close(void);
void PWM_Close_NLPM(void);


void PWM_Init(void);
void PWM_Config(int val);
void PWM_Open(void);
void PWM_Close(void);

void WarnInit(void);
void WarnOff(void);
void WarnOn(int times, int ms);
void WarnOn_2(int timems);
void HeatTimerInit(void);
void HeatTimerOn(int num);
void HeatTimerOff(void);
void turnoff_all_LED(void);
void EmptyLoadDetectTimerInit(void);
void EmptyLoadDetectTimerOn(void);
void EmptyLoadDetectTimerOff(void);
#if defined(MCU_LOW_POWER_MODE)	
int lpm_get(void);
void lpm_set(int mode);
#endif
void TIM_OUT_Config2(void);
enum led_blink_times {
	BLINK_TIMES_1=1,
	BLINK_TIMES_2=2,
	BLINK_TIMES_3=3,
	BLINK_TIMES_4=4,
	BLINK_TIMES_5=5,
	BLINK_TIMES_6=6,
	BLINK_TIMES_ALWAYS=0xFF
};

enum led_on_off_time_ms {
	ON_OFF_100MS=100,
	ON_OFF_200MS=200,
	ON_OFF_300MS=300,
	ON_OFF_400MS=400,
	ON_OFF_500MS=500,
	ON_OFF_800MS=800,
	ON_OFF_1000MS=1000,
	ON_OFF_2000MS=2000
};
//void TIM1_Config();
#endif  /* _TIME_H_ */
