#include "Timekeeper.h"
#include "string.h"

#define TIMName		TIM7
#define TIM_IRQn	TIM7_IRQn
#define TIM_RCC		RCC_APB1Periph_TIM7

TimekeeperStruct _timekeeper;
bool _timerState=false;//定时器的状态

//初始化计时器,精确到1ms的配置
void IniTimekeeper(void)
{
	/*---------------------------------------------------------------------
		system_stm32f4xx.c 文件中 void SetSysClock(void) 函数对时钟的配置如下：
		HCLK = SYSCLK / 1     (AHB1Periph)
		PCLK2 = HCLK / 2      (APB2Periph)
		PCLK1 = HCLK / 4      (APB1Periph)
		因为APB1 prescaler != 1, 所以 APB1上的TIMxCLK = PCLK1 x 2 = SystemCoreClock / 2;=84000000
		因为APB2 prescaler != 1, 所以 APB2上的TIMxCLK = PCLK2 x 2 = SystemCoreClock;=168000000

		APB1 定时器有 TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13,TIM14
		APB2 定时器有 TIM1, TIM8 ,TIM9, TIM10, TIM11
	-----------------------------------------------------------------------*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	RCC_APB1PeriphClockCmd(TIM_RCC,ENABLE);
	
	u32 timeMxCLK = SystemCoreClock / 2; //TIM7的总线频率
	u32 usPrescaler = timeMxCLK /2/ 100000 ;	//F407时，84000000/2/100000=420
	u32 usPeriod = 0.001*timeMxCLK/usPrescaler; //精确到1ms所需的计数值,200
	TIM_TimeBaseStructure.TIM_Prescaler = usPrescaler-1; //预分频器
	TIM_TimeBaseStructure.TIM_Period = usPeriod-1; //自动重装载寄存器
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //计数器向上计数模式
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;		//时钟分频因子
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0;		//每次溢出都产生事件更新
	TIM_TimeBaseInit(TIMName, &TIM_TimeBaseStructure);		 //写TIM7各寄存器参数
	TIM_ClearFlag(TIMName, TIM_FLAG_Update);		  //中断标志位清零
	TIM_ITConfig(TIMName, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIMName, DISABLE);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;//4
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void TIM7_IRQHandler(void)
{	
	if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)
	{	
		if(_timekeeper.TimerValue>4294967290)//防止溢出，一般不会有这么大的。4294967296-5
			_timekeeper.TimerValue=0;
		_timekeeper.TimerValue++;
		
		for(u8 i=0;i<SoftTimerCount;i++)
		{
			if(!_timekeeper.SoftTimers[i].IsUse) //如果没使用
				continue;
			if(!_timekeeper.SoftTimers[i].IsEnable) //如果没有开始运行
				continue;
			_timekeeper.SoftTimers[i].Count++;
			if(_timekeeper.SoftTimers[i].Count==_timekeeper.SoftTimers[i].PreValue)
			{
				if(_timekeeper.SoftTimers[i].TimerType==SingTimer)
				{
					_timekeeper.SoftTimers[i].IsEnable=false;
				}
				_timekeeper.SoftTimers[i].Count=0;
				if(_timekeeper.SoftTimers[i].CallFunc!=NULL)
				{
					_timekeeper.SoftTimers[i].CallFunc(i);
				}
			}
		}
		TIM_ClearFlag(TIM7, TIM_FLAG_Update);
	}
}

//获取计时器当前的计时数，单位ms
u32 GetTimekeeperValue(void)
{
	return _timekeeper.TimerValue;
}

//开始计时器
void StartTimekeeper(void)
{
	if(!_timerState)
	{
		_timekeeper.TimerValue=0;
		_timerState=true;
		TIM_Cmd(TIMName,ENABLE);
	}
}

//复位计时器
void ResetTimekeeper(void)
{
	_timekeeper.TimerValue=0;
}

//停止计时器
void StopTimekeeper(void)
{
	TIM_Cmd(TIMName,DISABLE);
	_timerState=false;
}

//创建一个软定时器。如果创建成功则返回0开始的索引，否则返回0xFF
//timerType:定时器类型，perValue:定时器预装值，当计时器累计到此值，将引发回调。callFunc：回调函数
u8 CreateSoftTimer(SoftTimerType timerType,u32 perValue,OneIntParameterHandler callFunc)
{
	for(u8 timerIndex=0;timerIndex<SoftTimerCount;timerIndex++)
	{
		if(_timekeeper.SoftTimers[timerIndex].IsUse)//如果定时器正在使用
			continue;
		_timekeeper.SoftTimers[timerIndex].TimerType=timerType;
		_timekeeper.SoftTimers[timerIndex].Count=0;
		_timekeeper.SoftTimers[timerIndex].PreValue=perValue;
		_timekeeper.SoftTimers[timerIndex].IsUse=true;
		_timekeeper.SoftTimers[timerIndex].IsEnable=false;
		_timekeeper.SoftTimers[timerIndex].CallFunc=callFunc;
		return timerIndex;
	}
	return 0xFF;
}

//删除一个定时器，释放定时器
void DeleteSoftTimer(u8 timerIndex)
{
	if(timerIndex<0 || timerIndex>=SoftTimerCount)
		return;
	StopSoftTimer(timerIndex);
	_timekeeper.SoftTimers[timerIndex].IsUse=false;
}

//开始一个定时器
void StartSoftTimer(u8 timerIndex)
{
	if(timerIndex<0 || timerIndex>=SoftTimerCount)
		return;
	if(!_timerState)//如果定时器正在运行
		StartTimekeeper();
	if(_timekeeper.SoftTimers[timerIndex].IsUse && !_timekeeper.SoftTimers[timerIndex].IsEnable)
		_timekeeper.SoftTimers[timerIndex].IsEnable=true;
}

//复位一个定时器
void ResetSoftTimer(u8 timerIndex)
{
	if(timerIndex<0 || timerIndex>=SoftTimerCount)
		return;
	_timekeeper.SoftTimers[timerIndex].Count=0;
}

//停止计时器
void StopSoftTimer(u8 timerIndex)
{
	if(timerIndex<0 || timerIndex>=SoftTimerCount)
		return;
	_timekeeper.SoftTimers[timerIndex].IsEnable=false;
}

