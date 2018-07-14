/*****************
UpdataDate:2016-08-31 22:10 XYQ
******************/

#include "main.h"

bool _isExtSRAMErr=false;	//扩展内存是否错误
bool _isFlashErr=false;		//Flash是否错误

//判断是否为热启动
bool PDHeatBoot(void)
{
    //if(RCC_GetFlagStatus(RCC_FLAG_PORRST)==RESET)   //如果不是软件复位
    if(RCC_GetFlagStatus(RCC_FLAG_PORRST)==SET) //如果是上电复位
    {
        //清除RCC中复位标志
        RCC_ClearFlag();
        memset((u8*)(__IO uint8_t *)BKPSRAM_BASE,0,0x1000);
//        for(u32 i=0;i<0x1000;i++)
//        {
//            //*(__IO uint32_t *) (BKPSRAM_BASE + i*4)=0;  //32位的方式
//            //*(__IO uint8_t *) (OutputStateAddrBase + i)=0;
//        }
        return false;
    }
    RCC_ClearFlag();
    return true;
}

//恢复热启动前的相关状态
void ResetState(bool isHeat)
{
    if(isHeat)//如果是热启动
    {
        //_currPermission=*(__IO uint8_t *) PermissionAddr;//注意：这样读出来是错误的
        _currPermission=*(__IO uint8_t *)(PermissionAddr);//这样是对的
        u8 data[OutPortNum]= {0};
        for (u32 i=0; i< OutPortNum; i++)
        {
            data[i] = *(__IO uint8_t *) (OutputStateAddrBase + i);
        }
        for(u8 i=0; i<OutPortNum; i++)
        {
            //要注意，类似电批、真空等特定的输出口是否也要保持状态！
            //应该也可以的，因为复位前需要关闭的输出端口已在程序中关闭
            SetOutPortState_Index(i,data[i]);
        }
    }
}

//初始化备份寄存器
void BKPreg_Configuration(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
    PWR_BackupAccessCmd(ENABLE);
    PWR_BackupRegulatorCmd(ENABLE);
}

void BSPInit(void)
{   
    BKPreg_Configuration();
    bool isHeat = PDHeatBoot();
    GPIO_Config();
    Timer_Config();
    ResetState(isHeat);
    NVIC_Config();
    Exti_Config();
    InitUSART_COM3();
    //ESP8266_StartServer();
    if(Parameter.CommChannel==RS232)
        InitUSART_COM1();
    //else
    //InitUSART_COM2();//F407使用的是USART6，要修改
    DeviceStatusSRQ(DeviceIniting,"设备初始化中");
    IniRTC();
    IniTimekeeper(); //初始化定时器、计时器
    FLASH_Init();
    InitExtSRAM(); //初始化外部SRAM
    _isExtSRAMErr= !ExtSRAMTest();
    _isFlashErr=!FlashTest();
//	  u8 buff[5]={1,2,3,4,5};
//    SendBytes_Pack(_srq,"TestDat=",buff,sizeof(buff));
}

void btnFormat_Click(u16 keyCode)
{
    ShowMessgeForm_NoButton("正在格式化磁盘，请稍候...");
    FormatDisk(SPI_FLASH);
    WorkData.DevieIniFailNum=0;
    WorkData.DeviceIniBadTag=false;
    WriteWorkData();
    //LCDBeep(500);
    Delay_ms_OS(1000);
    SystemReset();
}

void btnBack_Click(u16 keyCode)
{
    WorkData.DevieIniFailNum=0;
    WorkData.DeviceIniBadTag=false;
    WriteWorkData();
    SystemReset();
}

void DeviceInit(void)
{	
//    if(_isExtSRAMErr)
//    {
//        LCDBeep(2000);
//        ShowMessgeForm_NoButton("扩展内存读写异常，请检测主板硬件！");
//        while(true);
//    }
//    if(_isFlashErr)
//    {
//        LCDBeep(2000);
//        ShowMessgeForm_NoButton("Flash底层读写异常，请检测主板硬件！");
//        while(true);
//    }
    IniSysParameter();
    IniWorkData();
//    if(WorkData.DevieIniFailNum>=3)
//    {
//        WaitLCDPowerOn();
//        ShowMessgeForm("设备初始化失败!文件系统可能已损坏，是否格式化磁盘？",btnFormat_Click,btnBack_Click);
//        //return; //这样返回会出错，进入异常
//        while(true);
//    }
    WorkData.DeviceIniBadTag=true;
    WriteWorkData();
    bool isOK = IniParameter();
    if(!isOK)//如果重复三次参数文件初始化不成功，则要提示格式化磁盘
    {
        WorkData.DevieIniFailNum=3;
        WriteWorkData();
        SystemReset();
    }
    IniCoordFile();
    IniPortCFGFile();
    IniMoveProcFile();
    AxisInit();	//轴初始化
//    WaitLCDPowerOn();
//    ShowWelcomeForm();

    char* currDate=GetCurrDateStr();//当前时间
    char* lastLogDate=WorkData.LastOnPowerDate;//上次开机时间
    if(DateTimeCMP(currDate,lastLogDate)!=Equal)
    {
        //清零日统计数据
        WorkData.DayStatistics.totalNumber = 0;
        WorkData.DayStatistics.qualifiedNumber = 0;
        WorkData.DayStatistics.unQualifiedNumber = 0;
    }
    strcpy(WorkData.LastOnPowerDate, currDate);
    WorkData.DevieIniFailNum=0;
    WorkData.DeviceIniBadTag=false;
    WriteWorkData();

    //如果急停按下，按钮已改为常闭OK，弹开为急停
    //调试时要把急停端口拉低接地
    if(GetInPortState(StopPort))
    {
        Delay_ms_OS(500);
        SetCurrStatus(DevScram,"设备已急停");
        if(GetInPortState(StopPort))
        {
            while(GetInPortState(StopPort));
            //ShowWelcomeForm();
        }
    }
		//PTSpeedTypeEnum _currPTSpeedType=MEDIUM;//当前点动速度类型，默认为"中"
		SetPTRunPara(MEDIUM);
    //PositionResetMove();
//    LCDBeep(300);
//    SendCharsPack(_srq, "COM1_0K!");
//    //注册检查
    if(!CheckRegister())
    {
        //ShowRegisterForm(ShowMainForm);
      SetCurrStatus(RegWait,"注册码录入");
    }
    else
    {
        //ShowMainForm(0);
        SetCurrStatus(DevReady,"设备准备就绪");
    }
}

int main(void)
{
    SetCurrStatus(DeviceIniting,"设备初始化中");
    NVIC_SetVectorTable(FLASH_BASE, 0x40000);
    BSPInit();
    if(xTaskCreate(InitTask, "stat", configMINIMAL_STACK_SIZE, NULL, 3, NULL) !=pdTRUE)
    {
		SetCurrStatus(TaskInitFail,"任务启动失败");
    }
    vTaskStartScheduler();		
    for( ;; );
}

