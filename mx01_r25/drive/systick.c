#include "systick.h"


static __IO uint32_t Tickcnt=0;

void Delay25MS(__IO uint32_t nTime)
{
  uint32_t TimingDelay = Tickcnt+nTime;
  while(TimingDelay >= Tickcnt);
}

uint32_t GetTick(void)
{
	return Tickcnt;
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
	Tickcnt+=1;
	WWDG_SetCounter(0x7F);
}



void Systick_Init(void)
{
  if (SysTick_Config(SystemCoreClock / 40))//25∫¡√Î
  {
    /* Capture error */ 
    while (1);
  }
}
