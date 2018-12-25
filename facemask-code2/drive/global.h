#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "stm32f0xx.h"

static char Tx_Buffer[100];
//static float POWER_RANK[3];

//static char IS_FACE_EYE = 2; //0: is face mask;  1: is eys mask. 2��other
//static float CURRENT_POWER = -1; //current power value including face and eye mask.

//static char ADC_ON_OFF = 2; //0 : ADC is ON ; 1: ADC is OFF; 2: other
//static char ADC_END = 0; //0 : ADC is not finished; 1: ADC is finished

//static uint8_t R_value_New = 255; //  current resistance value of product.(default)
//static uint8_t R_value_Old = 254; //  last resistance value of product.(default)

//static uint16_t CURRENT_DUTY_PULSE = 0;  //��ǰռ�ձ�ֵ

//static uint32_t WRITE_24C02_ADDR = 0; //д24c02���׵�ַ

//static uint16_t TimerCounter = 0;
//static uint8_t  Long_Press_Flag =0; // 0: No long press; 1: long press.
//static uint8_t System_ON_OFF = 0 ; // 0: system off; 1: system on.
//static uint8_t Timing_flag = 0 ; // 0: 10 minutes; 1: 15 minutes; 2: 20 minutes.
//static uint8_t Power_rank_flag = 0; //0: rank1 ; 1: rank2 ; 2: rank3

#define VOLTAGE_ENLARGEMENT_FACTOR  1000 //��ѹ�Ŵ���  
#define ADC_PRECISION  4096 //ģ��ת���ľ���
#define ADC_REF_VOLTAGE  3.3 //ģ��ת���Ĳο���ѹ
#define ADC_REF_convert  0.805664 //ģ��ת���Ĳο���ѹ

#define VOUT_VOUT_DETECT_RATIO  10 //VOUT��VOUT_DETECT�ı�ֵ����Ӳ��ԭ��ͼ��

#define VOUT_ACCURACY 5  //�Ŵ�10���������ѹ����ֵ������ֵΪ0.5V
#define DUTY_CORR_V 4.35  //ռ�ձ����ѹ�Ķ�Ӧ��ϵ��ͨ�����ԣ�1%->0.23V
#define DUTY_ENLAR_FACTOR 1000  //ռ�ձȷŴ�����1000��
#define R36_RATIO 110/10  //vOUT��R36/(R63+R34)

#define VOUT_DETECT_MIN 0.3  //�����ѹ��������
#define VOUT_DETECT_MAX 2.7 //3 : R�����裩=10:110ŷķ
#define LOAD_DETECT 2 //2.9  // �������������������
#define VIN_DETECT_MIN 1.5  //�����ѹ������������
#define VIN_DETECT_MAX 2.65
#define V_MAX 28 //�������ѹֵ������28V

//���ֺ㹦�ʲ�������
#define P_FACE_MIN 8 //10  
#define P_FACE_MID 11.5 //17 
#define P_FACE_MAX 15  //25

//#define P_FACE_MIN 15 //10  
//#define P_FACE_MID 20 //17 
//#define P_FACE_MAX 25  //25

//���ֺ㹦�ʲ�������
#define P_EYE_MIN 2.5 //3.5 
#define P_EYE_MID 3.8 //5
#define P_EYE_MAX 5 //6.5

//��ʱʱ���������
#define TIME_MIN 600  //600 seconds
#define TIME_MID 900   //900 seconds
#define TIME_MAX 1200  //1200 seconds

#define NO_CONNECT_LOADER_TIME 60  //60 seconds

#define FACE_R_MIN 15 //�������޵���ֵ
//#define FACE_R_MAX 45 //�������޵���ֵ
//#define EYE_R_MIN 50 //�������޵���ֵ
#define FACE_R_MAX 40 //�������޵���ֵ
#define EYE_R_MIN 44 //�������޵���ֵ
#define EYE_R_MAX 150 //�������޵���ֵ

//PWM���ڲ���
#define PWM_V_REF 1.21  //VREF=1.21V
#define PWM_R_RATIO 22.1 // ����RUP�͵���RDOWN�ı�ֵ22.1

//24c02��������
#define WRITE_24C02_PERIOD 60  //ÿ60��дһ��24c02 
#define _24C02_ERASE_MAX   60000  //ÿ��Ԫ��д60000�κ󣬸����洢��Ԫ

#endif  /* _GLOBAL_H_ */

