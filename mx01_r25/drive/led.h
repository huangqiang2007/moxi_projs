#ifndef __LED_H
#define	__LED_H

#include "stm32f0xx.h"
/* Below define shall define by project begin*/
//#define LED1_PIN      	GPIO_Pin_1
//#define LED2_PIN       	GPIO_Pin_2
#define LED3_PIN      	GPIO_Pin_12
#define LED4_PIN       	GPIO_Pin_10
#define LED5_PIN      	GPIO_Pin_11
#define LED6_PIN       	GPIO_Pin_8
#define LED7_PIN      	GPIO_Pin_9
#define LED8_PIN       	GPIO_Pin_5
#define LED9_PIN       	GPIO_Pin_6
#define LED10_PIN       GPIO_Pin_4
#define LED15_PIN       GPIO_Pin_7

//green led
#define LED_TEMP1		LED6_PIN
#define LED_TEMP2		LED7_PIN
#define LED_TEMP3		LED4_PIN
#define LED_TEMP4		LED5_PIN
#define LED_TEMP5		LED3_PIN

//yellow led
#define LED_TIME1		LED15_PIN
#define LED_TIME2		LED8_PIN
#define LED_TIME3		LED9_PIN

//show error indicate
#define LED_ERROR		LED10_PIN

#define LED_PORT       GPIOA

/* About define shall define by project end*/

#define AW9523_REG_ID		0x10
#define AW9523_REG_OUTPUTP0		0x02
#define AW9523_REG_OUTPUTP1		0x03
#define sEE_HW_ADDRESS         0xB6
#define sEE_I2C_TIMING          0x00210507
#define sEE_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define sEE_LONG_TIMEOUT         ((uint32_t)(10 * sEE_FLAG_TIMEOUT))

/* Maximum number of trials for sEE_WaitEepromStandbyState() function */
#define sEE_MAX_TRIALS_NUMBER     300
      
#define sEE_OK                    0
#define sEE_FAIL                  1   


#define sEE_I2C                          I2C1
#define sEE_I2C_CLK                      RCC_APB1Periph_I2C1
   
#define sEE_I2C_SCL_PIN                  GPIO_Pin_6                  /* PB.06 */
#define sEE_I2C_SCL_GPIO_PORT            GPIOB                       /* GPIOB */
#define sEE_I2C_SCL_GPIO_CLK             RCC_AHBPeriph_GPIOB
#define sEE_I2C_SCL_SOURCE               GPIO_PinSource6
#define sEE_I2C_SCL_AF                   GPIO_AF_1

#define sEE_I2C_SDA_PIN                  GPIO_Pin_7                  /* PB.07 */
#define sEE_I2C_SDA_GPIO_PORT            GPIOB                       /* GPIOB */
#define sEE_I2C_SDA_GPIO_CLK             RCC_AHBPeriph_GPIOB
#define sEE_I2C_SDA_SOURCE               GPIO_PinSource7
#define sEE_I2C_SDA_AF                   GPIO_AF_1

void LED_Init(void);
void LevelLED_OffAll(void);
void LevelLED_On(int led_num);
void LevelLED_Off(int led_num);
void Mode_led_Blinking(int on);
void TimerLED_Off(void);
void TimerLED_On(int led_num);

void WARNLED_ON(void);
void WARNLED_OFF(void);
void WARNLED_Toggle(void);

#endif /* __LED_H */

