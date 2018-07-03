#ifndef __SYSTICK_H
#define __SYSTICK_H
#ifdef __cplusplus
 extern "C" {
#endif 
#include "stm32f4xx.h"
#include "stdbool.h"	 
//不使用SysTick，直接使用软延时。SysTick要占用中断资源且本系统对延时精度不是很高
//extern vu32 TimingDelay;
//void Init_SysTick(void);
//void Delay_10us(u32 ntimes);
//void Delay_ms_tick(vu32 nTime);


//延时，us
void Delay_us(u16 us);
//延时ms
void Delay_ms(u16 t);
//延时秒
void Delay_s(float time_s);
     
#ifdef __cplusplus
}
#endif
#endif
