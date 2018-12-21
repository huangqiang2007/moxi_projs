/******************** (C) COPYRIGHT 2012 青风电子 ********************
 * 文件名  ：main
 * 描述    ：         
 * 实验平台：青风stm32f051开发板
 * 描述    ：LED闪烁
 * 作者    ：青风
 * 店铺    ：qfv5.taobao.com
**********************************************************************/

#include "stm32f0xx.h"
#include "led.h"
#include "key.h"
#include "time.h"
#include "systick.h"
#include "exit.h"
#include "adc.h"
#include "wdog.h"
#include "uart.h"
#include "dbg.h"
#include <math.h>
#include <stdlib.h>


#define MAX_LOAD (2900)
#define DEFLAUT_R (25)

//this value is 3.0V reference,here set to Vbat=3.4v ,12bit adc value is 0xba0,0xba0*3000/0xFFF=2180
#define ADC_LOW_BATVOL_VALUE (2180) 
//output DETECT_TYPE_VOLTAGE correctsponding PWM number,for VOUT_SET is 27
#define DETECT_TYPE_VOLTAGE (16000) //unit is mV

#define EMPTY_LOAD_DELAY_SHUTDOWN_TIME  12 // 10 seconds as one unit ,2 :means 20 secconds
#define MODE_LED_TIMEOUT 40000    //about 2 seconds 
/**********************************************/
/* 函数功能；简单的延迟函数                   */
/* 入口参数：无                               */
/**********************************************/
static volatile int temp_cnt=0;
static volatile int time_cnt=0;
static volatile int temp_flag = 0;
static volatile int time_flag=0;
static volatile int flag_error=0;
static volatile int flag_start=0;
static volatile int flag_end=0;
static volatile int flag_chging=0;
static volatile int flag_chged=0;
static volatile int flag_vbusin=0;
static volatile int flag_lowpower=0; //for vbat lower than 3.4V,set to 1,turn off power module
volatile int flag_warncnt=0;  //flag to save Number of flashes,
static volatile int flag_adc=0;
//static volatile int flag_longpt=0;//for time level 3 always on flag,set to 1 when detect timekey longpress,use in interrupt
static volatile int flag_longpv=0;//when detect long press tempdown key,open powermodule,and set to 1;
//static volatile int flag_needtim2config=0; //for time level 3 always on,set 1 in interrupt,use in main loop
static volatile int flag_adcindex=0;
static volatile int index=0;

static volatile int flag_detdevice=0;
static volatile int flag_device=0;
static volatile int flag_deviceR=0;

static volatile int flag_devicedetstart=0;

static volatile float expect_I=0;
static volatile int ABC=0;
static volatile int expect_I_min=0;
static volatile int expect_I_max=0;
static volatile int flag_maxload=0;
extern volatile int timer16_decrese_count;
extern volatile int cooldown_count;

//for empty load use
volatile int is_really_empty_load_count = 0;
static volatile int empty_load_trig_shutdown_count =0;
static volatile int load_avg_index=0;	
unsigned int load_avg_value=0;			//use for judge the empty load or not
static int avg_arr_init_flag=0;			//for init use
static unsigned int load_sum=0;			//for average use
#define LOAD_AVERAGE_SIZE 20			//average size
static int load_bufferdata[LOAD_AVERAGE_SIZE]={0};//buffer array

//for winter or summer mode
//static volatile int flag_winter_mode=0;
//static volatile int power_mode_count=0;

//vbat use NTC high tempreture protect
static volatile int bat_high_temp_protect_count =0;
//,Rup=10k,vref voltage is 3v
//50c  ntc=4.13k,Vadc=3*(4.13/(10+4.13))=0.877v
//55c  ntc=3.51k,Vadc=3*(3.51/(10+3.51))=0.779v
//60c  ntc=3.00k,Vadc=3*(3.00/(10+3.00))=0.692 v
//65c  ntc=2.57k,Vadc=3*(2.57/(10+2.57))=0.613v
//70c  ntc=2.21k,Vadc=3*(2.21/(10+2.21))=0.543v
//75c  ntc=1.91k,Vadc=3*(1.91/(10+1.91))=0.482v
//80c  ntc=1.66k,Vadc=3*(1.61/(10+1.61))=0.427v
#define VBAT_HIGH_TEMP_ADC_VALUE 692  
#define VBAT_HIGH_TEMP_MAX_COUNT 10

volatile int warn_led_on_off_time=1000; //init 1000ms flag
int adc1_comp_shutdown_sys_flag = 0;//set to 1 when warn led blink complete
int warn_led_on_flag=0; //it set to 1,means warn led is running ,if warn led finish,set it to 0

static volatile int need_shutdown_flag = 0;

/*
	timer assign description
	timer 1 :empty load detect and shutdown power module
	timer 3 :PWM control
	timer 14:prehead and load type detect
	timer 16:timer level use
	timer 17:warn led use
*/
#if defined(MCU_LOW_POWER_MODE)
static volatile int flag_needlpm=1;//xienh  1:bootup and enter low power mode 0:bootup but not enter low power mode
#endif

uint16_t adcresult[4][4];

void EXTI2_3_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line3) != RESET) {//turn heat down level set button
		temp_flag=1;
		//flag_lowpower=0;
		EXTI_ClearITPendingBit(EXTI_Line3);
		pr_info(DBG_DEBUG, "EXTI_Line3 temp_flag=%d\r\n", temp_flag);
	}
}

//note! when in charging mode, we do not output pwm,just keep everything as zero status!
void EXTI4_15_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line5) != RESET) {//turn heat up level  set button
		temp_flag=1;
		//flag_lowpower=0; 
		EXTI_ClearITPendingBit(EXTI_Line5);
		pr_info(DBG_DEBUG, "EXTI_Line5 temp_flag=%d\r\n", temp_flag);
	}

	if(EXTI_GetITStatus(EXTI_Line4) != RESET)//time set button
	{
		if( temp_cnt )
			time_flag = 1;
		//flag_lowpower=0;
		EXTI_ClearITPendingBit(EXTI_Line4);
		pr_info(DBG_DEBUG, "EXTI_Line4 temp_cnt=%d, temp_flag=%d\r\n", temp_cnt, temp_flag);
	}

	if(EXTI_GetITStatus(EXTI_Line15) != RESET) {//usb detect interrupt
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15) == 1 )
			flag_vbusin=1;
		else 
			flag_vbusin=0;
		//flag_lowpower=0;
		EXTI_ClearITPendingBit(EXTI_Line15);
		pr_info(DBG_DEBUG, "EXTI_Line15 flag_vbusin=%d\r\n", flag_vbusin);
	}
}

//define 
#define TYPE1_PWM_NUMBER_LEVEL_1 520//512  	//-->16V  385-->13V
#define TYPE1_PWM_NUMBER_LEVEL_2 655//645	//-->19V
#define TYPE1_PWM_NUMBER_LEVEL_3 745//734	//-->21V
#define TYPE1_PWM_NUMBER_LEVEL_4 835//823	//-->23V
#define TYPE1_PWM_NUMBER_LEVEL_5 924//912	//-->25V

void pwm_adj(void)
{
	
	//when no load device plug in, flag_deviceR shall be DEFLAUT_R,use this power level
	if((flag_deviceR==DEFLAUT_R)||(flag_deviceR>=20&&flag_deviceR<=30)||(flag_deviceR>=60&&flag_deviceR<=70))
	{
		switch( temp_cnt )
		{
		case 1:
			PWM_Config(TYPE1_PWM_NUMBER_LEVEL_1);//512 379-->13v
			break;
		case 2:
			PWM_Config(TYPE1_PWM_NUMBER_LEVEL_2);
			break;
		case 3:
			PWM_Config(TYPE1_PWM_NUMBER_LEVEL_3);
			break;
		case 4:
			PWM_Config(TYPE1_PWM_NUMBER_LEVEL_4);
			break;
		case 5:
			PWM_Config(TYPE1_PWM_NUMBER_LEVEL_5);
			break; 
		default:
			PWM_Config(0); //no valid temp_cnt,turn off PWM
			break;
	  	}
		pr_info(DBG_DEBUG, "pwm_adj no load in\r\n");
	}
	else
	{
		switch( temp_cnt )
		{
		case 1:
			PWM_Config(512);//512
			break;
		case 2:
			PWM_Config(645);
			break;
		case 3:
			PWM_Config(734);
			break;
		case 4:
			PWM_Config(823);
			break;
		case 5:
			PWM_Config(912);
			break; 
		default:
			PWM_Config(0); //no valid temp_cnt,turn off PWM
			break;
	  	}
		pr_info(DBG_DEBUG, "pwm_adj valid load in\r\n");
	}
}

//for preheat short timer!
void TIM14_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM14, TIM_IT_CC1) != RESET)
	{
		//full time heat ends!
		flag_end=1;
		pwm_adj();		
		//close tim14		
		TIM_ClearITPendingBit(TIM14, TIM_IT_CC1);
		HeatTimerOff();
		pr_info(DBG_DEBUG, "TIM14_IRQHandler preheat\r\n");
	}
}
void reset_empty_load_para(void)
{

	int i;
	for(i=0;i<LOAD_AVERAGE_SIZE;i++)
		load_bufferdata[i]=0;
	load_sum=0;

	empty_load_trig_shutdown_count=0;
	is_really_empty_load_count=0;
}
void TIM1_CC_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM1, TIM_IT_CC1) != RESET)
	{
		TIM_ClearITPendingBit(TIM1, TIM_IT_CC1);
		//WARNLED_ON();
		if(is_really_empty_load_count>20)
		{
			empty_load_trig_shutdown_count++;
			if(empty_load_trig_shutdown_count>=EMPTY_LOAD_DELAY_SHUTDOWN_TIME)
			{
				//do shutdown system and enter low power mode
				//WARNLED_ON();
				turnoff_all_LED();
				//flag_error=1;
				temp_cnt=0;
				time_cnt=0;
				flag_start=0;
				flag_end=0;
				flag_longpv=0;
				//flag_longpt=0;
				//flag_winter_mode=0;
				flag_devicedetstart=0;
				reset_empty_load_para();
				adc_it_close();
				EmptyLoadDetectTimerOff();
				//HeatTimerOff();
				//PWM_Close();
				pr_warn("TIM1_CC empty load detected, load_avg_value=%d, is_really_empty_load_count=%d\r\n",
					load_avg_value, is_really_empty_load_count);
			}
			else
				EmptyLoadDetectTimerOn();
		}
		else
		{
			empty_load_trig_shutdown_count=0;
			EmptyLoadDetectTimerOn();
		}
		
	}
}

void TIM16_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM16, TIM_IT_CC1) != RESET) {
		TIM_ClearITPendingBit(TIM16, TIM_IT_CC1);//clean interrupt first
		if(timer16_decrese_count>0) {
			timer16_decrese_count--;
			cooldown_count++;
		}

		if(cooldown_count==10||cooldown_count==21||cooldown_count==32) {//when heat 5minus ,close the pwm output for 1 count ,that is 30 seconds
			PWM_Close_NLPM();
			pr_info(DBG_DEBUG, "cooldown_count=%d, 5mins heating, stop 30secs\r\n", cooldown_count);
		}

		if(cooldown_count==11||cooldown_count==22||cooldown_count==33) {//when cooldown for 1 count ,that is 30 seconds,restart the pwm output
			ADC_Config();
#ifdef ADC_USE_DMA
			DMA_Config();
#endif
			PWM_Init();
			PWM_Open();
			pwm_adj();
		}

		if(timer16_decrese_count>0) {
			TIM_OUT_Config2();//restart timer
			pr_info(DBG_DEBUG, "restart heating timer16_decrese_count=%d\r\n", timer16_decrese_count);
		} else {
			temp_cnt = 0;
			time_cnt=0;
			flag_start=0;
			flag_end=0;
			flag_devicedetstart=0;
			expect_I=0;
			flag_longpv=0;
			pr_info(DBG_DEBUG, "stopping heating\r\n");
		}
	}
}

static volatile int tim17_noneed_act_flag = 0;
//for warning toggle timer!
void TIM17_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM17, TIM_IT_CC1) != RESET)
	{
		TIM_ClearITPendingBit(TIM17, TIM_IT_CC1);
		adc_it_open();
		if(flag_warncnt!=0xFFFF)
		{
			if(flag_warncnt!=0)
				flag_warncnt--;
			
			if(flag_warncnt>=1)
			{
				WarnOn_2(warn_led_on_off_time);
				WARNLED_Toggle();
				tim17_noneed_act_flag = 0;
			}
			else //include flag_warncnt==0 case
			{
				if(tim17_noneed_act_flag==0)
				{
				tim17_noneed_act_flag = 1;
				WarnOff();
				if(adc1_comp_shutdown_sys_flag==2)//when warn led timer is active by adc1_comp
					{
					//adc1_comp_shutdown_sys_flag = 1;
				// if(adc1_comp_shutdown_sys_flag == 1)
					flag_error=1;
					temp_cnt=0;
					time_cnt=0;	
					//PWM_Close();
					// we stop heat timer!
					flag_start=0;
					flag_end=0;
					flag_devicedetstart=0;
					HeatTimerOff();			
					adc_it_close();
					adc1_comp_shutdown_sys_flag = 3; //set flag
					}
					
				#if defined(MCU_LOW_POWER_MODE)	
				//delay set the lpm flag when there is warn timer running
				//if(warn_led_on_flag==2)
				{
					//lpm_set(1);
					flag_needlpm=1;
				}
				#endif
				
				}
			}
		}
		else //case flag_warncnt==0xFFFF
		{
			WarnOn_2(warn_led_on_off_time);
			WARNLED_Toggle();
		}

	//if vbat rise up to 3.6V,then turn off the blink warn LED, cancel shutdown power module action	
	//	if( adc_lv > 2314)	//2314 for 3.6v
	//	{
	//		WarnOff();
	//		adc1_comp_shutdown_sys_flag = 0;
	//		flag_lowpower = 0;
	//	}	

	}
}

void WWDG_IRQHandler(void)
{
  WWDG_SetCounter(0x7F);       //更新计数值
  WWDG_ClearFlag();            //清除标志位
}

void ADC1_COMP_IRQHandler(void)
{
  if(ADC_GetITStatus(ADC1, ADC_IT_AWD) != RESET)
  {
    /* Clear ADC1 AWD pending interrupt bit */    
	// we need shutdown pwm and close all the func!

	  flag_lowpower++;
	//only show warn led  in open mode
	if(flag_longpv&&flag_warncnt==0){
		if( flag_lowpower == 30)
	  	{
		adc_it_close();
		WarnOn(BLINK_TIMES_4,ON_OFF_800MS); //led闪烁
		adc1_comp_shutdown_sys_flag = 2; //set adc1_comp flag to 2
		LevelLED_On(0);
		TimerLED_On(0);
		PWM_Close_NLPM();
		flag_lowpower = 0;
	  	}	
	}
	
	ADC_ClearITPendingBit(ADC1, ADC_IT_AWD);

  }
}


void DMA1_Channel1_IRQHandler(void)
{
//    if(DMA_GetFlagStatus(DMA1_FLAG_HT1))
//    {
//        DMA_ClearITPendingBit(DMA_IT_HT);
//        memcpy(WriteBuff, ADC_ConvertedValue, ADC_BUFF_LEN*sizeof(uint16_t)/2);
//    }
//
//    if(DMA_GetFlagStatus(DMA1_FLAG_TC1))
//    {
//        DMA_ClearITPendingBit(DMA1_FLAG_TC1);
//        memcpy(WriteBuff+ADC_BUFF_LEN/2, ADC_ConvertedValue+ADC_BUFF_LEN/2, ADC_BUFF_LEN*sizeof(uint16_t)/2);
////此处可以增加标志位
//    }
	if(DMA_GetITStatus(DMA1_IT_TC1)) {
		//ADC_Ok=TRUE;		 
		flag_adc=1;
		index=flag_adcindex;
		memcpy(adcresult[flag_adcindex++],RegularConvData_Tab,sizeof(RegularConvData_Tab));		 
		flag_adcindex%=4;
		DMA_ClearITPendingBit(DMA1_IT_GL1); //清除全部中断标志			
	}
}
#if defined(MCU_LOW_POWER_MODE)	

/**
  * @brief  Configures system clock after wake-up from STOP: enable HSE, PLL
  *         and select PLL as system clock source.
  * @param  None
  * @retval None
  */
static void SYSCLKConfig_STOP(void)
{
	pr_info(DBG_DEBUG, "SYSCLKConfig_STOP()\r\n");
	/* After wake-up from STOP reconfigure the system clock */
	/* Enable HSE */
	RCC_HSEConfig(RCC_HSE_ON);

	/* Wait till HSE is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET)
	{}

	/* Enable PLL */
	RCC_PLLCmd(ENABLE);

	/* Wait till PLL is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
	{}

	/* Select PLL as system clock source */
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

	/* Wait till PLL is used as system clock source */
	while (RCC_GetSYSCLKSource() != 0x08)
	{}
}
#endif

static volatile	int save_usbin=0;
static volatile int usbin=0;
static int deviceR[20];
static volatile int save_tempcnt=0, save_timecnt=0, lpm_count=0;;
static void tempkey_handle(void)
{
	if( temp_flag ) {
		// do some fliter
		/*延时消抖*/
		//Delay10MS(10);
		Delay25MS(2);
		if(GPIO_ReadInputDataBit(KEY_PORT,KEY_TEMPDN) == 0 ) {//temp-
			int delaycnt=0;
			Delay25MS(2);
			while(GPIO_ReadInputDataBit(KEY_PORT,KEY_TEMPDN) == 0) {
				Delay25MS(2);
				delaycnt++;
				if(delaycnt > 15)break;//long press
			}

			if(delaycnt > 14) {
				/* long press */
				flag_longpv++;
				flag_longpv%=2;
				if(flag_longpv){
					//open
					//read adc_lv first
#ifdef ADC_DMA_IRQ
					if(flag_adc)
						adc_lv = adcresult[index][2]* 3000 / 0xFFF;
#endif
					if((adc_lv <= ADC_LOW_BATVOL_VALUE)&&(adc_lv!=0)) {//battery voltage lower than 3.4v ,show warn led blink
						temp_cnt=0;
						time_cnt=0;
						WarnOn(BLINK_TIMES_4,ON_OFF_800MS); //led闪烁
						adc1_comp_shutdown_sys_flag = 2; //set adc1_comp flag to 2
						pr_warn("adc_lv=%d is too low\r\n", adc_lv);
					} else {
						temp_cnt=1;
						time_cnt=1;
					}
					save_usbin = 0;	//set flag to stop open pwm module again
					flag_start=0;
					flag_end=0;
					flag_devicedetstart=0;
					empty_load_trig_shutdown_count=0;	//every time reset the time counter
					EmptyLoadDetectTimerOn();			//start empty load timer detect procedure
					flag_lowpower = 0;

					pr_info(DBG_DEBUG, "tempdn lp open flag_adc=%d, adc_lv=%d, temp_cnt=%d, time_cnt=%d\r\n",
						flag_adc, adc_lv, temp_cnt, time_cnt);
				} else {//close
					temp_cnt=0;
					time_cnt=0;
					//PWM_Close();
					need_shutdown_flag=1;
					// we stop heat timer!
					flag_start=0;
					flag_end=0;
					flag_devicedetstart=0;
					HeatTimerOff();
					adc_it_close();
					expect_I=0;
					//flag_longpt=0;
					//flag_winter_mode=0;
					ABC=0;
					pr_info(DBG_DEBUG, "tempdn lp close\r\n");
				}
			}else {
				/* short press */
				if (flag_longpv) {
					if(temp_cnt > 0)
						temp_cnt--;

					if(adc_lv > ADC_LOW_BATVOL_VALUE) {
						if(!time_cnt) time_cnt++;
						if(!temp_cnt) temp_cnt=1;
					}
				}
				pr_info(DBG_DEBUG, "tempdn sp flag_longpv=%d, temp_cnt=%d, time_cnt=%d, adc_lv=%d\r\n",
				flag_longpv, temp_cnt, time_cnt, adc_lv);
			}
		} else if(GPIO_ReadInputDataBit(KEY_PORT,KEY_TEMPUP) == 0 ) {//temp+
			//Delay10MS(10);
			Delay25MS(2);
			if(GPIO_ReadInputDataBit(KEY_PORT,KEY_TEMPUP) == 0 ) {
				if(flag_longpv) {
					if(adc_lv > ADC_LOW_BATVOL_VALUE) {
						if(temp_cnt < 6)
							temp_cnt++;

						if(!time_cnt) time_cnt++;
						if(temp_cnt>5) temp_cnt=5;
					}
				}
				pr_info(DBG_DEBUG, "tempup flag_longpv=%d, adc_lv=%d, temp_cnt=%d, time_cnt=%d\r\n",
					flag_longpv, adc_lv, temp_cnt, time_cnt);
			}
		}

		temp_flag=0;
	}
}

static void timekey_handle(void)
{
	if( time_flag ) {
		//int delaycnt=0;
		// do some fliter
		//Delay10MS(10);
		Delay25MS(2);
		if(GPIO_ReadInputDataBit(KEY_PORT,KEY_TIME) == 0) {
			Delay25MS(2);
			if(GPIO_ReadInputDataBit(KEY_PORT,KEY_TIME) == 0)
				if(flag_longpv) {
					time_cnt++;
					time_cnt%=3;
					if(!time_cnt)
					  time_cnt=3;
				}
		}

		pr_info(DBG_DEBUG, "timekey flag_longpv=%d, time_cnt=%d\r\n", flag_longpv, time_cnt);

		time_flag=0;
	}
}

static void usbin_judge(void)
{
	//save_usbin=flag_vbusin;
	//if(flag_vbusin || flag_e1)
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15) == 1 ) {
		usbin=1;		//charger connect
	} else {
		usbin=0;
	}

	if((save_usbin!=usbin)&&(flag_longpv==1)) {
		/* Clear ADC1 AWD pending interrupt bit */
		// we need shutdown pwm and close all the func!
		if(!usbin) {
			WarnOff();
			adc_it_open();
		} else {
			flag_vbusin=1;
			flag_error=1;
			temp_cnt=0;
			time_cnt=0;
			WarnOn(BLINK_TIMES_1,ON_OFF_800MS);
			//PWM_Close();
			need_shutdown_flag=1;
			// we stop heat timer!
			flag_start=0;
			flag_end=0;
			flag_devicedetstart=0;
			HeatTimerOff();
			adc_it_close();
			expect_I=0;
			flag_longpv=0;
			//flag_longpt=0;
			//flag_winter_mode=0;
		}
		save_usbin=usbin;

		pr_warn("usbin_judge usbin=%d, save_usbin=%d\r\n", usbin, save_usbin);
	}
}


static void adc_to_deviceR(void)
{
	if(flag_adc) {
		//doing some adc working here? 9864--LOAD_DETECT,LV_DETECT,TEMP_BATT,TEMP_DETECT
		//adc_innertemp --> mainboard			adc_load -->loaddetect voltage,
		//adc_lv  -->battery voltage detect 		adc_temp -->battery NTC voltage
		adc_innertemp		= adcresult[index][0]* 3000 / 0xFFF;
		adc_load			= adcresult[index][1]* 3000 / 0xFFF;
		adc_lv				= adcresult[index][2]* 3000 / 0xFFF;
		adc_temp			= adcresult[index][3]* 3000 / 0xFFF;

		flag_adc=0;

		//pr_info(DBG_DEBUG, "adc_inner=%d, adc_load=%d, adc_lv=%d, adc_temp=%d\r\n",
		//	adc_innertemp, adc_load, adc_lv, adc_temp);

		if(( flag_end==0)&&(flag_start==1)) {
			if(adc_load>0) {
				//if R40=0.02,then adc_load =I*0.02*100*1000=I*2000,--> I=adc_load/2000
				//detect voltage is DETECT_TYPE_VOLTAGE
				//R=U/I= DETECT_TYPE_VOLTAGE/(adc_load/2000)=DETECT_TYPE_VOLTAGE*2/adc_load
				int validR= DETECT_TYPE_VOLTAGE*2/adc_load;

				if(validR<1000) {
					deviceR[flag_detdevice++]=validR;
					flag_detdevice%=20;
				}
			}
		}
	}
}

static void first_pwm_adj(void)
{
	if((flag_end==1)&&(flag_devicedetstart==0)) {
		int i=0;

		for(i=0;i<20;i++)
			flag_deviceR+=deviceR[i];

		flag_deviceR/=20;  //calculate the device  resistance value,use for judge device type
		flag_devicedetstart=1;

		//there must de default value for deviceR
		if(flag_deviceR > 500) flag_deviceR=DEFLAUT_R;
		if(flag_deviceR < 8) flag_deviceR=DEFLAUT_R;

		pwm_adj();
	}
}

static void empty_load_detect(void)
{
	//use load_avg_value to judge the empty load or not
	if(flag_end==1) {
		if(avg_arr_init_flag==0) {
			int i;

			for(i=0;i<LOAD_AVERAGE_SIZE;i++)
				load_bufferdata[i]=adc_load;

			avg_arr_init_flag = 1;
			load_sum=adc_load*LOAD_AVERAGE_SIZE;
		}

		//load_sum -= load_bufferdata[load_avg_index];
		//load_sum+=  adc_load;
		load_bufferdata[load_avg_index]=adc_load;
		load_sum=0;
		{
			int i;
			for(i=0;i<LOAD_AVERAGE_SIZE;i++)
				load_sum+=load_bufferdata[i];
		}
		load_avg_index++;
		load_avg_index=load_avg_index%20;
		load_avg_value = load_sum/LOAD_AVERAGE_SIZE;


		//begin to judge
		if(is_really_empty_load_count>2000)
			is_really_empty_load_count=20; //avoid over flow

		if(load_avg_value<200)
			is_really_empty_load_count++;
		else
			is_really_empty_load_count=0;
	}
}


static void vbat_high_temp_protect(void)
{
	//vbat high temp protect
	if(adc_temp < VBAT_HIGH_TEMP_ADC_VALUE)
		bat_high_temp_protect_count++;
	else
		bat_high_temp_protect_count=0;
	
	if((bat_high_temp_protect_count > VBAT_HIGH_TEMP_MAX_COUNT)&&(flag_longpv==1)) {
		bat_high_temp_protect_count = 0; //reset tha counter
		temp_cnt=0;
		time_cnt=0;	
		WarnOn(BLINK_TIMES_2,ON_OFF_800MS); 
		//PWM_Close();
		need_shutdown_flag=1;
		flag_start=0;
		flag_end=0;
		flag_devicedetstart=0;
		HeatTimerOff();			
		adc_it_close();
		flag_longpv=0;
		//flag_longpt=0;
		//flag_winter_mode=0;
	}
}

static void temp_and_time_key_parse(void)
{
	if((save_tempcnt != temp_cnt) || (save_timecnt != time_cnt)
		||(adc1_comp_shutdown_sys_flag == 3)||(need_shutdown_flag==1)) {
		if( save_tempcnt != temp_cnt) {
			save_tempcnt=temp_cnt;
			LevelLED_On(temp_cnt);
		}

		if(save_timecnt != time_cnt) {
			save_timecnt=time_cnt;
			TimerLED_On(time_cnt);

			//set timer  only when time_cnt change ,do the timer will be reset to time_cnt *10 minus
			if(time_cnt!=0)
				TIM_OUT_Config(time_cnt);
		}

		if(adc1_comp_shutdown_sys_flag == 3)
			adc1_comp_shutdown_sys_flag = 0;//reset flag

		need_shutdown_flag=0;//reset flag

		if( temp_cnt==0) {//close pwm output,clear timer
			PWM_Config(0);
			PWM_Close();//really shutdown pwm here
			// we stop heat timer!
			flag_start=0;
			flag_end=0;
			flag_devicedetstart=0;
			flag_longpv=0;
			//flag_longpt=0;
			//flag_winter_mode=0;
			HeatTimerOff();
		} else {
			//Every time, we start pwm output, we need start adc monitor again!
			ADC_Config();
#ifdef ADC_USE_DMA
      		DMA_Config();
#endif				
			PWM_Init();
			PWM_Open();
			//every new start will heat 10s full!
			if( !flag_start) {
				flag_start=1;
				//open pwm output
				HeatTimerOn(5);//we start 10s full heat!
				PWM_Config(TYPE1_PWM_NUMBER_LEVEL_1);//output TYPE1_PWM_NUMBER_LEVEL_1 voltage ,for VOUT_SET is 27
				pr_info(DBG_DEBUG, "PWM enable, temp_cnt=%d, time_cnt=%d\r\n", temp_cnt, time_cnt);
			} else {
				if(flag_end) {
					pwm_adj();
					pr_info(DBG_DEBUG, "PWM_adj, temp_cnt=%d, time_cnt=%d\r\n", temp_cnt, time_cnt);
				} else  {
					PWM_Config(TYPE1_PWM_NUMBER_LEVEL_1);
					//PWM_Config(800);
				}
			}			
		}
	}
}

static void adcload_high_detect(void)
{
	if(adc_load > MAX_LOAD) {
		//try to lowerdown power!
		flag_maxload++;
		//close output!and warning on!
		if(flag_maxload>3) {
			flag_error=1;
			temp_cnt=0;
			time_cnt=0;	
			WarnOn(BLINK_TIMES_3,ON_OFF_800MS); 
			//PWM_Close();
			need_shutdown_flag=1;
			// we stop heat timer!
			flag_start=0;
			flag_end=0;
			flag_devicedetstart=0;
			HeatTimerOff();			
			adc_it_close();
			flag_longpv=0;
			//flag_longpt=0;
			//flag_needlpm=1;
			//flag_winter_mode=0;
		}
	} else {
		flag_maxload=0;
	}
}

/*
* if it hasn't power on operation more than 10secs,
* enter into low power mode directly.
*/
static void low_power_handle(void)
{
#if defined(MCU_LOW_POWER_MODE)
	if(flag_needlpm) {
		if(flag_longpv==0)
			lpm_count++;
		else {
			lpm_count=0;
			flag_needlpm=0;
			lpm_set(0);
		}

		if(lpm_count > 2000000) {//about 10 seconds
			lpm_count=0;
			flag_needlpm=0;
			if(warn_led_on_flag==1)
				warn_led_on_flag=2;
			else
				lpm_set(1);
		}
	}

	/* Configures system clock after wake-up from STOP */
	if(lpm_get()) {
		SYSCLKConfig_STOP();
		lpm_set(0);
		if(flag_longpv==0)flag_needlpm=1;
	}
#endif //#if defined(MCU_LOW_POWER_MODE)

}

/**********************************************/
/* 函数功能；主函数                           */
/* 入口参数：无                               */
/**********************************************/
//volatile int mode_led_count = 1;
volatile int mode_led_flag=0;
int main(void)
{
	Systick_Init();
	KEY_Init();
	LED_Init();
	TIM_INT_Config();
	EXIT_KEY_Init();
	WarnInit();
	ADC_Config();
#ifdef ADC_USE_DMA
	DMA_Config();
#endif
	HeatTimerInit();
	EmptyLoadDetectTimerInit();
	USART_Configuration();
	//ffputs("\r\nuart init done...\r\n");
	printf("\r\nuart init done...\r\n");

	while(1) {
#if 1
		tempkey_handle();
		timekey_handle();
		usbin_judge();

		if((flag_longpv==1 )&&(flag_start==1) && ( flag_end==0)) {
			//detect device type and resist in the 10s full heat!

			if(ABC!=0) {
				ABC=0;
			}

			flag_devicedetstart=0;
		}

//ADC_DMA_IRQ define in C/C++ Preprocessor Symbols Define
#ifdef ADC_DMA_IRQ
		adc_to_deviceR();
#endif

		first_pwm_adj();
		empty_load_detect();
		vbat_high_temp_protect();
		temp_and_time_key_parse();
		adcload_high_detect();

		low_power_handle();
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15) == 0 ) {
			//flag_e1=1;
		} else {
			//flag_e1 =0;
		}
#endif	
	}
}

