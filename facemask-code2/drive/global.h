#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "stm32f0xx.h"

static char Tx_Buffer[100];
#define VOLTAGE_ENLARGEMENT_FACTOR  1000 //电压放大倍数  
#define ADC_PRECISION  4096 //模数转换的精度
#define ADC_REF_VOLTAGE  3.3 //模数转换的参考电压
#define ADC_REF_convert  0.805664 //模数转换的参考电压

#define VOUT_VOUT_DETECT_RATIO  10 //VOUT与VOUT_DETECT的比值，见硬件原理图。

#define VOUT_ACCURACY 5  //放大10倍的输出电压精度值；精度值为0.5V
#define DUTY_CORR_V 4.35  //占空比与电压的对应关系，通过测试：1%->0.23V
#define DUTY_ENLAR_FACTOR 1000  //占空比放大倍数，1000倍
#define R36_RATIO 110/10  //vOUT：R36/(R63+R34)

#define VOUT_DETECT_MIN 0.3 //输出电压参数定义
#define VOUT_DETECT_MAX 2.7 // //3 : R（电阻）=10:110欧姆
#define LOAD_DETECT 2 //2.9  // 输出过流保护参数定义
#define VIN_DETECT_MIN 1.5  //输入电压保护参数定义
#define VIN_DETECT_MAX 2.65
#define V_MAX 28 //输出最大电压值限制在28V

//面罩恒功率参数定义
#define P_FACE_MIN 8 //10  
#define P_FACE_MID 11.5 //17 
#define P_FACE_MAX 15  //25

//眼罩恒功率参数定义
#define P_EYE_MIN 2.5 //3.5 
#define P_EYE_MID 3.8 //5
#define P_EYE_MAX 5 //6.5

//定时时间参数定义
#define TIME_MIN 600  //600 seconds
#define TIME_MID 900   //900 seconds
#define TIME_MAX 1200  //1200 seconds

#define NO_CONNECT_LOADER_TIME 60  //60 seconds

#define FACE_R_MIN 15 //面罩下限电阻值
#define FACE_R_MAX 40 //面罩上限电阻值
#define EYE_R_MIN 44 //眼罩下限电阻值
#define EYE_R_MAX 150 //眼罩上限电阻值

//PWM调节参数
#define PWM_V_REF 1.21  //VREF=1.21V
#define PWM_R_RATIO 22.1 // 电阻RUP和电阻RDOWN的比值22.1

//24c02参数配置
#define WRITE_24C02_PERIOD 60  //每60秒写一次24c02 
#define _24C02_ERASE_MAX   60000  //每单元擦写60000次后，更换存储单元

#endif  /* _GLOBAL_H_ */
