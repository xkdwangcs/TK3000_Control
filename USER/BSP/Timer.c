#include "timer.h"
#include "gpio.h"

void Timer_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM4 |RCC_APB1Periph_TIM5 |RCC_APB1Periph_TIM6,ENABLE); //定时器2、3、4、6时钟使能
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_TIM1 |RCC_APB2Periph_TIM8,ENABLE); 
//------------------
	//TIM2_Configuration M1 X轴		TIM2_CH1
	TIM_TimeBaseStructure.TIM_Period = 9;     									//自动重装载寄存器   默认值600K
	TIM_TimeBaseStructure.TIM_Prescaler = MotorPortList[M1].Prescaler-1;		//SystemCoreClock/2/T_FREQ-1;  //预分频器,F=168M/2/6000000-1=13
	TIM_TimeBaseStructure.TIM_CounterMode =TIM_CounterMode_Up;// TIM_CounterMode_Down;//TIM_CounterMode_Up; 	//计数器向上计数模式,S曲线要用向下计数，否则会卡，CNT有时会乱
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;      						//时钟分频因子
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0;      					//每次溢出都产生事件更新
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);       						//写TIM2各寄存器参数
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);          								//中断标志位清零
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);       							//允许捕获/比较3中断

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;   						//PWM模式2 TIM2_CCMR1[14:12]=111 在向上计数时，一旦TIMx_CNT
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   			//输入/捕获2输出允许
	TIM_OCInitStructure.TIM_Pulse = (TIM_TimeBaseStructure.TIM_Period + 1) / 2; //确定占空比，这个值决定了有效电平的时间。
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;      				//输出极性,低电平有效
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);         							//配置定时器输出模式,比较参数等
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);       					//不能使用TIM2在CCR2上的预装载寄存器
	TIM_Cmd(TIM2, DISABLE);
	
//------------------
	//TIM5_Configuration M2 Y1轴		TIM5_CH3
	TIM_TimeBaseStructure.TIM_Period = 9;     									//自动重装载寄存器   默认值600K
	TIM_TimeBaseStructure.TIM_Prescaler = MotorPortList[M1].Prescaler-1;		//SystemCoreClock/2/T_FREQ-1;  //预分频器,F=168M/2/6000000-1=13
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM_CounterMode_Down;//TIM_CounterMode_Up; 				//计数器向上计数模式
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;      						//时钟分频因子
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0;      					//每次溢出都产生事件更新
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);       						//写TIM5各寄存器参数
	TIM_ClearFlag(TIM5, TIM_FLAG_Update);          								//中断标志位清零
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);       							//允许捕获/比较3中断

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;   						//PWM模式2 TIM5_CCMR1[14:12]=111 在向上计数时，一旦TIMx_CNT
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   			//输入/捕获2输出允许
	TIM_OCInitStructure.TIM_Pulse = (TIM_TimeBaseStructure.TIM_Period + 1) / 2; //确定占空比，这个值决定了有效电平的时间。
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;      				//输出极性,低电平有效
	TIM_OC3Init(TIM5, &TIM_OCInitStructure);         							//配置定时器输出模式,比较参数等
	TIM_OC3PreloadConfig(TIM5, TIM_OCPreload_Disable);
	TIM_Cmd(TIM5, DISABLE);

//------------------
	//TIM3_Configuration M3 Z轴  TIM3_CH3
	TIM_TimeBaseStructure.TIM_Period = 9;     									//自动重装载寄存器   默认值600K
	TIM_TimeBaseStructure.TIM_Prescaler = MotorPortList[M1].Prescaler-1;		//SystemCoreClock/2/T_FREQ-1;       //预分频器,F=168M/2/6000000-1=13
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down; 				//计数器向上计数模式
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;      						//时钟分频因子
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0;      					//每次溢出都产生事件更新
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);       						//写TIM3各寄存器参数
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);          								//中断标志位清零
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);       							//允许捕获/比较3中断

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;   						//PWM模式2 TIM3_CCMR1[14:12]=111 在向上计数时，一旦TIMx_CNT
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   			//输入/捕获2输出允许
	TIM_OCInitStructure.TIM_Pulse = (TIM_TimeBaseStructure.TIM_Period + 1) / 2; //确定占空比，这个值决定了有效电平的时间。
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;      				//输出极性,低电平有效
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);         							//配置定时器输出模式,比较参数等
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Disable);
	TIM_Cmd(TIM3, DISABLE);

//------------------
	//TIM4_Configuration M4 Y2轴  TIM4_CH1
	TIM_TimeBaseStructure.TIM_Period = 9;     									//自动重装载寄存器   默认值600K
	TIM_TimeBaseStructure.TIM_Prescaler = MotorPortList[M1].Prescaler-1;		//SystemCoreClock/2/T_FREQ-1;       //预分频器,F=168M/2/6000000-1=13
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 				//计数器向上计数模式
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;      						//时钟分频因子
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0;      					//每次溢出都产生事件更新
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);       						//写TIM4各寄存器参数
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);          								//中断标志位清零
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);       							//允许捕获/比较3中断

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;   						//PWM模式2 TIM4_CCMR1[14:12]=111 在向上计数时，一旦TIMx_CNT
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   			//输入/捕获2输出允许
	TIM_OCInitStructure.TIM_Pulse = (TIM_TimeBaseStructure.TIM_Period + 1) / 2; //确定占空比，这个值决定了有效电平的时间。
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;      				//输出极性,低电平有效
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);         							//配置定时器输出模式,比较参数等
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Disable);
	TIM_Cmd(TIM4, DISABLE);
	
//------------------
	//TIM1_Configuration M5 A轴	TIM1_CH1
	TIM_TimeBaseStructure.TIM_Period = 9;     									//自动重装载寄存器  默认值600K
	TIM_TimeBaseStructure.TIM_Prescaler = MotorPortList[M1].Prescaler-1;		//SystemCoreClock/T_FREQ-1;      //预分频器,F=168M/4/6000000-1=6
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 				//计数器向上计数模式
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;      						//时钟分频因子
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0;      					//每次溢出都产生事件更新
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);       						//写TIM1各寄存器参数
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);          								//中断标志位清零
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);       							//允许捕获/比较3中断

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;   						//PWM模式2 TIM8_CCMR1[14:12]=111 在向上计数时，一旦TIMx_CNT
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   			//输入/捕获2输出允许
	TIM_OCInitStructure.TIM_Pulse = (TIM_TimeBaseStructure.TIM_Period + 1) / 2; //确定占空比，这个值决定了有效电平的时间。
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;      				//输出极性,低电平有效
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);         							//配置定时器输出模式,比较参数等
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Disable);
	TIM_Cmd(TIM1, DISABLE);
	TIM_CtrlPWMOutputs(TIM1,ENABLE);
	
//------------------
	//TIM8_Configuration M6 W轴  TIM8_CH2
	TIM_TimeBaseStructure.TIM_Period = 9;     									//自动重装载寄存器  默认值600K
	TIM_TimeBaseStructure.TIM_Prescaler = MotorPortList[M1].Prescaler-1;		//SystemCoreClock/T_FREQ-1;      //预分频器,F=168M/4/6000000-1=6
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 				//计数器向上计数模式
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;      						//时钟分频因子
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0;      					//每次溢出都产生事件更新
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);       						//写TIM8各寄存器参数
	TIM_ClearFlag(TIM8, TIM_FLAG_Update);          								//中断标志位清零
	TIM_ITConfig(TIM8, TIM_IT_Update, ENABLE);       							//允许捕获/比较3中断

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;   						//PWM模式2 TIM8_CCMR1[14:12]=111 在向上计数时，一旦TIMx_CNT
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   			//输入/捕获2输出允许
	TIM_OCInitStructure.TIM_Pulse = (TIM_TimeBaseStructure.TIM_Period + 1) / 2; //确定占空比，这个值决定了有效电平的时间。
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;      				//输出极性,低电平有效
	TIM_OC2Init(TIM8, &TIM_OCInitStructure);         							//配置定时器输出模式,比较参数等
	TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Disable);
	TIM_Cmd(TIM8, DISABLE);
	TIM_CtrlPWMOutputs(TIM8,ENABLE);

	//TIM6_Configuration
//  u32 timeMxCLK = SystemCoreClock / 2; //TIM6的总线频率
//	u32 usPrescaler = timeMxCLK /2/ 100000 ;	//F407时，84000000/2/100000=420
//	u32 usPeriod = 0.001*timeMxCLK/usPrescaler; //精确到1ms所需的计数值,200
//	TIM_TimeBaseStructure.TIM_Prescaler = usPrescaler-1; //预分频器
//	TIM_TimeBaseStructure.TIM_Period = usPeriod-1; //自动重装载寄存器
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //计数器向上计数模式
//	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;      		//时钟分频因子
//	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0;      	//每次溢出都产生事件更新
//	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);       		//写TIM6各寄存器参数
//	TIM_ClearFlag(TIM6, TIM_FLAG_Update);          				//中断标志位清零
//	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);       			//允许捕获/比较3中断
//	TIM_Cmd(TIM6, DISABLE);
}

//PWM定时器启动/停止设置。motorID：电机号，setState：启动/停止
void PWM_TIMControl(MotorIDEnum motorID, FunctionalState setState)
{
	switch (motorID)
	{
		case M1:
			//XYQ,2018.0324,要加此句，两次清除定时器标志，并且中断中调用PWM函数要放在判断之内
			//否则DISABLE后还会触发一次，每次会多一个脉冲
			if(setState==DISABLE)  
				TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
			TIM_Cmd(TIM2, setState);
			break;
		case M2:
			if(setState==DISABLE)
				TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
			TIM_Cmd(TIM5, setState);
			break;
		case M3:
			if(setState==DISABLE)
				TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
			TIM_Cmd(TIM3, setState);
			break;
		case M4:
			if(setState==DISABLE)
				TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
			TIM_Cmd(TIM4, setState);
			break;
		case M5:
			if(setState==DISABLE)
				TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
			TIM_Cmd(TIM1, setState);
			break;
		case M6:
			if(setState==DISABLE)
				TIM_ClearITPendingBit(TIM8, TIM_IT_Update);
			TIM_Cmd(TIM8, setState);
			break;
	}
}

void TIMPeriodSet(MotorIDEnum id,u32 period)
{
	switch(id)
	{			
		case M1:
			 TIM2->CCR1 = period>>1;//占空比
			 TIM2->ARR = period-1;//预装载
			 break;
		case M2:
			 TIM5->CCR3 = period>>1;
			 TIM5->ARR = period-1;
			 break;
		case M3:
			 TIM3->CCR3 = period>>1;
			 TIM3->ARR = period-1;
				break; 
		case M4:
			 TIM4->CCR1 = period>>1;
			 TIM4->ARR = period-1;
			 break;
		case M5:
			 TIM1->CCR1 = period>>1;
			 TIM1->ARR = period-1;
			 break;
		case M6:
			TIM8->CCR2 = period>>1;
			TIM8->ARR = period-1;
			break; 			
	}
}
