/******************************************************************/
/* ??3?㏒oTFT?y?‘                                                   */
/* 車2?t芍??車㏒oPB0-PB7---DB0-DB7   米赤8??                             */
/*           PC0-PC7---DB8-DB7   ??8??                             */
/*           PD2---WR  PC12---RD PC11---CS  PC10---RS  PB11---CM     */            
/* ℅¯??㏒o?角﹞?                                                     */
/*                  ㏒“米那?﹞㏒o	qfv5.taobao.com㏒?                   */
/* 芍a?米﹞?那?㏒oQQ:157736309                                         */
/******************************************************************/

#include"stm32f0xx.h"
#include"ili9328.h"
#include "w25x16.h"
#include "ascii.h"

uint16_t  POINT_COLOR=BLACK;
uint16_t  BACK_COLOR=WHITE;
/**************************************************/
/* o‘那y1|?邦㏒??足?Y?車那㊣㏒??aLCD3?那??‘那㊣D辰﹞t??        */
/**************************************************/	

static void delay (int cnt) {

  cnt <<= DELAY_2N;
  while (cnt--);
}
/***************************************************/
/* o‘那y1|?邦㏒???ILI9325米???∩??¯D∩那y?Y               */
/* 豕??迆2?那y㏒?val㏒o16??那y?Y                         */
/* ?米?¯㏒o    D∩16??那y?Y㏒??∫?～～?谷?米?LCD2谷車?8??芍??車㏒?*/
/*           ?迄辰?㏒?16??那y?Y﹞?芍?∩?D∩????∩??¯㏒??豕D∩??*/
/*           ??㏒??迄D∩米赤???㏒                        */
/* ℅⊿辰a㏒o那1車?那㊣D豕辰a?豕那1車?LCD_WR_REG(reg)????辰a～?那y */
/*       ?YD∩米???????∩??¯角????㏒                    */
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
/* o‘那y1|?邦㏒o??ILI9238米?D∩?邦芍?2迄℅¯??車|??∩??¯         */
/* 豕??迆2?那y㏒?D∩?邦芍?                                 */
/* ?米?¯㏒o    ??∩??¯o?那?16??那y㏒?8??芍??車﹞?那?D豕辰aD∩芍?∩?*/
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
/* o‘那y1|?邦㏒w?辰??∩??¯角?D∩那y?Y                        */
/* 豕??迆2?那y㏒?reg㏒o??∩??¯o?  data㏒o那y?Y?米             */
/* ?米?¯㏒o    ??o‘那y那??～芍???o‘那y米?o?3谷㏒?辰2?赤那?谷???芍??? */
/*           o‘那y辰?～?米?車?﹞“?赤那??a?迄℅谷o??㏒             */
/******************************************************/

void LCD_WR_REG_DATA(uint16_t reg, uint16_t data)
{ 
	 
   	LCD_WR_REG(reg);
   	LCD_WR_DATA(data);
 	
	}	
	
	
/****************************************************/
/* o‘那y1|?邦㏒o∩車ILI9238?D?芍那y?Y                      */
/* 豕??迆2?那y㏒?那y?Y                               */
/* ?米?¯㏒o    ??∩??¯o?那?16??那y㏒?8??芍??車﹞?那?D豕辰aD∩芍?∩?*/
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
	               //那?豕?赤那芍?o車∩車D?谷豕???a那?3?
		GPIOB->MODER|=0x00005555; 
		GPIOC->MODER|=0x00005555; 
	Set_nRd;
	return val;
}	

/******************************************************/
/* o‘那y1|?邦㏒w?芍??∩??¯角?米?那y?Y                        */
/* 豕??迆2?那y㏒?reg㏒o??∩??¯o?                           */
/* ?米?¯㏒o    ??o‘那y那??～芍???o‘那y米?o?3谷㏒?辰2?赤那?谷???芍??? */
/*           o‘那y辰?～?米?車?﹞“?赤那??a?迄℅谷o??㏒             */
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
/* o‘那y1|?邦㏒o3?那??‘LCD                                 */
/*******************************************************/
void LCD_init (void) 
{ uint16_t i;
	//////////////////////////////////////////////////////////////////////1邦??3?那??‘
	 RCC->AHBENR&=0xffe3ffff;
	 RCC->AHBENR|=0x001c0000;
	
	GPIOB->MODER&=0xffff0000;		   // 谷豕??PB?迆米赤8??辰y???a那?3?㏒?車?℅¯LCD16??2⊿DD那y?Yo赤????辰y??
	GPIOB->MODER|=0x00005555;  
// 	GPIOB->BSRR=0x00ff;
  GPIOB->ODR &= 0xff00;	
// 	GPIOB->ODR |= 0x00ff;	   // PB?迆米赤8??辰y??????
	
	GPIOC->MODER&=0xffff0000;		   // 谷豕??PC?迆米赤8??辰y???a那?3?㏒?車?℅¯LCD16??2⊿DD那y?Yo赤????辰y??
	GPIOC->MODER|=0x00005555; 
// 	GPIOC->BSRR=0x00ff;
	GPIOC->ODR &= 0xff00;	
// 	GPIOC->ODR |= 0x00ff;	   // PC?迆米赤8??辰y??????
	
	GPIOB->MODER&=0xff3fffff;
	GPIOB->MODER|=0x00400000;  //PB11?a那?3??㏒那?
	GPIOB->BRR =0x00000800;    //那?3??a米赤米???
	
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
* ??    3?㏒ovoid GLCD_Test(void)
* 1|    ?邦㏒o2a那?辰o?∫?芍
* 豕??迆2?那y㏒o?T
* 3??迆2?那y㏒o?T
* ?米    ?¯㏒o??那?2那足?㏒?2a那?辰o?∫?芍那?﹞??y3㏒1∟℅¯
* 米¯車?﹞?﹞“㏒oGLCD_Test();
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
/* o‘那y1|?邦㏒??a????那?                         */
/* ?米    ?¯㏒o辰?～?車?車迆1?㊣???那?o車米??a??         */
/**********************************************/
void LCD_DisplayOn(void)
{
 	LCD_WR_REG_DATA(0x0007, 0x0133); // ?a????那?
}
/**********************************************/
/* o‘那y1|?邦㏒?1?㊣???那?                         */
/* ?米    ?¯㏒o2⊿2?那?那1TFTo迆?芍㏒???那?那1米?TFT谷?米? */
/*           ?迆豕Y㊣㏒3?2?㊣?㏒?辰?～?車?車迆TFT米?那y?Y??*/
/*           o赤???邦?㏒?谷芍??車㏒?2迄℅¯???邦?㏒?谷米?那㊣ */
/*           o辰2??㏒赤?車～?足TFT米???那??㏒          */
/**********************************************/
void LCD_DisplayOff(void)
{
	LCD_WR_REG_DATA(0x0007, 0x0); // 1?㊣???那?
}

/***************************************************/
/* o‘那y1|?邦㏒?谷豕????∩???車辰                          */
/* ?米    ?¯㏒o谷豕????辰a??那?米???∩?XY?e那?o赤?芍那?℅?㊣那    */
/* ℅⊿    辰a㏒o車?赤那辰?o車辰a???∩?e米?(0,0)o赤??米?(239,319)*/
/***************************************************/
void LCD_XYRAM(uint16_t xstart ,uint16_t ystart ,uint16_t xend ,uint16_t yend)
{
	LCD_WR_REG_DATA(0x0050, xstart); // 谷豕??o芍℅?㊣那GRAM?e那?米??﹞
	LCD_WR_REG_DATA(0x0051, xend); // 谷豕??o芍℅?㊣那GRAM?芍那?米??﹞
	LCD_WR_REG_DATA(0x0052, ystart); // 谷豕??℅Y℅?㊣那GRAM?e那?米??﹞
	LCD_WR_REG_DATA(0x0053, yend); // 谷豕??℅Y℅?㊣那GRAM?芍那?米??﹞
}

/*******************************************************/
/* o‘那y1|?邦㏒o谷豕??TFT?芍?e那?℅?㊣那                         */
/*******************************************************/
void LCD_SetC(uint16_t x, uint16_t y)
{
	LCD_WR_REG_DATA(0x0020,x);  //谷豕??X℅?㊣那????
	LCD_WR_REG_DATA(0x0021,y);  //谷豕??Y℅?㊣那????
}
/**********************************************/
/* o‘那y1|?邦㏒????芍TFT                          */
/* 豕??迆2?那y㏒ocolor ??谷??米                     */
/**********************************************/
void LCD_Clear(uint16_t color)
{
    uint32_t temp;
    LCD_WR_REG_DATA(0x0020,0);//谷豕??X℅?㊣那????
    LCD_WR_REG_DATA(0x0021,0);//谷豕??Y℅?㊣那????
    LCD_WR_REG(0x0022);				//???辰RAM??∩??¯㏒?℅?㊣?D∩那y?Y米?RAM
	for(temp=0;temp<76800;temp++)
	{
		LCD_WR_DATA(color);    
	}
}
/******************************************/
/* o‘那y1|?邦㏒o?-辰???????米?米?               */
/* 豕??迆2?那y㏒ox,y   米?米?℅?㊣那               */
/******************************************/
void LCD_DrawPoint(uint16_t x,uint16_t y)
{
	LCD_WR_REG_DATA(0x0020,x);//谷豕??X℅?㊣那????
    LCD_WR_REG_DATA(0x0021,y);//谷豕??Y℅?㊣那????
	LCD_WR_REG(0x0022);       //?a那?D∩豕?GRAM
	LCD_WR_DATA(POINT_COLOR); 
}
/******************************************/
/* o‘那y1|?邦㏒o?-辰???D?那?℅?㊣那               */
/* 豕??迆2?那y㏒ox,y   米?米?℅?㊣那               */
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
/* o‘那y1|?邦㏒o?芍TFT?3辰?米?米???谷?            */
/* 3??迆2?那y㏒ocolor  米???谷??米              */
/******************************************/
uint16_t LCD_ReadPoint(uint16_t x,uint16_t y)
{
	uint16_t  color;

	LCD_WR_REG_DATA(0x0020,x);//谷豕??X℅?㊣那????
  LCD_WR_REG_DATA(0x0021,y);//谷豕??Y℅?㊣那????
	LCD_WR_REG(0x0022);       //?a那?D∩豕?GRAM
	
	GPIOB->MODER&=0xffff0000;		   
  GPIOC->MODER&=0xffff0000;        //～?TFT那y?Y辰y??谷豕???a那?豕?
	
	color = LCD_RD_DATA();	  //?芍3?GRAM?米㏒“℅⊿辰a㏒oGRAM?米㊣?D??芍豕?芍?∩?㏒?
	color = LCD_RD_DATA();	  //?芍3?GRAM?米㏒“?那??ILI932x那y?Y那?2芍㏒?
	
	GPIOB->MODER|=0x00005555; 
	GPIOC->MODER|=0x00005555; 		  //???∩那y?Y辰y???a那?3?

	return color;
} 

/**********************************************/
/* o‘那y1|?邦㏒??-?㊣??                           */
/**********************************************/
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	uint16_t t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 

	delta_x=x2-x1; //????℅?㊣那??芍? 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //谷豕??米ㄓ2?﹞??辰 
	else if(delta_x==0)incx=0;//∩1?㊣?? 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//?????? 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //??豕??迄㊣???芍?℅?㊣那?芍 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//?-??那?3? 
	{  
		LCD_DrawPoint(uRow,uCol);//?-米? 
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
/* o‘那y1|?邦㏒??-??D?                           */
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
/* o‘那y1|?邦㏒???D?足?3?                         */
/**********************************************/
void LCD_Fill(uint16_t xstart ,uint16_t ystart ,uint16_t xend ,uint16_t yend ,uint16_t color)
{                    
    uint32_t max;
	LCD_XYRAM(xstart ,ystart ,xend ,yend); // 谷豕??GRAM℅?㊣那
	LCD_WR_REG_DATA(0x0020,xstart);//谷豕??X℅?㊣那????
    LCD_WR_REG_DATA(0x0021,ystart);//谷豕??Y℅?㊣那????
    LCD_WR_REG(0x0022);				//???辰RAM??∩??¯㏒?℅?㊣?D∩那y?Y米?RAM
	max=(uint32_t)((xend-xstart+1)*(yend-ystart+1));    
	while(max--)
	{
		LCD_WR_DATA(color);
	}
	LCD_XYRAM(0x0000 ,0x0000 ,0x00EF ,0X013F); // ???∩GRAM???芍??那?
}
/*******************************************/
/* o‘那y1|?邦㏒o?-?2                          */
/* 豕??迆2?那y㏒ox0,y0  ?2D?℅?㊣那               */
/*           r      ～???(米ㄓ??㏒o????)       */
/*******************************************/
void LCD_DrawCircle(uint8_t x0, uint16_t y0, uint8_t r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //?D??????米?????米?㊣那??
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
		//那1車?Bresenham??﹞“?-?2     
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
/* o‘那y1|?邦㏒??車m米?n∩?﹞?                       */
/**********************************************/
uint32_t mypow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}			 
/**********************************************/
/* o‘那y1|?邦㏒o??那?那y℅?                         */
/* 豕??迆2?那y㏒ox,y :?e米?℅?㊣那	 	              */
/*           len :那y℅?米???那y				  */
/*           num:那y?米(0~4294967295);	   	  */
/**********************************************/
void LCD_ShowNum(uint8_t x,uint16_t y,uint32_t num,uint8_t len)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;		 // ∩?㊣?芍?車?角∩豕ㄓ米?℅?????米?0	
				   
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
/* o‘那y1|?邦㏒o??那?8*16米??車車⊿??℅?﹞?             */
/* 豕??迆2?那y㏒ox,y :?e米?℅?㊣那	 	              */
/*           num:℅????辰﹞?o?         	   	  */
/* ℅⊿    辰a㏒ox,y米?豕??米辰a?迆240米?320﹞??∫?迆      */
/**********************************************/
void LCD_ShowChar(uint16_t x,uint16_t y,uint16_t num)
{
    uint8_t temp;
    uint8_t pos,t;
	      
   	LCD_WR_REG_DATA(0x0020,x);//谷豕??X℅?㊣那????
    LCD_WR_REG_DATA(0x0021,y);//谷豕??Y℅?㊣那????
 	/*?a㊣迄??∩???車辰*/
	LCD_XYRAM(x,y,x+7,y+15); // 谷豕??GRAM℅?㊣那
    LCD_WR_REG(0x0022);	  	 //???辰RAM??∩??¯㏒?℅?㊣?D∩那y?Y米?RAM
 	
	num=num-' ';//米?米???辰?o車米??米
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
	/* ???∩??∩???那???車辰240*320 */
	LCD_XYRAM(0x0000 ,0x0000 ,0x00EF ,0X013F); // ???∩GRAM???芍??那?
	return;
}

uint8_t buf[32]; //車?車迆∩?﹞?16*16米??車oo℅?那y?Y

/**********************************************/
/* o‘那y1|?邦㏒?∩車W25X16?D足芍豕?米??車??             */
/* 豕??迆2?那y㏒ocode:GBK??米迆辰???℅??迆             */
/*           dz_data:∩?﹞?米??車??米?那y℅谷         */
/**********************************************/
void Get_GBK_DZK(uint8_t *code, uint8_t *dz_data)
{
	uint8_t GBKH,GBKL;   // GBK??????車?米赤??					  
	uint32_t offset; 	   // 米??車??辰?芍?

	GBKH=*code;
	GBKL=*(code+1);	   // GBKL=*(code+1);
	if(GBKH>0XFE||GBKH<0X81)return;
	GBKH-=0x81;
	GBKL-=0x40;
	offset=((uint32_t)192*GBKH+GBKL)*32;//米?米?℅??a?D米?℅??迆??辰?芍? 
// 	W25X16_Read(dz_data,offset+0x100,32); 		  
	return;
}
/**********************************************/
/* o‘那y1|?邦㏒o??那?16*16米??車?D??                */
/* 豕??迆2?那y㏒ox,y :?e米?℅?㊣那	 	              */
/*           *hz㏒ooo℅?               	   	  */
/**********************************************/
void LCD_Show_hz(uint16_t x,uint16_t y,uint8_t *hz)
{
	uint8_t i,j,temp;
	uint8_t dz_data[32];

	Get_GBK_DZK(hz, dz_data);

	LCD_WR_REG_DATA(0x0020,x);//谷豕??X℅?㊣那????
    LCD_WR_REG_DATA(0x0021,y);//谷豕??Y℅?㊣那????
 	/*?a㊣迄??∩???車辰*/
	LCD_XYRAM(x,y,x+15,y+15); // 谷豕??GRAM℅?㊣那
    LCD_WR_REG(0x0022);	  	 //???辰RAM??∩??¯㏒?℅?㊣?D∩那y?Y米?RAM

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

	/* ???∩??∩???那???車辰240*320 */
	LCD_XYRAM(0x0000 ,0x0000 ,0x00EF ,0X013F); // ???∩GRAM???芍??那?
	
	return;
}

/*********************************************/
/* o‘那y1|?邦㏒o??那?℅?﹞?∩?㏒“?D??o赤車⊿??㏒?        */
/* 豕??迆2?那y㏒ox,y: ℅?㊣那                       */
/*           *p:℅?﹞?∩?                       */
/*********************************************/
void LCD_ShowString(uint16_t x,uint16_t y,uint8_t *p)
{         
    while(*p!='\0')	// 豕?1???車D?芍那?
    {       
        if(*p>0x80)//豕?1?那??D??
		{
			if((*p=='\n')||(x>224))  // ????o赤??DD
			{
				y=y+19;   //℅?足???16 DD???角3
				x=2;      //2那?㊣??角
			}
		LCD_Show_hz(x, y, p);
			x+=16;
			p+=2;
		}
		else	   //豕?1?那?車⊿??
		{
			if((*p=='\n')||(x>224))   // ????o赤??DD
			{
				y=y+19;   //℅?足???16 DD???角3
				x=2;      //2那?㊣??角
			}
			LCD_ShowChar(x,y,*p);
			x+=8;
			p++;
		}
    }  
}

  


