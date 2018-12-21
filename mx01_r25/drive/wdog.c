#include "wdog.h"

void WWDG_NVIC_Config(void)
{
   NVIC_InitTypeDef NVIC_InitStructure;

  
  RCC_APB2PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);

  
  NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void WdogInit(void)
{

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
  /* Set IWDG Prescaler value to 8 */
  WWDG_SetPrescaler(WWDG_Prescaler_8);

  WWDG_SetWindowValue(0x7f);
  

  WWDG_Enable(0x7f);
    
    // �����ǰ�����жϱ�־λ
  WWDG_ClearFlag();    
    // ����WWDG�ж����ȼ�
  WWDG_NVIC_Config();    
    // ��WWDG �ж�
  WWDG_EnableIT();

}

void WdogFeed(void)
{
	WWDG_SetCounter( 0x7f );
}
