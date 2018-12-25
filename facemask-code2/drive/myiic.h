#ifndef __MYIIC_H
#define	__MYIIC_H

#include "stm32f0xx.h"
#include <stdio.h>

void IIC_Init(void);
void SDA_OUT(void);
void SDA_IN(void);
uint8_t SDA_READ(void);
void IIC_SDA_1(void);
void IIC_SDA_0(void);
void IIC_SCL_1(void);
void IIC_SCL_0(void);
void IIC_Start(void);
void IIC_Stop(void);
uint8_t IIC_Wait_Ack(void);
void IIC_Ack(void);
void IIC_NAck(void);
void IIC_Send_Byte(uint8_t txd);
uint8_t IIC_Read_Byte(unsigned char ack);
//uint8_t ReadData(void);
//void WriteData(uint8_t DataToWrite);

#endif /* __MYIIC_H */
