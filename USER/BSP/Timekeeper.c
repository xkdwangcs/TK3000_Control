#include "Timekeeper.h"
#include "string.h"

#define TIMName		TIM7
#define TIM_IRQn	TIM7_IRQn
#define TIM_RCC		RCC_APB1Periph_TIM7

TimekeeperStruct _timekeeper;
bool _timerState=false;//��ʱ����״̬

//��ʼ����ʱ��,��ȷ��1ms������
void IniTimekeeper(void)
{
	/*---------------------------------------------------------------------
		system_stm32f4xx.c �ļ��� void SetSysClock(void) ������ʱ�ӵ��������£�
		HCLK = SYSCLK / 1     (AHB1Periph)
		PCLK2 = HCLK / 2      (APB2Periph)
		PCLK1 = HCLK / 4      (APB1Periph)
		��ΪAPB1 prescaler != 1, ���� APB1�ϵ�TIMxCLK = PCLK1 x 2 = SystemCoreClock / 2;=84000000
		��ΪAPB2 prescaler != 1, ���� APB2�ϵ�TIMxCLK = PCLK2 x 2 = SystemCoreClock;=168000000

		APB1 ��ʱ���� TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13,TIM14
		APB2 ��ʱ���� TIM1, TIM8 ,TIM9, TIM10, TIM11
	-----------------------------------------------------------------------*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	RCC_APB1PeriphClockCmd(TIM_RCC,ENABLE);
	
	u32 timeMxCLK = SystemCoreClock / 2; //TIM7������Ƶ��
	u32 usPrescaler = timeMxCLK /2/ 100000 ;	//F407ʱ��84000000/2/100000=420
	u32 usPeriod = 0.001*timeMxCLK/usPrescaler; //��ȷ��1ms����ļ���ֵ,200
	TIM_TimeBaseStructure.TIM_Prescaler = usPrescaler-1; //Ԥ��Ƶ��
	TIM_TimeBaseStructure.TIM_Period = usPeriod-1; //�Զ���װ�ؼĴ���
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //���������ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;		//ʱ�ӷ�Ƶ����
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0;		//ÿ������������¼�����
	TIM_TimeBaseInit(TIMName, &TIM_TimeBaseStructure);		 //дTIM7���Ĵ�������
	TIM_ClearFlag(TIMName, TIM_FLAG_Update);		  //�жϱ�־λ����
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
		if(_timekeeper.TimerValue>4294967290)//��ֹ�����һ�㲻������ô��ġ�4294967296-5
			_timekeeper.TimerValue=0;
		_timekeeper.TimerValue++;
		
		for(u8 i=0;i<SoftTimerCount;i++)
		{
			if(!_timekeeper.SoftTimers[i].IsUse) //���ûʹ��
				continue;
			if(!_timekeeper.SoftTimers[i].IsEnable) //���û�п�ʼ����
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

//��ȡ��ʱ����ǰ�ļ�ʱ������λms
u32 GetTimekeeperValue(void)
{
	return _timekeeper.TimerValue;
}

//��ʼ��ʱ��
void StartTimekeeper(void)
{
	if(!_timerState)
	{
		_timekeeper.TimerValue=0;
		_timerState=true;
		TIM_Cmd(TIMName,ENABLE);
	}
}

//��λ��ʱ��
void ResetTimekeeper(void)
{
	_timekeeper.TimerValue=0;
}

//ֹͣ��ʱ��
void StopTimekeeper(void)
{
	TIM_Cmd(TIMName,DISABLE);
	_timerState=false;
}

//����һ����ʱ������������ɹ��򷵻�0��ʼ�����������򷵻�0xFF
//timerType:��ʱ�����ͣ�perValue:��ʱ��Ԥװֵ������ʱ���ۼƵ���ֵ���������ص���callFunc���ص�����
u8 CreateSoftTimer(SoftTimerType timerType,u32 perValue,OneIntParameterHandler callFunc)
{
	for(u8 timerIndex=0;timerIndex<SoftTimerCount;timerIndex++)
	{
		if(_timekeeper.SoftTimers[timerIndex].IsUse)//�����ʱ������ʹ��
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

//ɾ��һ����ʱ�����ͷŶ�ʱ��
void DeleteSoftTimer(u8 timerIndex)
{
	if(timerIndex<0 || timerIndex>=SoftTimerCount)
		return;
	StopSoftTimer(timerIndex);
	_timekeeper.SoftTimers[timerIndex].IsUse=false;
}

//��ʼһ����ʱ��
void StartSoftTimer(u8 timerIndex)
{
	if(timerIndex<0 || timerIndex>=SoftTimerCount)
		return;
	if(!_timerState)//�����ʱ����������
		StartTimekeeper();
	if(_timekeeper.SoftTimers[timerIndex].IsUse && !_timekeeper.SoftTimers[timerIndex].IsEnable)
		_timekeeper.SoftTimers[timerIndex].IsEnable=true;
}

//��λһ����ʱ��
void ResetSoftTimer(u8 timerIndex)
{
	if(timerIndex<0 || timerIndex>=SoftTimerCount)
		return;
	_timekeeper.SoftTimers[timerIndex].Count=0;
}

//ֹͣ��ʱ��
void StopSoftTimer(u8 timerIndex)
{
	if(timerIndex<0 || timerIndex>=SoftTimerCount)
		return;
	_timekeeper.SoftTimers[timerIndex].IsEnable=false;
}

