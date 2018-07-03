#include "timer.h"
#include "gpio.h"

void Timer_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM4 |RCC_APB1Periph_TIM5 |RCC_APB1Periph_TIM6,ENABLE); //��ʱ��2��3��4��6ʱ��ʹ��
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_TIM1 |RCC_APB2Periph_TIM8,ENABLE); 
//------------------
	//TIM2_Configuration M1 X��		TIM2_CH1
	TIM_TimeBaseStructure.TIM_Period = 9;     									//�Զ���װ�ؼĴ���   Ĭ��ֵ600K
	TIM_TimeBaseStructure.TIM_Prescaler = MotorPortList[M1].Prescaler-1;		//SystemCoreClock/2/T_FREQ-1;  //Ԥ��Ƶ��,F=168M/2/6000000-1=13
	TIM_TimeBaseStructure.TIM_CounterMode =TIM_CounterMode_Up;// TIM_CounterMode_Down;//TIM_CounterMode_Up; 	//���������ϼ���ģʽ,S����Ҫ�����¼���������Ῠ��CNT��ʱ����
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;      						//ʱ�ӷ�Ƶ����
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0;      					//ÿ������������¼�����
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);       						//дTIM2���Ĵ�������
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);          								//�жϱ�־λ����
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);       							//������/�Ƚ�3�ж�

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;   						//PWMģʽ2 TIM2_CCMR1[14:12]=111 �����ϼ���ʱ��һ��TIMx_CNT
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   			//����/����2�������
	TIM_OCInitStructure.TIM_Pulse = (TIM_TimeBaseStructure.TIM_Period + 1) / 2; //ȷ��ռ�ձȣ����ֵ��������Ч��ƽ��ʱ�䡣
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;      				//�������,�͵�ƽ��Ч
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);         							//���ö�ʱ�����ģʽ,�Ƚϲ�����
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);       					//����ʹ��TIM2��CCR2�ϵ�Ԥװ�ؼĴ���
	TIM_Cmd(TIM2, DISABLE);
	
//------------------
	//TIM5_Configuration M2 Y1��		TIM5_CH3
	TIM_TimeBaseStructure.TIM_Period = 9;     									//�Զ���װ�ؼĴ���   Ĭ��ֵ600K
	TIM_TimeBaseStructure.TIM_Prescaler = MotorPortList[M1].Prescaler-1;		//SystemCoreClock/2/T_FREQ-1;  //Ԥ��Ƶ��,F=168M/2/6000000-1=13
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM_CounterMode_Down;//TIM_CounterMode_Up; 				//���������ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;      						//ʱ�ӷ�Ƶ����
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0;      					//ÿ������������¼�����
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);       						//дTIM5���Ĵ�������
	TIM_ClearFlag(TIM5, TIM_FLAG_Update);          								//�жϱ�־λ����
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);       							//������/�Ƚ�3�ж�

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;   						//PWMģʽ2 TIM5_CCMR1[14:12]=111 �����ϼ���ʱ��һ��TIMx_CNT
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   			//����/����2�������
	TIM_OCInitStructure.TIM_Pulse = (TIM_TimeBaseStructure.TIM_Period + 1) / 2; //ȷ��ռ�ձȣ����ֵ��������Ч��ƽ��ʱ�䡣
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;      				//�������,�͵�ƽ��Ч
	TIM_OC3Init(TIM5, &TIM_OCInitStructure);         							//���ö�ʱ�����ģʽ,�Ƚϲ�����
	TIM_OC3PreloadConfig(TIM5, TIM_OCPreload_Disable);
	TIM_Cmd(TIM5, DISABLE);

//------------------
	//TIM3_Configuration M3 Z��  TIM3_CH3
	TIM_TimeBaseStructure.TIM_Period = 9;     									//�Զ���װ�ؼĴ���   Ĭ��ֵ600K
	TIM_TimeBaseStructure.TIM_Prescaler = MotorPortList[M1].Prescaler-1;		//SystemCoreClock/2/T_FREQ-1;       //Ԥ��Ƶ��,F=168M/2/6000000-1=13
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down; 				//���������ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;      						//ʱ�ӷ�Ƶ����
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0;      					//ÿ������������¼�����
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);       						//дTIM3���Ĵ�������
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);          								//�жϱ�־λ����
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);       							//������/�Ƚ�3�ж�

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;   						//PWMģʽ2 TIM3_CCMR1[14:12]=111 �����ϼ���ʱ��һ��TIMx_CNT
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   			//����/����2�������
	TIM_OCInitStructure.TIM_Pulse = (TIM_TimeBaseStructure.TIM_Period + 1) / 2; //ȷ��ռ�ձȣ����ֵ��������Ч��ƽ��ʱ�䡣
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;      				//�������,�͵�ƽ��Ч
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);         							//���ö�ʱ�����ģʽ,�Ƚϲ�����
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Disable);
	TIM_Cmd(TIM3, DISABLE);

//------------------
	//TIM4_Configuration M4 Y2��  TIM4_CH1
	TIM_TimeBaseStructure.TIM_Period = 9;     									//�Զ���װ�ؼĴ���   Ĭ��ֵ600K
	TIM_TimeBaseStructure.TIM_Prescaler = MotorPortList[M1].Prescaler-1;		//SystemCoreClock/2/T_FREQ-1;       //Ԥ��Ƶ��,F=168M/2/6000000-1=13
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 				//���������ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;      						//ʱ�ӷ�Ƶ����
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0;      					//ÿ������������¼�����
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);       						//дTIM4���Ĵ�������
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);          								//�жϱ�־λ����
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);       							//������/�Ƚ�3�ж�

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;   						//PWMģʽ2 TIM4_CCMR1[14:12]=111 �����ϼ���ʱ��һ��TIMx_CNT
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   			//����/����2�������
	TIM_OCInitStructure.TIM_Pulse = (TIM_TimeBaseStructure.TIM_Period + 1) / 2; //ȷ��ռ�ձȣ����ֵ��������Ч��ƽ��ʱ�䡣
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;      				//�������,�͵�ƽ��Ч
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);         							//���ö�ʱ�����ģʽ,�Ƚϲ�����
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Disable);
	TIM_Cmd(TIM4, DISABLE);
	
//------------------
	//TIM1_Configuration M5 A��	TIM1_CH1
	TIM_TimeBaseStructure.TIM_Period = 9;     									//�Զ���װ�ؼĴ���  Ĭ��ֵ600K
	TIM_TimeBaseStructure.TIM_Prescaler = MotorPortList[M1].Prescaler-1;		//SystemCoreClock/T_FREQ-1;      //Ԥ��Ƶ��,F=168M/4/6000000-1=6
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 				//���������ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;      						//ʱ�ӷ�Ƶ����
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0;      					//ÿ������������¼�����
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);       						//дTIM1���Ĵ�������
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);          								//�жϱ�־λ����
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);       							//������/�Ƚ�3�ж�

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;   						//PWMģʽ2 TIM8_CCMR1[14:12]=111 �����ϼ���ʱ��һ��TIMx_CNT
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   			//����/����2�������
	TIM_OCInitStructure.TIM_Pulse = (TIM_TimeBaseStructure.TIM_Period + 1) / 2; //ȷ��ռ�ձȣ����ֵ��������Ч��ƽ��ʱ�䡣
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;      				//�������,�͵�ƽ��Ч
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);         							//���ö�ʱ�����ģʽ,�Ƚϲ�����
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Disable);
	TIM_Cmd(TIM1, DISABLE);
	TIM_CtrlPWMOutputs(TIM1,ENABLE);
	
//------------------
	//TIM8_Configuration M6 W��  TIM8_CH2
	TIM_TimeBaseStructure.TIM_Period = 9;     									//�Զ���װ�ؼĴ���  Ĭ��ֵ600K
	TIM_TimeBaseStructure.TIM_Prescaler = MotorPortList[M1].Prescaler-1;		//SystemCoreClock/T_FREQ-1;      //Ԥ��Ƶ��,F=168M/4/6000000-1=6
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 				//���������ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;      						//ʱ�ӷ�Ƶ����
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0;      					//ÿ������������¼�����
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);       						//дTIM8���Ĵ�������
	TIM_ClearFlag(TIM8, TIM_FLAG_Update);          								//�жϱ�־λ����
	TIM_ITConfig(TIM8, TIM_IT_Update, ENABLE);       							//������/�Ƚ�3�ж�

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;   						//PWMģʽ2 TIM8_CCMR1[14:12]=111 �����ϼ���ʱ��һ��TIMx_CNT
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   			//����/����2�������
	TIM_OCInitStructure.TIM_Pulse = (TIM_TimeBaseStructure.TIM_Period + 1) / 2; //ȷ��ռ�ձȣ����ֵ��������Ч��ƽ��ʱ�䡣
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;      				//�������,�͵�ƽ��Ч
	TIM_OC2Init(TIM8, &TIM_OCInitStructure);         							//���ö�ʱ�����ģʽ,�Ƚϲ�����
	TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Disable);
	TIM_Cmd(TIM8, DISABLE);
	TIM_CtrlPWMOutputs(TIM8,ENABLE);

	//TIM6_Configuration
//  u32 timeMxCLK = SystemCoreClock / 2; //TIM6������Ƶ��
//	u32 usPrescaler = timeMxCLK /2/ 100000 ;	//F407ʱ��84000000/2/100000=420
//	u32 usPeriod = 0.001*timeMxCLK/usPrescaler; //��ȷ��1ms����ļ���ֵ,200
//	TIM_TimeBaseStructure.TIM_Prescaler = usPrescaler-1; //Ԥ��Ƶ��
//	TIM_TimeBaseStructure.TIM_Period = usPeriod-1; //�Զ���װ�ؼĴ���
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //���������ϼ���ģʽ
//	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;      		//ʱ�ӷ�Ƶ����
//	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0;      	//ÿ������������¼�����
//	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);       		//дTIM6���Ĵ�������
//	TIM_ClearFlag(TIM6, TIM_FLAG_Update);          				//�жϱ�־λ����
//	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);       			//������/�Ƚ�3�ж�
//	TIM_Cmd(TIM6, DISABLE);
}

//PWM��ʱ������/ֹͣ���á�motorID������ţ�setState������/ֹͣ
void PWM_TIMControl(MotorIDEnum motorID, FunctionalState setState)
{
	switch (motorID)
	{
		case M1:
			//XYQ,2018.0324,Ҫ�Ӵ˾䣬���������ʱ����־�������ж��е���PWM����Ҫ�����ж�֮��
			//����DISABLE�󻹻ᴥ��һ�Σ�ÿ�λ��һ������
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
			 TIM2->CCR1 = period>>1;//ռ�ձ�
			 TIM2->ARR = period-1;//Ԥװ��
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
