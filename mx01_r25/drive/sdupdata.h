#ifndef __SDUPDATA_H
#define __SDUPDATA_H

#include "stm32f0xx.h"
#include "w25x16.h"
#include "ili9328.h"
#include "stdio.h"						 //printf�������ļ�
#include "string.h"						 //�ַ������ļ�
#include "integer.h"
#include "ffconf.h"
#include "diskio.h"
#include "ff.h"

#define EN_UPDATE_FONT //ʹ���������,ͨ���ر�����ʵ�ֽ�ֹ�ֿ����

 
extern uint32_t FONT16ADDR ;
extern uint32_t FONT12ADDR ;
extern uint32_t UNI2GBKADDR;

uint8_t Update_Font(void);//�����ֿ�   
uint8_t Font_Init(void);//��ʼ���ֿ�


#endif /* _SDUPDATA_H */