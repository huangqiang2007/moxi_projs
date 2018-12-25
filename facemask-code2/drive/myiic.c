/**
  ******************************************************************************
  * @file      i2c simu.c
  * @brief     simulation function
  * @CPU       STM32F051
  * @compiler  Keil uVision V4.74
  * @author    MetalSeed
  * @copyright WSHHB
  * @version   V1.0.0
  * @date      18-Sept-2014
  * @modifydate20-Sept-2014
  ******************************************************************************
  * @attention
  */
#include "myiic.h"
#include "delay.h"


GPIO_InitTypeDef        GPIO_InitStructure;  

#define _24C02_RCC_AHBPeriph_GPIOX   RCC_AHBPeriph_GPIOB
#define _24C02_GPIOX   GPIOB
#define _24C02_SCL      GPIO_Pin_6//GPIO_Pin_6
#define _24C02_SDA     GPIO_Pin_7  //GPIO_Pin_7

/**
  * @brief  IIC Init
  * @param  A:
  * @retval None
  */
void IIC_Init(void)
{					     
	
	RCC_AHBPeriphClockCmd(	_24C02_RCC_AHBPeriph_GPIOX, ENABLE );	
	GPIO_InitStructure.GPIO_Pin = _24C02_SCL | _24C02_SDA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT ;   //推挽输出
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(_24C02_GPIOX, &GPIO_InitStructure);
	GPIO_SetBits(_24C02_GPIOX,_24C02_SCL | _24C02_SDA); 	//PB6,PB7 输出高
}

/**
  * @brief  Set SDA Pin as Output Mode
  * @retval None
  */
void SDA_OUT(void)  
{  
  
	GPIO_StructInit(&GPIO_InitStructure);  
  GPIO_InitStructure.GPIO_Pin   = _24C02_SDA;  
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;  
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   
  GPIO_Init(_24C02_GPIOX, &GPIO_InitStructure);  
}  

/**
  * @brief  Set SDA Pin as Input Mode
  * @retval None
  */
void SDA_IN(void)  
{  
 
	GPIO_StructInit(&GPIO_InitStructure);  
  GPIO_InitStructure.GPIO_Pin   = _24C02_SDA;  
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;// !!!
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   
  GPIO_Init(_24C02_GPIOX, &GPIO_InitStructure);  
} 

/**
  * @brief  read input voltage from SDA pin
  * @retval None
  */
uint8_t SDA_READ(void)
{
  return GPIO_ReadInputDataBit(_24C02_GPIOX, _24C02_SDA);
}

/**
  * @brief  output high form SDA pin
  * @retval None
  */
void IIC_SDA_1(void)
{
  GPIO_SetBits(_24C02_GPIOX, _24C02_SDA);
}

/**
  * @brief  output low form SDA pin
  * @retval None
  */
void IIC_SDA_0(void)
{
  GPIO_ResetBits(_24C02_GPIOX, _24C02_SDA);
}

/**
  * @brief  output high form SCL pin
  * @retval None
  */
void IIC_SCL_1(void)
{
GPIO_SetBits(_24C02_GPIOX, _24C02_SCL);
}

/**
  * @brief  output LOW form SCL pin
  * @retval None
  */
void IIC_SCL_0(void)
{
GPIO_ResetBits(_24C02_GPIOX, _24C02_SCL);  
}


/**
* @brief  Simulate IIC conmunication :Create Start signal
  * @retval None
  */
void IIC_Start(void)
{
	SDA_OUT();     //sda output
	IIC_SDA_1();	  	  
	IIC_SCL_1();
	delay_us(4);
 	IIC_SDA_0();   //START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL_0();   //hold scl line, prepare to transmit data
}	  

/**
  * @brief  Simulate IIC conmunication : Create Stop signal
  * @retval None
  */
void IIC_Stop(void)
{
	SDA_OUT();    //sda output mode 
	IIC_SCL_0();
	IIC_SDA_0();  //STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL_1(); 
	IIC_SDA_1();  //indicate transmit over
	delay_us(4);							   	
}

/**
* @brief  Simulate IIC conmunication : wait for target device's ACK
* @retval ACK (0) : receive success
* @retval NACK(1) : receive unsuccess
  */
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime = 0;
	SDA_IN();      //set as input mode
	IIC_SDA_1();
  delay_us(1);	   
	IIC_SCL_1();
  delay_us(1);	 
	while(SDA_READ())
	{
		ucErrTime++;
		if(ucErrTime > 250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL_0(); //release scl line
	return 0;  
} 

/**
  * @brief  Simulate IIC conmunication : make an ACK
  * @retval None
  */
void IIC_Ack(void)
{
	IIC_SCL_0();
	SDA_OUT();
	IIC_SDA_0();
	delay_us(2);
	IIC_SCL_1();
	delay_us(2);
	IIC_SCL_0();
}

/**
  * @brief  Simulate IIC conmunication : don't make an ACK
  * @retval None
  */
void IIC_NAck(void)
{
	IIC_SCL_0();
	SDA_OUT();
	IIC_SDA_1();
	delay_us(2);
	IIC_SCL_1();
	delay_us(2);
	IIC_SCL_0();
}					 				     


/**
  * @brief  Simulate IIC conmunication : Transmit one byte Data
  * @param  txd: data to be transmit
  * @retval None
  */
void IIC_Send_Byte(uint8_t txd)
{                        
  uint8_t i;   
  SDA_OUT(); 	    
  IIC_SCL_0();//push down scl  to start transmit data
  for(i = 0; i < 8; ++i)
  {              
    if(txd & 0x80)
    {
      IIC_SDA_1();
    }
    else
    {
      IIC_SDA_0();
    }
    txd <<= 1; 	  
    delay_us(2);   
    IIC_SCL_1();
    delay_us(2); 
    IIC_SCL_0();	
    delay_us(2);
  }	 
} 	  

//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
/**
  * @brief  Simulate IIC conmunication : Receive one byte Data
  * @param  ack: Whether transmit ACK
  * @retval the data have been receive
  */
uint8_t IIC_Read_Byte(unsigned char ack)
{
	unsigned char i, res = 0;
	SDA_IN();               //SDA input mode
  for(i = 0; i < 8; ++i )
	{
    IIC_SCL_0(); 
    delay_us(2);
    IIC_SCL_1();
    res <<= 1;
    if(SDA_READ())
    {
      res++; 
    }      
		delay_us(1); 
  }					 
  if (!ack)
  {
    IIC_NAck();//make NACK
  }
  else
  {
    IIC_Ack(); //make ACK
  }
  return res;
}


/*JUST SOME TEST FUNTION DEMO*/
uint8_t ReadData(void)
{				  
	uint8_t temp=0;		  	    																 
  IIC_Start();  
	
  IIC_Send_Byte(0x80); 
	IIC_Wait_Ack(); 
  
  IIC_Send_Byte(0xf5); 
	IIC_Wait_Ack();	    
	
  IIC_Start();  	 	   
	IIC_Send_Byte(0XA1); 
	IIC_Wait_Ack();	 
  
  temp=IIC_Read_Byte(0);		   
  IIC_Stop();
	return temp;
}

void WriteData(uint8_t DataToWrite)
{				   	  	    																 
  IIC_Start();  
  IIC_Send_Byte(0x80);
  if( IIC_Wait_Ack() == 0)
  {
     //printf("Recieve ACK \n");
  }    
  
  IIC_Send_Byte(0xf5);
	IIC_Wait_Ack(); 	 										  		   
	
  IIC_Send_Byte( DataToWrite ); 
	IIC_Wait_Ack();  		    	   
  
  IIC_Stop();
	delay_ms(10);	 
}

