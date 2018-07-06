#include "nvic.h"

//����ռʽ���ȼ�����ʱ����Ӧ���ȼ����ߣ�ִ��ʱ��Ӧ���ȼ����ߵ��жϲ������ж���Ӧ���ȼ��͵��ж�
void NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3); //�ж����ȼ�����ֻ�ܳ�ʼ��һ�� 4λ����Ϊ��Ӧ���ȼ�������ռʽ  

	//X
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn; //ָ���ж�Դ
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ�����ȼ��趨
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        //�����ȼ��趨
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//Y
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn; //ָ���ж�Դ
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //ռ�����ȼ��趨
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        //�����ȼ��趨
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//Z
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn; //ָ���ж�Դ
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //ռ�����ȼ��趨
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        //�����ȼ��趨
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//Y2
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn; //ָ���ж�Դ
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //ռ�����ȼ��趨
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        //�����ȼ��趨
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//��ͣ
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn; //ָ���ж�Դ
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //ռ�����ȼ��趨
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        //�����ȼ��趨
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;//ֹͣ�źŵ��¸��ж���������ȼ�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	

	//����1�ж����ȼ�����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	//ʹ�ܴ���1���ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//DMA�����ж����ã�����1 TX
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream7_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
	NVIC_Init(&NVIC_InitStructure);  

	//����2�ж����ȼ�����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;	//ʹ�ܴ���2���ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//DMA�����ж����ã�����2 TX
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream6_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =2;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
	NVIC_Init(&NVIC_InitStructure);  

    //����3�ж����ȼ�����
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =2; //3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

    //DMA�����ж����ã�����3 TX
	//�������ȼ�Ҫ���ڽ����жϷ��������쳣
	//���ȼ��;Ͳ����ڰ�����ťʱ������ͣ����
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream3_IRQn;//����3 TX
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =1; //2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;  //ѡ��ʱ��TIM2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;          //ѡ������ʽ���ȼ������ж�Ƕ�׼����йأ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;       //ѡ�������ȼ���ͬ����ʽ���ȼ�����Ӧ˳��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    //ѡ��ʹ���ж�Դ
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //ѡ��ʱ��TIM2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;          //ѡ������ʽ���ȼ������ж�Ƕ�׼����йأ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;       //ѡ�������ȼ���ͬ����ʽ���ȼ�����Ӧ˳��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    //ѡ��ʹ���ж�Դ
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //ѡ��ʱ��TIM2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;          //ѡ������ʽ���ȼ������ж�Ƕ�׼����йأ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;       //ѡ�������ȼ���ͬ����ʽ���ȼ�����Ӧ˳��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    //ѡ��ʹ���ж�Դ
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //ѡ��ʱ��TIM2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;          //ѡ������ʽ���ȼ������ж�Ƕ�׼����йأ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;       //ѡ�������ȼ���ͬ����ʽ���ȼ�����Ӧ˳��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    //ѡ��ʹ���ж�Դ
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;  //ѡ��ʱ��TIM2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;          //ѡ������ʽ���ȼ������ж�Ƕ�׼����йأ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;       //ѡ�������ȼ���ͬ����ʽ���ȼ�����Ӧ˳��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    //ѡ��ʹ���ж�Դ
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;  //ѡ��ʱ��TIM2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;          //ѡ������ʽ���ȼ������ж�Ƕ�׼����йأ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;       //ѡ�������ȼ���ͬ����ʽ���ȼ�����Ӧ˳��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    //ѡ��ʹ���ж�Դ
	NVIC_Init(&NVIC_InitStructure);

//	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;  //ѡ��ʱ��TIM2
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;          //ѡ������ʽ���ȼ������ж�Ƕ�׼����йأ�
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;       //ѡ�������ȼ���ͬ����ʽ���ȼ�����Ӧ˳��
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    //ѡ��ʹ���ж�Դ
//	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_TIM13_IRQn;  //ѡ��ʱ��TIM2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;          //ѡ������ʽ���ȼ������ж�Ƕ�׼����йأ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;       //ѡ�������ȼ���ͬ����ʽ���ȼ�����Ӧ˳��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    //ѡ��ʹ���ж�Դ
	NVIC_Init(&NVIC_InitStructure);
	
}
