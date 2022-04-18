#include "bsp_delay.h"

#define CPU_FREQUENCY_MHZ    168		// STM32时钟主频
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
// 	SysTick->LOAD = nus * SYSCLK;	//时间加载
// 	SysTick->VAL = 0x00;	//清空计数器
// 	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;	//开始倒数
// 	do{
// 		temp = SysTick->CTRL;
// 	}while( (temp&0x01) && !(temp&(1<<16)) );	//等待时间到达
// 	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
// 	SysTick->VAL = 0x00;	//清空计时器
// }


