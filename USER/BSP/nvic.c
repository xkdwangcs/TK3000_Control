#include "nvic.h"

//无抢占式优先级，定时器响应优先级更高，执行时响应优先级更高的中断并不会中断响应优先级低的中断
void NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3); //中断优先级分组只能初始化一次 4位设置为响应优先级，无抢占式  

	//X
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn; //指定中断源
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //抢占先优先级设定
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        //副优先级设定
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//Y
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn; //指定中断源
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //占先优先级设定
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        //副优先级设定
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//Z
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn; //指定中断源
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //占先优先级设定
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        //副优先级设定
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//Y2
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn; //指定中断源
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //占先优先级设定
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        //副优先级设定
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//急停
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn; //指定中断源
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //占先优先级设定
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        //副优先级设定
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;//停止信号导致该中断需最高优先级
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	

	//串口1中断优先级配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	//使能串口1的中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//DMA发送中断设置，串口1 TX
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream7_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
	NVIC_Init(&NVIC_InitStructure);  

	//串口2中断优先级配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;	//使能串口2的中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//DMA发送中断设置，串口2 TX
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream6_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =2;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
	NVIC_Init(&NVIC_InitStructure);  

    //串口3中断优先级配置
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =2; //3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

    //DMA发送中断设置，串口3 TX
	//发送优先级要高于接收中断否则触摸会异常
	//优先级低就不能在按急按钮时弹出急停界面
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream3_IRQn;//串口3 TX
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =1; //2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;  //选择定时器TIM2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;          //选择抢先式优先级（与中断嵌套级别有关）
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;       //选择子优先级（同抢先式优先级的响应顺序）
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    //选择使能中断源
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //选择定时器TIM2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;          //选择抢先式优先级（与中断嵌套级别有关）
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;       //选择子优先级（同抢先式优先级的响应顺序）
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    //选择使能中断源
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //选择定时器TIM2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;          //选择抢先式优先级（与中断嵌套级别有关）
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;       //选择子优先级（同抢先式优先级的响应顺序）
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    //选择使能中断源
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //选择定时器TIM2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;          //选择抢先式优先级（与中断嵌套级别有关）
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;       //选择子优先级（同抢先式优先级的响应顺序）
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    //选择使能中断源
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;  //选择定时器TIM2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;          //选择抢先式优先级（与中断嵌套级别有关）
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;       //选择子优先级（同抢先式优先级的响应顺序）
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    //选择使能中断源
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;  //选择定时器TIM2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;          //选择抢先式优先级（与中断嵌套级别有关）
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;       //选择子优先级（同抢先式优先级的响应顺序）
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    //选择使能中断源
	NVIC_Init(&NVIC_InitStructure);

//	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;  //选择定时器TIM2
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;          //选择抢先式优先级（与中断嵌套级别有关）
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;       //选择子优先级（同抢先式优先级的响应顺序）
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    //选择使能中断源
//	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_TIM13_IRQn;  //选择定时器TIM2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;          //选择抢先式优先级（与中断嵌套级别有关）
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;       //选择子优先级（同抢先式优先级的响应顺序）
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    //选择使能中断源
	NVIC_Init(&NVIC_InitStructure);
	
}
