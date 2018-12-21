#include "time.h"
#include "dbg.h"
#include <math.h>
#include <stdio.h>

#define PWM_FREQ (1)
extern int warn_led_on_flag;

#if defined(MCU_LOW_POWER_MODE)
static volatile int flag_lpm=0;
#endif
void PWM_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	uint16_t TimerPeriod = 0;
	uint16_t Channel1Pulse = 0;
	uint16_t Prescaler=0;

	/* 使能GPIO时钟 */
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOB, ENABLE);

	/* 配置GPIO管脚复用*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_1);


	//TimerPeriod = (SystemCoreClock / 1000) - 1;
	/*计算CCR1 跳转值 在占空比为50%时*/
	//Channel1Pulse = (uint16_t) (((uint32_t) 50 * (TimerPeriod - 1)) / 100);
	Prescaler= (SystemCoreClock / 2000000 ) - 1;
	TimerPeriod = 100/PWM_FREQ - 1;
	//TimerPeriod = (SystemCoreClock / 1000 ) - 1;
	/* Compute CCR1 value to generate a duty cycle at 50% for channel 1 and 1N */
	Channel1Pulse = (uint16_t) (((uint32_t) 5 * (TimerPeriod - 1)) / 10);

	/* TIM3 时钟使能 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);

	/* Time 定时基础设置*/
	TIM_TimeBaseStructure.TIM_Prescaler = Prescaler;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  /* Time 定时设置为上升沿计算模式*/
	TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	/* 频道1，2，3，4的PWM 模式设置 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_Pulse = Channel1Pulse;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

	TIM_OC4Init(TIM3, &TIM_OCInitStructure);

	//TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	//TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	//TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	//TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	//TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	//TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	//TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

	//TIM_OCInitStructure.TIM_Pulse = Channel1Pulse;//使能频道1配置
	//TIM_OC1Init(TIM3, &TIM_OCInitStructure);


	//TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  //????TIM14??CCR1?????¤×°???????÷ 

	TIM_ARRPreloadConfig(TIM3,ENABLE);//ARPE????

	/* TIM3 计算器使能*/
	TIM_Cmd(TIM3, ENABLE);

	TIM_CtrlPWMOutputs(TIM3, ENABLE);

}

//we set pwm to 1k, but the value can adjust!
void PWM_Config(int val)
{
	//uint16_t TimerPeriod = (SystemCoreClock / 1000 ) - 1;
	uint16_t TimerPeriod = 100/PWM_FREQ - 1;
	/* Compute CCR1 value to generate a duty cycle at 50% for channel 1 and 1N */
	uint16_t Channel1Pulse = (uint16_t) (((uint32_t) val * (TimerPeriod - 1)) / 1000);

	TIM_SetCompare4(TIM3,Channel1Pulse);
}

void PWM_Open(void)
{
	TIM_Cmd(TIM3, ENABLE);
	GPIO_ResetBits(KEY_PORT, PWR_EN);
}


void PWM_Close_NLPM(void)
{
	//GPIO_SetBits(KEY_PORT, PWR_EN);
	GPIO_SetBits(KEY_PORT, PWR_EN);
	TIM_Cmd(TIM3, DISABLE);
	//TIM_Cmd(TIM2, DISABLE);
}

void PWM_Close(void)
{
	//GPIO_SetBits(KEY_PORT, PWR_EN);
	GPIO_SetBits(KEY_PORT, PWR_EN);
	TIM_Cmd(TIM3, DISABLE);
	//TIM_Cmd(TIM2, DISABLE);
#if defined(MCU_LOW_POWER_MODE)	
	/* Request to enter STOP mode with regulator in low power mode */
	if(warn_led_on_flag==1)
		warn_led_on_flag=2;
	else
		lpm_set(1);
#endif
	//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , DISABLE);
	//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, DISABLE);
}

#if defined(MCU_LOW_POWER_MODE)
void disable_all_timer(void)
{
	TIM_Cmd(TIM1, DISABLE);
	TIM_Cmd(TIM3, DISABLE);
	TIM_Cmd(TIM14, DISABLE);
	TIM_Cmd(TIM16, DISABLE);
	TIM_Cmd(TIM17, DISABLE);
}
int lpm_get(void)
{
	return flag_lpm;
}

void lpm_set(int mode)
{
	flag_lpm=mode;
	if(mode)
	{
		printf("enter into low power mode.\r\n");
		turnoff_all_LED();
		disable_all_timer();
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
		PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
	}
}
#endif

#define FACEMASK_LEVEL_1 16  //thia unit is V
#define FACEMASK_LEVEL_2 19
#define FACEMASK_LEVEL_3 21
#define FACEMASK_LEVEL_4 23
#define FACEMASK_LEVEL_5 25

#define WARM_PALACE_PO_LEVEL_1 19
#define WARM_PALACE_PO_LEVEL_2 21
#define WARM_PALACE_PO_LEVEL_3 23
#define WARM_PALACE_PO_LEVEL_4 25
#define WARM_PALACE_PO_LEVEL_5 27

#define EYEMASK_LEVEL_1 19
#define EYEMASK_LEVEL_2 21
#define EYEMASK_LEVEL_3 23
#define EYEMASK_LEVEL_4 25
#define EYEMASK_LEVEL_5 27

//VOUT_SET can config by change sc8201 PIN FB's RUP and RDOWN to config
//VOUT_SET=VREF*(1+RUP/RDOWN) ,here VREF=1.21V in IC SPEC
#define VOUT_SET (27.7)

#define FACEMASK_RESISTANCE 25
#define WARM_PALACE_PO_RESISTANCE 45
#define EYEMASK_RESISTANCE 65


//because VOUT_SET is 28.5,for 
//max power for FACEMASK is 32.49W
//max power for WARM_PALACE_PO is 18.05W
//max power for EYEMASK is 12.49W



//VOUT=VOUT_SET*(1/6+5*D/6),here,D mean PWM duty
//P=U*U/R -->U=sqrt(P*R),we have know P and R value,neet to calculate D value
//return value shall be in 0 - 100
int voltage_calculate_pwm_num(int voltage_value)
{
	int pwm_value=0,pwm_calc=0;
	
	//because the voltage_value shall less than VOUT_SET
	if(voltage_value>VOUT_SET)
		voltage_value=VOUT_SET;
	if(voltage_value<0) //invalie value case
		voltage_value = 0;
	if(voltage_value==0)
		return 0;
	pwm_calc=(((voltage_value/(float)VOUT_SET)-0.167)*6)/5;
	 //Accurate to one decimal place,if value is 500,that means duty is 50%
	pwm_value = (int)(pwm_calc*1000);

	return pwm_value;

	
}

int facemask_power_level_mapto_pwm_value(int level)
{
	int powervalue=0,ret=0;
	if(level>5)
		return 0;
	
	switch (level)
	{
		case 1:
			powervalue = FACEMASK_LEVEL_1;
			break;
		case 2:
			powervalue = FACEMASK_LEVEL_2;
			break;
		case 3:
			powervalue = FACEMASK_LEVEL_3;
			break;
		case 4:
			powervalue = FACEMASK_LEVEL_4;
			break;
		case 5:
			powervalue = FACEMASK_LEVEL_5;
			break;
		default:
			break;
	}
	
	ret = voltage_calculate_pwm_num(powervalue);
	if(ret>1000) ret=1000;
	return ret;
}
int warm_palace_po_power_level_mapto_pwm_value(int level)
{
	int powervalue=0,ret=0;
	if(level>5)
		return 0;
	
	switch (level)
	{
		case 1:
			powervalue = WARM_PALACE_PO_LEVEL_1;
			break;
		case 2:
			powervalue = WARM_PALACE_PO_LEVEL_2;
			break;
		case 3:
			powervalue = WARM_PALACE_PO_LEVEL_3;
			break;
		case 4:
			powervalue = WARM_PALACE_PO_LEVEL_4;
			break;
		case 5:
			powervalue = WARM_PALACE_PO_LEVEL_5;
			break;
		default:
			break;
	}
	
	ret = voltage_calculate_pwm_num(powervalue);
	if(ret>1000) ret=1000;
	return ret;
}

int eyemask_power_level_mapto_pwm_value(int level)
{
	int powervalue=0,ret=0;
	if(level>5)
		return 0;
	
	switch (level)
	{
		case 1:
			powervalue = EYEMASK_LEVEL_1;
			break;
		case 2:
			powervalue = EYEMASK_LEVEL_2;
			break;
		case 3:
			powervalue = EYEMASK_LEVEL_3;
			break;
		case 4:
			powervalue = EYEMASK_LEVEL_4;
			break;
		case 5:
			powervalue = EYEMASK_LEVEL_5;
			break;
		default:
			break;
	}
	
	ret = voltage_calculate_pwm_num(powervalue);
	if(ret>1000) ret=1000;
	return ret;
}


void PWM_set_to_devicetype_level(LOAD_TYPE type,int level )
{
	int pwm_value = 0;
	
	if(type==LOAD_FACE_MASK)
	{
		pwm_value=facemask_power_level_mapto_pwm_value(level);
	}
	else if(type==LOAD_WARM_PALACE_PO)
	{
		pwm_value=warm_palace_po_power_level_mapto_pwm_value(level);
	}
	else if(type==LOAD_EYE_MASK)
	{
		pwm_value=eyemask_power_level_mapto_pwm_value(level);
	}
	else
		pwm_value = facemask_power_level_mapto_pwm_value(level);	//set facemask power level as default
	
	PWM_Config(pwm_value);	
}


//we set tim2 as 10or20min timer! tim2 is 32bit timer

void TIM_INT_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* TIM2 clock enable */
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE);

	/*  TIM3 中断嵌套设计*/
	NVIC_InitStructure.NVIC_IRQChannel = TIM16_IRQn;//TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
volatile int timer16_decrese_count = 0;
volatile int cooldown_count = 0;
//num :1-10mins 2-20mins 3-allways on!
void TIM_OUT_Config(int num)
{
	//count 1 is 30 seconds unit,so we can custom the count though parameter num
	if(num == 1)timer16_decrese_count = 20; //10 minus
	else if(num == 2)timer16_decrese_count = 30;//15 minis
	else if(num == 3)timer16_decrese_count = 40;// 20 mins
	else timer16_decrese_count = num*20;
	
	TIM_OUT_Config2();
	cooldown_count = 0;
	
}

void TIM_OUT_Config2(void)
{
#if 1
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE);
	/* -----------------------------------------------------------------------
	TIM2 配置: 输出比较时序模式:

	本试验中 TIM3输入时钟(TIM3CLK) 被设为APB1 时钟 (PCLK1),  
	  => TIM16CLK = PCLK1 = SystemCoreClock = 48 MHz
	      
	当 TIM2 计数时钟 设为1KHz, 预分频器可以按照下面公式计算：
	   Prescaler = (TIM3CLK / TIM3 counter clock) - 1
	   Prescaler = (PCLK1 /1K Hz) - 1                                               
	----------------------------------------------------------------------- */   

	/* 计算预分频值 */
	uint16_t PrescalerValue = (uint16_t) (SystemCoreClock  / 1000) - 1;

	/* Time 定时器基础设置 */
	TIM_TimeBaseStructure.TIM_Period = 30000;//10 or 20 minutes
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM16, &TIM_TimeBaseStructure);

	/* 预分频器配置 */
	//TIM_PrescalerConfig(TIM2, PrescalerValue, TIM_PSCReloadMode_Immediate);

	//  /* 输出比较时序模式配置设置 */
	//  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
	//  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	//
	//  /* 输出比较时序模式配置: 频道3*/
	//  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	//  TIM_OCInitStructure.TIM_Pulse = CCR3_Val;
	//  TIM_OC3Init(TIM2, &TIM_OCInitStructure);
	//  TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Disable);

	/* Output Compare Timing Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 30000-1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC1Init(TIM16, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM16, TIM_OCPreload_Disable);

	/* TIM 中断使能 */
	TIM_ITConfig(TIM16, TIM_IT_CC1, ENABLE);

	/* TIM16 使能 */
	TIM_Cmd(TIM16, ENABLE);
  #endif

}

void TIM_Close(void)
{
	TIM_Cmd(TIM16, DISABLE);
	
}


//use timer1 to detect the system is empty load or not,if empty load timeout,turnoff the power module system

extern volatile int is_really_empty_load_count, load_avg_value;
void EmptyLoadDetectTimerInit(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* TIM1 clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
void EmptyLoadDetectTimerOn(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

    NVIC_InitTypeDef NVIC_InitStructure;

	/* TIM1 clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);


	/* 计算预分频值 */
	uint16_t PrescalerValue = (uint16_t) (SystemCoreClock  / 1000) - 1;

	/* Time 定时器基础设置 */
	TIM_TimeBaseStructure.TIM_Period = 10000;//10s
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	/* 预分频器配置 */
	TIM_PrescalerConfig(TIM1, PrescalerValue, TIM_PSCReloadMode_Immediate);

	/* Output Compare Timing Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 10000-1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Disable);

	/* TIM 中断使能 */
	TIM_ITConfig(TIM1, TIM_IT_CC1, ENABLE);

	/* TIM1 使能 */
	TIM_Cmd(TIM1, ENABLE);

	pr_info(DBG_DEBUG, "emptyload time on: load_avg_value=%d, is_really_empty_load_count=%d\r\n",
		load_avg_value, is_really_empty_load_count);
}

void EmptyLoadDetectTimerOff(void)
{
	TIM_Cmd(TIM1, DISABLE);
}


//we set tim17 as 0.5s timer

void WarnInit(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* TIM2 clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM17, ENABLE);

	/*  TIM3 中断嵌套设计*/
	NVIC_InitStructure.NVIC_IRQChannel = TIM17_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
extern volatile int flag_warncnt;
extern int wrn;
void WARNLED_ON(void);
void WARNLED_OFF(void);
extern volatile int warn_led_on_off_time;


//para-->times:Number of flashes,ms:on or off time,unit is ms
void WarnOn(int times, int ms)
{
	/* -----------------------------------------------------------------------
	TIM2 配置: 输出比较时序模式:

	本试验中 TIM3输入时钟(TIM3CLK) 被设为APB1 时钟 (PCLK1),  
	  => TIM16CLK = PCLK1 = SystemCoreClock = 48 MHz

	当 TIM2 计数时钟 设为1KHz, 预分频器可以按照下面公式计算：
	   Prescaler = (TIM3CLK / TIM3 counter clock) - 1
	   Prescaler = (PCLK1 /1K Hz) - 1                                               
	----------------------------------------------------------------------- */  
	if(times==0xFF)
		flag_warncnt=0xFFFF;  //always blink
	else
		flag_warncnt = times*2; //set the times value

	wrn = 1;
	if(ms!=0)
		warn_led_on_off_time = ms; //set the glabal timems value
	else
		warn_led_on_off_time = 1000; //if ms is invalid,set default value

	WarnOn_2(warn_led_on_off_time);
	WARNLED_ON(); //turn on warn LED first
	warn_led_on_flag = 1;
	pr_info(DBG_DEBUG, "WarnOn()\r\n");
}

void WarnOn_2(int timems)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM17, ENABLE);

	/* 计算预分频值 */
	uint16_t PrescalerValue = (uint16_t) (SystemCoreClock  / 1000) - 1;

	/* Time 定时器基础设置 */
	TIM_TimeBaseStructure.TIM_Period = timems;//1s
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM17, &TIM_TimeBaseStructure);

	/* 预分频器配置 */
	TIM_PrescalerConfig(TIM17, PrescalerValue, TIM_PSCReloadMode_Immediate);

	/* Output Compare Timing Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = timems-1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC1Init(TIM17, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM17, TIM_OCPreload_Disable);

	/* TIM 中断使能 */
	TIM_ITConfig(TIM17, TIM_IT_CC1, ENABLE);

	/* TIM2 使能 */
	TIM_Cmd(TIM17, ENABLE);
}

void WarnOff(void)
{
	TIM_Cmd(TIM17, DISABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM17, DISABLE);
	WARNLED_OFF();//turn off warn LED
	warn_led_on_flag = 0;
}

void HeatTimerInit(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* TIM2 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);

	/*  TIM3 中断嵌套设计*/
	NVIC_InitStructure.NVIC_IRQChannel = TIM14_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void HeatTimerOn(int num)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	/* -----------------------------------------------------------------------
	TIM2 配置: 输出比较时序模式:

	本试验中 TIM3输入时钟(TIM3CLK) 被设为APB1 时钟 (PCLK1),  
	  => TIM16CLK = PCLK1 = SystemCoreClock = 48 MHz
	      
	当 TIM2 计数时钟 设为1KHz, 预分频器可以按照下面公式计算：
	   Prescaler = (TIM3CLK / TIM3 counter clock) - 1
	   Prescaler = (PCLK1 /1K Hz) - 1                                               
	----------------------------------------------------------------------- */   

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
	/* 计算预分频值 */
	uint16_t PrescalerValue = (uint16_t) (SystemCoreClock  / 1000) - 1;

	/* Time 定时器基础设置 */
	TIM_TimeBaseStructure.TIM_Period = num*1000;//20s
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);

	/* 预分频器配置 */
	TIM_PrescalerConfig(TIM14, PrescalerValue, TIM_PSCReloadMode_Immediate);

	/* Output Compare Timing Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = num*1000-1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC1Init(TIM14, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM14, TIM_OCPreload_Disable);

	/* TIM 中断使能 */
	TIM_ITConfig(TIM14, TIM_IT_CC1, ENABLE);

	/* TIM2 使能 */
	TIM_Cmd(TIM14, ENABLE);
}

void HeatTimerOff(void)
{
	TIM_Cmd(TIM14, DISABLE);
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, DISABLE);
}
