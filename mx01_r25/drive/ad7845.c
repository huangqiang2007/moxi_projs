// ------------------------------------
// Ӳ�����ӣ�PB10---T-INT
//           PA4----T-CS 
//           PA5----T-CLK
//           PA6----T-OUT
// 					 PA7----T-DIN
//           SPI1-- AF0
//
// -------------------------------------
#include "ad7845.h"


// A/D ͨ��ѡ�������ֺ͹����Ĵ���
#define	CHX 	0x90 	//ͨ��X+��ѡ�������	
#define	CHY 	0xd0	//ͨ��Y+��ѡ������� 


static void Delayus( int k)
{
 int j;
    
    for(j=k; j > 0; j--);    
}


static uint16_t RD_AD(uint8_t cmd) 
{
 uint8_t temp;
 uint16_t buf=0;
	
    //buf=GPIOD->IDR;
    //if(isBit16(buf, 9)==1)
    //   return(0);
    
    SPI_TOUCH_CS_LOW()   //TP_CS(0);

    //os_dly_wait(1);//300ms���ٲ���1��
    Delayus(50);
    
    SPI_Send_Byte(cmd);
    
    temp=SPI_Send_Byte(0xff);
    buf = temp;
    buf <<= 8;
    temp = SPI_Send_Byte(0xff);
    
 
   SPI_TOUCH_CS_HIGH(); //TP_CS(1);
    
    
    
    buf += temp;
    
    buf>>=4;//ֻ��12λ��Ч
    //buf&=0x0fff;
    return(buf);
}

//����������� ReadLoop�ɼ������� LOSS_DATA�������ֵ����Сֵ����

#define ReadLoop 13 //�����ɼ���������>3
#define LOSS_DATA 5 //���������С���ݸ���
/*
���ܣ��ɼ�TP��������

�����
  xValue X����ɼ�����ֵ,  yValue Y����ɼ�����ֵ
���أ�
  0=��Ч���ݣ�1=��Ч����

˵�����ɼ�13�Σ�Ȼ�����򣬱����м�3��ȡƽ��ֵ
*/
uint8_t Read_XY(uint16_t *xValue, uint16_t *yValue) 
{ 
 uint16_t i, j;
 uint16_t bufX[ReadLoop];
 uint16_t bufY[ReadLoop];
 uint32_t sumX;
 uint32_t sumY;
 uint16_t val;
    
    //�����ɼ�X���꣬Y��������
    for(i=0; i<ReadLoop; i++)
    {
       //����ɼ���û�꣬�����Ѿ�̧����ô�����ôβɼ�
       if(TP_IRQ)
          return(0);
       
       //while(TP_BUSY);
       //Delayus(5);
       bufX[i]=RD_AD(CHY);
       
       //while(TP_BUSY);
       //Delayus(5);
       bufY[i]=RD_AD(CHX);
       
       Delayus(5);
    }
    
    
    //�Բɼ���������
    for(i=0; i<ReadLoop-1; i++)
    {
       for(j=i+1; j<ReadLoop; j++)
       {
          if(bufX[i]>bufX[j])
          {
             val=bufX[i];
             bufX[i]=bufX[j];
             bufX[j]=val;
          }
          
          if(bufY[i]>bufY[j])
          {
             val=bufY[i];
             bufY[i]=bufY[j];
             bufY[j]=val;
          }
       }
    }
    
    //���������С���ݺ����
    sumX=0;sumY=0;
    for(i=LOSS_DATA; i<ReadLoop-LOSS_DATA; i++)
    {
       sumX += bufX[i];
       sumY += bufY[i];
    }
    
    //ȡƽ��ֵ
    *xValue=sumX/(ReadLoop-2*LOSS_DATA);
    *yValue=sumY/(ReadLoop-2*LOSS_DATA);
    
    return (1);    
}


/*
���ܣ��ɼ�TP��������
�����
  x��x����
  y��y����
���أ�
  0=��Ч���ݣ�1=��Ч����
*/
uint8_t TP_GetAdXY(uint16_t *x, uint16_t *y) 
{
 uint8_t flag;
    
    if(TP_IRQ)
       return(0);
    flag=Read_XY(x, y);
    
    if(flag==0)
       return(0);
      
    //һ�������X,Y����0�󣬾��������ʵ���������С��һ����Χ�Ŀ�����Ϊ����Ч����
    if(*x<100 || *y<100)
       return(0);
    else
       return(1);
}

extern void Delay(unsigned long time);
/*
���ܣ���������������2�Σ����ʱ����delay����ȷ��,2�β������+-50��Χ�ڲ�����Ч
2����ָ�����ɼ���2����������Ч���ݣ�����ʵ���ǲɼ�2*13=26������
�����
  x��x����
  y��y����
���룺
  delay���������ʱ�䣬��λms

���أ�
  0=��Ч���ݣ�1=��Ч����
*/
uint8_t TP_GetAdXY2(uint16_t *x, uint16_t *y, uint32_t delay) 
{uint16_t x1,y1;
 uint16_t x2,y2;
 uint8_t flag;
 //u16 adx,ady;
 

    flag=TP_GetAdXY(&x1, &y1);
    
    if(flag==0)
       return(0);
    
    //os_dly_wait(delay/OS_TIME);//300ms���ٲ���1��
	Delay(delay);
    
    flag=TP_GetAdXY(&x2, &y2);
    
    if(flag==0)
       return(0);
    
    //���β���������뱣����һ����Χ�ڣ�����ֵ������ʵ�������
    if( ( (x2<=x1 && x1<x2+50) || (x1<=x2 && x2<x1+50) )//ǰ�����β�����+-50��
     && ( (y2<=y1 && y1<y2+50) || (y1<=y2 && y2<y1+50) ) )
    {
        *x=(x1+x2)/2;
        *y=(y1+y2)/2;
        return(1);
    }
    else
       return(0);
}





int setCalibrationMatrix( POINT * displayPtr,
                          POINT * screenPtr,
                          MATRIX * matrixPtr)
{

    int  retValue = OK ;



    matrixPtr->Divider = ((screenPtr[0].x - screenPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) -
                         ((screenPtr[1].x - screenPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)) ;

    if ( matrixPtr->Divider == 0 )
    {
        retValue = NOT_OK ;
    }
    else
    {
        matrixPtr->An = ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) -
                        ((displayPtr[1].x - displayPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)) ;

        matrixPtr->Bn = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].x - displayPtr[2].x)) -
                        ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].x - screenPtr[2].x)) ;

        matrixPtr->Cn = (screenPtr[2].x * displayPtr[1].x - screenPtr[1].x * displayPtr[2].x) * screenPtr[0].y +
                        (screenPtr[0].x * displayPtr[2].x - screenPtr[2].x * displayPtr[0].x) * screenPtr[1].y +
                        (screenPtr[1].x * displayPtr[0].x - screenPtr[0].x * displayPtr[1].x) * screenPtr[2].y ;

        matrixPtr->Dn = ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].y - screenPtr[2].y)) -
                        ((displayPtr[1].y - displayPtr[2].y) * (screenPtr[0].y - screenPtr[2].y)) ;

        matrixPtr->En = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].y - displayPtr[2].y)) -
                        ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].x - screenPtr[2].x)) ;

        matrixPtr->Fn = (screenPtr[2].x * displayPtr[1].y - screenPtr[1].x * displayPtr[2].y) * screenPtr[0].y +
                        (screenPtr[0].x * displayPtr[2].y - screenPtr[2].x * displayPtr[0].y) * screenPtr[1].y +
                        (screenPtr[1].x * displayPtr[0].y - screenPtr[0].x * displayPtr[1].y) * screenPtr[2].y ;
    }

    return( retValue ) ;

} /* end of setCalibrationMatrix() */

int getDisplayPoint( POINT * displayPtr,
                     POINT * screenPtr,
                     MATRIX * matrixPtr )
{
    int  retValue = OK ;


    if ( matrixPtr->Divider != 0 )
    {

        /* Operation order is important since we are doing integer */
        /*  math. Make sure you add all terms together before      */
        /*  dividing, so that the remainder is not rounded off     */
        /*  prematurely.                                           */

        displayPtr->x = ( (matrixPtr->An * screenPtr->x) +
                          (matrixPtr->Bn * screenPtr->y) +
                          matrixPtr->Cn
                        ) / matrixPtr->Divider ;

        displayPtr->y = ( (matrixPtr->Dn * screenPtr->x) +
                          (matrixPtr->En * screenPtr->y) +
                          matrixPtr->Fn
                        ) / matrixPtr->Divider ;
    }
    else
    {
        retValue = NOT_OK ;
    }

    return( retValue ) ;

} /* end of getDisplayPoint() */