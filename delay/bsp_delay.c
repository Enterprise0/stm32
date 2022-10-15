#include "bsp_delay.h"

#define CPU_FREQUENCY_MHZ    168		// STM32ʱ����Ƶ
void delay_us(__IO uint32_t delay)
{
   int last, curr, val;
   int temp;
   while (delay != 0)
   {
       temp = delay > 900 ? 900 : delay;
       last = SysTick->VAL;
       curr = last - CPU_FREQUENCY_MHZ * temp;
       if (curr >= 0)
       {
           do
           {
               val = SysTick->VAL;
           }
           while ((val < last) && (val >= curr));
       }
       else
       {
           curr += CPU_FREQUENCY_MHZ * 1000;
           do
           {
               val = SysTick->VAL;
           }
           while ((val <= last) || (val > curr));
       }
       delay -= temp;
   }
}

//uint32_t SYSCLK = 0;
//void delay_us_init(void)
//{
//	SYSCLK = HAL_RCC_GetSysClockFreq()/1000000;	//MHZ
//}
//#define SYSCLK	HAL_RCC_GetSysClockFreq()/1000000	//MHZ
// #define SYSCLK	72		//MHZ
// void delay_us(uint32_t nus)
// {
// 	uint32_t temp=0;
// 	SysTick->LOAD = nus * SYSCLK;	//ʱ�����
// 	SysTick->VAL = 0x00;	//��ռ�����
// 	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;	//��ʼ����
// 	do{
// 		temp = SysTick->CTRL;
// 	}while( (temp&0x01) && !(temp&(1<<16)) );	//�ȴ�ʱ�䵽��
// 	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;	//�رռ�����
// 	SysTick->VAL = 0x00;	//��ռ�ʱ��
// }


