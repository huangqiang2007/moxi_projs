/******************************************************************/
/* ??3?��oTFT?y?��                                                   */
/* ��2?t��??����oPB0-PB7---DB0-DB7   �̨�8??                             */
/*           PC0-PC7---DB8-DB7   ??8??                             */
/*           PD2---WR  PC12---RD PC11---CS  PC10---RS  PB11---CM     */            
/* ����??��o?����?                                                     */
/*                  �ꡧ�̨�?����o	qfv5.taobao.com��?                   */
/* ��a?�̡�?��?��oQQ:157736309                                         */
/******************************************************************/

#include"stm32f0xx.h"
#include"ili9328.h"
#include "w25x16.h"
#include "ascii.h"

uint16_t  POINT_COLOR=BLACK;
uint16_t  BACK_COLOR=WHITE;
/**************************************************/
/* o����y1|?����??��?Y?��������??aLCD3?��??������D����t??        */
/**************************************************/	

static void delay (int cnt) {

  cnt <<= DELAY_2N;
  while (cnt--);
}
/***************************************************/
/* o����y1|?����???ILI9325��???��??��D�䨺y?Y               */
/* ��??��2?��y��?val��o16??��y?Y                         */
/* ?��?�¡�o    D��16??��y?Y��??��?���?��?��?LCD2����?8??��??����?*/
/*           ?����?��?16??��y?Y��?��?��?D��????��??�¡�??��D��??*/
/*           ??��??��D��̨�???��                        */
/* ���騰a��o��1��?����D����a?����1��?LCD_WR_REG(reg)????��a��?��y */
/*       ?YD���???????��??�¨�????��                    */
/***************************************************/
void LCD_WR_DATA(uint16_t val)
{  
	
 	Set_Rs;
	Clr_Cs;
// 	Set_nRd;	
  GPIOB->ODR &= 0xff00;
  GPIOC->ODR &= 0xff00;		
	GPIOB->ODR|=((val)&(0x00ff));
	GPIOC->ODR|=(val>>8);
	Clr_nWr;
	Set_nWr;
  Set_Cs;	
}

/****************************************************/
/* o����y1|?����o??ILI9238��?D��?����?2������??��|??��??��         */
/* ��??��2?��y��?D��?����?                                 */
/* ?��?�¡�o    ??��??��o?��?16??��y��?8??��??����?��?D����aD�䨢?��?*/
/****************************************************/
void LCD_WR_REG(uint16_t cmd)
{
	Clr_Rs;
	Clr_Cs;
// 	Set_nRd;
	 GPIOB->ODR &= 0xff00;
   GPIOC->ODR &= 0xff00;	
	GPIOB->ODR |=((cmd)&(0x00ff));
	GPIOC->ODR|=(cmd>>8);
	Clr_nWr;
	Set_nWr;
	 Set_Cs;	
	}
/******************************************************/
/* o����y1|?����w?��??��??�¨�?D�䨺y?Y                        */
/* ��??��2?��y��?reg��o??��??��o?  data��o��y?Y?��             */
/* ?��?�¡�o    ??o����y��??�㨢???o����y��?o?3����?��2?����?��???��??? */
/*           o����y��?��?��?��?����?����??a?������o??��             */
/******************************************************/

void LCD_WR_REG_DATA(uint16_t reg, uint16_t data)
{ 
	 
   	LCD_WR_REG(reg);
   	LCD_WR_DATA(data);
 	
	}	
	
	
/****************************************************/
/* o����y1|?����o�䨮ILI9238?D?����y?Y                      */
/* ��??��2?��y��?��y?Y                               */
/* ?��?�¡�o    ??��??��o?��?16??��y��?8??��??����?��?D����aD�䨢?��?*/
/****************************************************/
uint16_t LCD_RD_DATA(void)
{ 
	unsigned short val,val1,val2;
	Set_Rs;
	Set_nWr;
	Clr_nRd;
	GPIOB->MODER&=0xffff0000;		   
  GPIOC->MODER&=0xffff0000;
	val1 = GPIOB->IDR;
	val2 = GPIOC->IDR;
	val=((val1)&(0x00ff))+((val2<<8)&(0xff00));
	               //��?��?������?o���䨮D?����???a��?3?
		GPIOB->MODER|=0x00005555; 
		GPIOC->MODER|=0x00005555; 
	Set_nRd;
	return val;
}	

/******************************************************/
/* o����y1|?����w?��??��??�¨�?��?��y?Y                        */
/* ��??��2?��y��?reg��o??��??��o?                           */
/* ?��?�¡�o    ??o����y��??�㨢???o����y��?o?3����?��2?����?��???��??? */
/*           o����y��?��?��?��?����?����??a?������o??��             */
/******************************************************/
uint16_t  LCD_RD_REG_DATA (uint16_t  reg) 
{
  uint16_t value;
 	Clr_Cs;
	LCD_WR_REG(reg);    
	value = LCD_RD_DATA();      	
	Set_Cs;
	return value;
}

/*******************************************************/
/* o����y1|?����o3?��??��LCD                                 */
/*******************************************************/
void LCD_init (void) 
{ uint16_t i;
	//////////////////////////////////////////////////////////////////////1��??3?��??��
	 RCC->AHBENR&=0xffe3ffff;
	 RCC->AHBENR|=0x001c0000;
	
	GPIOB->MODER&=0xffff0000;		   // ����??PB?���̨�8??��y???a��?3?��?��?����LCD16??2��DD��y?Yo��????��y??
	GPIOB->MODER|=0x00005555;  
// 	GPIOB->BSRR=0x00ff;
  GPIOB->ODR &= 0xff00;	
// 	GPIOB->ODR |= 0x00ff;	   // PB?���̨�8??��y??????
	
	GPIOC->MODER&=0xffff0000;		   // ����??PC?���̨�8??��y???a��?3?��?��?����LCD16??2��DD��y?Yo��????��y??
	GPIOC->MODER|=0x00005555; 
// 	GPIOC->BSRR=0x00ff;
	GPIOC->ODR &= 0xff00;	
// 	GPIOC->ODR |= 0x00ff;	   // PC?���̨�8??��y??????
	
	GPIOB->MODER&=0xff3fffff;
	GPIOB->MODER|=0x00400000;  //PB11?a��?3??�꨺?
	GPIOB->BRR =0x00000800;    //��?3??a�̨���???
	
	 GPIOC->MODER&= 0xfc0fffff;                                   //PD2---WR  PC12---RD PC11---CS  PC10---RS  
   GPIOC->MODER|=0x01500000;
	 
	 GPIOC->BSRR = 0x00001c00;
	 GPIOD->MODER&= 0xffffffcf;
   GPIOD->MODER|=0x00000010;
	 
	 GPIOD->BSRR = 0x00000004;
//////////////////////////////////////////////////////////////////	
	
	delay(5);                             /* Delay 50 ms                        */

	LCD_WR_REG_DATA(0x00e7,0x0010);      
	LCD_WR_REG_DATA(0x0000,0x0001);  			//start internal osc
	LCD_WR_REG_DATA(0x0001,0x0100);     
	LCD_WR_REG_DATA(0x0002,0x0700); 				//power on sequence                     
	LCD_WR_REG_DATA(0x0003,(1<<12)|(1<<5)|(1<<4) ); 	//65K 
	LCD_WR_REG_DATA(0x0004,0x0000);                                   
	LCD_WR_REG_DATA(0x0008,0x0207);	           
	LCD_WR_REG_DATA(0x0009,0x0000);         
	LCD_WR_REG_DATA(0x000a,0x0000); 				//display setting         
	LCD_WR_REG_DATA(0x000c,0x0001);				//display setting          
	LCD_WR_REG_DATA(0x000d,0x0000); 				//0f3c          
	LCD_WR_REG_DATA(0x000f,0x0000);
	//Power On sequence //
	LCD_WR_REG_DATA(0x0010,0x0000);   
	LCD_WR_REG_DATA(0x0011,0x0007);
	LCD_WR_REG_DATA(0x0012,0x0000);                                                                 
	LCD_WR_REG_DATA(0x0013,0x0000);                 
	for(i=50000;i>0;i--);
	for(i=50000;i>0;i--);
  LCD_WR_REG_DATA(0x0010,0x1590);   
	LCD_WR_REG_DATA(0x0011,0x0227);
	for(i=50000;i>0;i--);
	for(i=50000;i>0;i--);
	LCD_WR_REG_DATA(0x0012,0x009c);                  
	for(i=50000;i>0;i--);
	for(i=50000;i>0;i--);
	LCD_WR_REG_DATA(0x0013,0x1900);   
	LCD_WR_REG_DATA(0x0029,0x0023);
	LCD_WR_REG_DATA(0x002b,0x000e);
	for(i=50000;i>0;i--);
	for(i=50000;i>0;i--);
	LCD_WR_REG_DATA(0x0020,0x0000);                                                            
	LCD_WR_REG_DATA(0x0021,0x0000);           
	///////////////////////////////////////////////////////      
	for(i=50000;i>0;i--);
	for(i=50000;i>0;i--);
	LCD_WR_REG_DATA(0x0030,0x0007); 
	LCD_WR_REG_DATA(0x0031,0x0707);   
	LCD_WR_REG_DATA(0x0032,0x0006);
	LCD_WR_REG_DATA(0x0035,0x0704);
	LCD_WR_REG_DATA(0x0036,0x1f04); 
	LCD_WR_REG_DATA(0x0037,0x0004);
	LCD_WR_REG_DATA(0x0038,0x0000);        
	LCD_WR_REG_DATA(0x0039,0x0706);     
	LCD_WR_REG_DATA(0x003c,0x0701);
	LCD_WR_REG_DATA(0x003d,0x000f);
	for(i=50000;i>0;i--);
	for(i=50000;i>0;i--);
	LCD_WR_REG_DATA(0x0050,0x0000);        
	LCD_WR_REG_DATA(0x0051,0x00ef);   
	LCD_WR_REG_DATA(0x0052,0x0000);     
	LCD_WR_REG_DATA(0x0053,0x013f);
	LCD_WR_REG_DATA(0x0060,0xa700);        
	LCD_WR_REG_DATA(0x0061,0x0001); 
	LCD_WR_REG_DATA(0x006a,0x0000);
	LCD_WR_REG_DATA(0x0080,0x0000);
	LCD_WR_REG_DATA(0x0081,0x0000);
	LCD_WR_REG_DATA(0x0082,0x0000);
	LCD_WR_REG_DATA(0x0083,0x0000);
	LCD_WR_REG_DATA(0x0084,0x0000);
	LCD_WR_REG_DATA(0x0085,0x0000);
	
	LCD_WR_REG_DATA(0x0090,0x0010);     
	LCD_WR_REG_DATA(0x0092,0x0000);  
	LCD_WR_REG_DATA(0x0093,0x0003);
	LCD_WR_REG_DATA(0x0095,0x0110);
	LCD_WR_REG_DATA(0x0097,0x0000);        
	LCD_WR_REG_DATA(0x0098,0x0000);  
	 //display on sequence     
	LCD_WR_REG_DATA(0x0007,0x0133);
	
	LCD_WR_REG_DATA(0x0020,0x0000);                                                            
	LCD_WR_REG_DATA(0x0021,0x0000);
 
}

/****************************************************************************
* ??    3?��ovoid GLCD_Test(void)
* 1|    ?����o2a��?��o?��?��
* ��??��2?��y��o?T
* 3??��2?��y��o?T
* ?��    ?�¡�o??��?2����?��?2a��?��o?��?����?��??y3��1�����
* �̡¨�?��?������oGLCD_Test();
****************************************************************************/
void GLCD_Test(void)
{
  uint16_t i,j;
  LCD_WR_REG_DATA(0x20, 0);
  LCD_WR_REG_DATA(0x21, 0);

  Clr_Cs; 
  LCD_WR_REG(0x22);
  
  for(i=0;i<320;i++)
    for(j=0;j<240;j++)
    {
      if(i>279)LCD_WR_DATA(0x0000);
      else if(i>239)LCD_WR_DATA(0x001f);
      else if(i>199)LCD_WR_DATA(0x07e0);
      else if(i>159)LCD_WR_DATA(0x07ff);
      else if(i>119)LCD_WR_DATA(0xf800);
      else if(i>79)LCD_WR_DATA(0xf81f);
      else if(i>39)LCD_WR_DATA(0xffe0);
      else LCD_WR_DATA(0xffff);
    }
  Set_Cs;
}

/**********************************************/
/* o����y1|?����??a????��?                         */
/* ?��    ?�¡�o��?��?��?����1?��???��?o����??a??         */
/**********************************************/
void LCD_DisplayOn(void)
{
 	LCD_WR_REG_DATA(0x0007, 0x0133); // ?a????��?
}
/**********************************************/
/* o����y1|?����?1?��???��?                         */
/* ?��    ?�¡�o2��2?��?��1TFTo��?����???��?��1��?TFT��?��? */
/*           ?����Y����3?2?��?��?��?��?��?����TFT��?��y?Y??*/
/*           o��???��?��?����??����?2������???��?��?����?���� */
/*           o��2??�ꨪ?����?��TFT��???��??��          */
/**********************************************/
void LCD_DisplayOff(void)
{
	LCD_WR_REG_DATA(0x0007, 0x0); // 1?��???��?
}

/***************************************************/
/* o����y1|?����?����????��???����                          */
/* ?��    ?�¡�o����????��a??��?��???��?XY?e��?o��?����?��?����    */
/* ����    ��a��o��?������?o����a???��?e��?(0,0)o��??��?(239,319)*/
/***************************************************/
void LCD_XYRAM(uint16_t xstart ,uint16_t ystart ,uint16_t xend ,uint16_t yend)
{
	LCD_WR_REG_DATA(0x0050, xstart); // ����??o����?����GRAM?e��?��??��
	LCD_WR_REG_DATA(0x0051, xend); // ����??o����?����GRAM?����?��??��
	LCD_WR_REG_DATA(0x0052, ystart); // ����??��Y��?����GRAM?e��?��??��
	LCD_WR_REG_DATA(0x0053, yend); // ����??��Y��?����GRAM?����?��??��
}

/*******************************************************/
/* o����y1|?����o����??TFT?��?e��?��?����                         */
/*******************************************************/
void LCD_SetC(uint16_t x, uint16_t y)
{
	LCD_WR_REG_DATA(0x0020,x);  //����??X��?����????
	LCD_WR_REG_DATA(0x0021,y);  //����??Y��?����????
}
/**********************************************/
/* o����y1|?����????��TFT                          */
/* ��??��2?��y��ocolor ??��??��                     */
/**********************************************/
void LCD_Clear(uint16_t color)
{
    uint32_t temp;
    LCD_WR_REG_DATA(0x0020,0);//����??X��?����????
    LCD_WR_REG_DATA(0x0021,0);//����??Y��?����????
    LCD_WR_REG(0x0022);				//???��RAM??��??�¡�?��?��?D�䨺y?Y��?RAM
	for(temp=0;temp<76800;temp++)
	{
		LCD_WR_DATA(color);    
	}
}
/******************************************/
/* o����y1|?����o?-��???????��?��?               */
/* ��??��2?��y��ox,y   ��?��?��?����               */
/******************************************/
void LCD_DrawPoint(uint16_t x,uint16_t y)
{
	LCD_WR_REG_DATA(0x0020,x);//����??X��?����????
    LCD_WR_REG_DATA(0x0021,y);//����??Y��?����????
	LCD_WR_REG(0x0022);       //?a��?D�䨨?GRAM
	LCD_WR_DATA(POINT_COLOR); 
}
/******************************************/
/* o����y1|?����o?-��???D?��?��?����               */
/* ��??��2?��y��ox,y   ��?��?��?����               */
/******************************************/
void DrawCross(uint16_t x,uint16_t y)
{
    uint8_t b;    
    for(b=0; b<10; b++)
    {
        LCD_DrawPoint(240-(x),y-b);
        LCD_DrawPoint(240-(x),y+b);
        LCD_DrawPoint(240-(x+b),y);
        LCD_DrawPoint(240-(x-b),y);
    }
       
}	

/******************************************/
/* o����y1|?����o?��TFT?3��?��?��???��?            */
/* 3??��2?��y��ocolor  ��???��??��              */
/******************************************/
uint16_t LCD_ReadPoint(uint16_t x,uint16_t y)
{
	uint16_t  color;

	LCD_WR_REG_DATA(0x0020,x);//����??X��?����????
  LCD_WR_REG_DATA(0x0021,y);//����??Y��?����????
	LCD_WR_REG(0x0022);       //?a��?D�䨨?GRAM
	
	GPIOB->MODER&=0xffff0000;		   
  GPIOC->MODER&=0xffff0000;        //��?TFT��y?Y��y??����???a��?��?
	
	color = LCD_RD_DATA();	  //?��3?GRAM?�̡ꡧ���騰a��oGRAM?�̡�?D??����?��?��?��?
	color = LCD_RD_DATA();	  //?��3?GRAM?�̡ꡧ?��??ILI932x��y?Y��?2����?
	
	GPIOB->MODER|=0x00005555; 
	GPIOC->MODER|=0x00005555; 		  //???�䨺y?Y��y???a��?3?

	return color;
} 

/**********************************************/
/* o����y1|?����??-?��??                           */
/**********************************************/
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	uint16_t t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 

	delta_x=x2-x1; //????��?����??��? 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //����??�̣�2?��??�� 
	else if(delta_x==0)incx=0;//��1?��?? 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//?????? 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //??��??����???��?��?����?�� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//?-??��?3? 
	{  
		LCD_DrawPoint(uRow,uCol);//?-��? 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}
/**********************************************/
/* o����y1|?����??-??D?                           */
/**********************************************/
void LCD_DrawRectage(uint16_t xstart,uint16_t ystart,uint16_t xend,uint16_t yend,uint16_t color)
{
	POINT_COLOR=color;
	LCD_DrawLine(xstart, ystart, xend, ystart);
	LCD_DrawLine(xstart, yend, xend, yend);
	LCD_DrawLine(xstart, ystart, xstart, yend);
	LCD_DrawLine(xend, ystart, xend, yend);
}
/**********************************************/
/* o����y1|?����???D?��?3?                         */
/**********************************************/
void LCD_Fill(uint16_t xstart ,uint16_t ystart ,uint16_t xend ,uint16_t yend ,uint16_t color)
{                    
    uint32_t max;
	LCD_XYRAM(xstart ,ystart ,xend ,yend); // ����??GRAM��?����
	LCD_WR_REG_DATA(0x0020,xstart);//����??X��?����????
    LCD_WR_REG_DATA(0x0021,ystart);//����??Y��?����????
    LCD_WR_REG(0x0022);				//???��RAM??��??�¡�?��?��?D�䨺y?Y��?RAM
	max=(uint32_t)((xend-xstart+1)*(yend-ystart+1));    
	while(max--)
	{
		LCD_WR_DATA(color);
	}
	LCD_XYRAM(0x0000 ,0x0000 ,0x00EF ,0X013F); // ???��GRAM???��??��?
}
/*******************************************/
/* o����y1|?����o?-?2                          */
/* ��??��2?��y��ox0,y0  ?2D?��?����               */
/*           r      ��???(�̣�??��o????)       */
/*******************************************/
void LCD_DrawCircle(uint8_t x0, uint16_t y0, uint8_t r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //?D??????��?????��?����??
	while(a<=b)
	{
		LCD_DrawPoint(x0-b,y0-a);             //3           
		LCD_DrawPoint(x0+b,y0-a);             //0           
		LCD_DrawPoint(x0-a,y0+b);             //1       
		LCD_DrawPoint(x0-b,y0-a);             //7           
		LCD_DrawPoint(x0-a,y0-b);             //2             
		LCD_DrawPoint(x0+b,y0+a);             //4               
		LCD_DrawPoint(x0+a,y0-b);             //5
		LCD_DrawPoint(x0+a,y0+b);             //6 
		LCD_DrawPoint(x0-b,y0+a);             
		a++;
		//��1��?Bresenham??����?-?2     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 
		LCD_DrawPoint(x0+a,y0+b);
	}
} 
/**********************************************/
/* o����y1|?����??��m��?n��?��?                       */
/**********************************************/
uint32_t mypow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}			 
/**********************************************/
/* o����y1|?����o??��?��y��?                         */
/* ��??��2?��y��ox,y :?e��?��?����	 	              */
/*           len :��y��?��???��y				  */
/*           num:��y?��(0~4294967295);	   	  */
/**********************************************/
void LCD_ShowNum(uint8_t x,uint16_t y,uint32_t num,uint8_t len)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;		 // ��?��?��?��?���䨨����?��?????��?0	
				   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+8*t,y,' ');
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+8*t,y,temp+'0'); 
	}
} 
/**********************************************/
/* o����y1|?����o??��?8*16��??������??��?��?             */
/* ��??��2?��y��ox,y :?e��?��?����	 	              */
/*           num:��????����?o?         	   	  */
/* ����    ��a��ox,y��?��??�̨�a?��240��?320��??��?��      */
/**********************************************/
void LCD_ShowChar(uint16_t x,uint16_t y,uint16_t num)
{
    uint8_t temp;
    uint8_t pos,t;
	      
   	LCD_WR_REG_DATA(0x0020,x);//����??X��?����????
    LCD_WR_REG_DATA(0x0021,y);//����??Y��?����????
 	/*?a����??��???����*/
	LCD_XYRAM(x,y,x+7,y+15); // ����??GRAM��?����
    LCD_WR_REG(0x0022);	  	 //???��RAM??��??�¡�?��?��?D�䨺y?Y��?RAM
 	
	num=num-' ';//��?��???��?o����??��
	for(pos=0;pos<16;pos++)
	{
	    temp=ascii_16[num][pos];
	    for(t=0;t<8;t++)
	    {                 
	        if(temp&0x80)LCD_WR_DATA(POINT_COLOR);
	        else LCD_WR_DATA(BACK_COLOR);    
	        temp<<=1; 
	    }
	}
	/* ???��??��???��???����240*320 */
	LCD_XYRAM(0x0000 ,0x0000 ,0x00EF ,0X013F); // ???��GRAM???��??��?
	return;
}

uint8_t buf[32]; //��?������?��?16*16��??��oo��?��y?Y

/**********************************************/
/* o����y1|?����?�䨮W25X16?D������?��??��??             */
/* ��??��2?��y��ocode:GBK??�̨���???��??��             */
/*           dz_data:��?��?��??��??��?��y����         */
/**********************************************/
void Get_GBK_DZK(uint8_t *code, uint8_t *dz_data)
{
	uint8_t GBKH,GBKL;   // GBK??????��?�̨�??					  
	uint32_t offset; 	   // ��??��??��?��?

	GBKH=*code;
	GBKL=*(code+1);	   // GBKL=*(code+1);
	if(GBKH>0XFE||GBKH<0X81)return;
	GBKH-=0x81;
	GBKL-=0x40;
	offset=((uint32_t)192*GBKH+GBKL)*32;//��?��?��??a?D��?��??��??��?��? 
// 	W25X16_Read(dz_data,offset+0x100,32); 		  
	return;
}
/**********************************************/
/* o����y1|?����o??��?16*16��??��?D??                */
/* ��??��2?��y��ox,y :?e��?��?����	 	              */
/*           *hz��ooo��?               	   	  */
/**********************************************/
void LCD_Show_hz(uint16_t x,uint16_t y,uint8_t *hz)
{
	uint8_t i,j,temp;
	uint8_t dz_data[32];

	Get_GBK_DZK(hz, dz_data);

	LCD_WR_REG_DATA(0x0020,x);//����??X��?����????
    LCD_WR_REG_DATA(0x0021,y);//����??Y��?����????
 	/*?a����??��???����*/
	LCD_XYRAM(x,y,x+15,y+15); // ����??GRAM��?����
    LCD_WR_REG(0x0022);	  	 //???��RAM??��??�¡�?��?��?D�䨺y?Y��?RAM

 	for(i=0;i<32;i++)
	{
		temp=dz_data[i];
	 	for(j=0;j<8;j++)
		{
		 	if(temp&0x80)LCD_WR_DATA(POINT_COLOR);
	        else LCD_WR_DATA(BACK_COLOR);    
	        temp<<=1; 
		}
	}

	/* ???��??��???��???����240*320 */
	LCD_XYRAM(0x0000 ,0x0000 ,0x00EF ,0X013F); // ???��GRAM???��??��?
	
	return;
}

/*********************************************/
/* o����y1|?����o??��?��?��?��?�ꡧ?D??o������??��?        */
/* ��??��2?��y��ox,y: ��?����                       */
/*           *p:��?��?��?                       */
/*********************************************/
void LCD_ShowString(uint16_t x,uint16_t y,uint8_t *p)
{         
    while(*p!='\0')	// ��?1???��D?����?
    {       
        if(*p>0x80)//��?1?��??D??
		{
			if((*p=='\n')||(x>224))  // ????o��??DD
			{
				y=y+19;   //��?��???16 DD???��3
				x=2;      //2��?��??��
			}
		LCD_Show_hz(x, y, p);
			x+=16;
			p+=2;
		}
		else	   //��?1?��?����??
		{
			if((*p=='\n')||(x>224))   // ????o��??DD
			{
				y=y+19;   //��?��???16 DD???��3
				x=2;      //2��?��??��
			}
			LCD_ShowChar(x,y,*p);
			x+=8;
			p++;
		}
    }  
}

  


