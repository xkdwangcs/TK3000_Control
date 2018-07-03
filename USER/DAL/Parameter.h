#ifndef PARAMETER_H_
#define PARAMETER_H_

#ifdef __cplusplus
extern "C" {
#endif 

#include "stm32f4xx.h"
#include "stdbool.h"
#include "SST25V.h"
#include "AxisControl.h"
#include "SysParameter.h"
#include "AxisLinkage.h"
#include "PublicFunction.h"
#include "Handlers.h"
#include "Protocol.h"
	
extern Dictionary _yesNoDic[2];

typedef enum
{
	NoLogined=0,    //未登陆，未输入任何权限类型的密码
	UserPer,        //用户权限	
	AdminiPer,      //管理员权限
	DevicePer,		//设备管理员权限
	SuperPer,       //超级权限
}PermissionTypeEnum;//权限等级类型，目前只用密码来区分权限等级，未启用户名

	//螺丝参数结构
	typedef struct
	{
		float length;			//镙丝长度
		float TimeDLimit;		//单位：s。打螺丝的时间下限，用于判断浮高
		float TimeULimit;		//单位：s。打螺丝的时间上限，用于判断滑牙
	} ScrewParaStruct;

	//取料参数
	typedef struct
	{
		CoordStruct Coord;				//取料坐标
		float heigth;					//取料高度
		float getScrewSpeed;			//取料速度
		float getDelayTime;				//真空延时、取料延时
	} GetScrewParaStruct;

	//故障处理方式
	typedef enum
	{
		IGNORE = 0,  //忽略模式
		WAIT  		//等待模式
	} FaultModeEnum;

	//报警灯模式
	typedef enum
	{
		Keep = 0,  //常亮模式
		Twinkle  	//闪烁模式
	} AlarmLampModeEnum;

	typedef enum
	{
		QFQQ=0,		//前放前取
		QFHQ,		//前放后取
	}GJQFModeEnum; //工件取放模式

	typedef enum
	{
		DoublePlatform,//双平台
		SinglePlatform,//单平台  
		CustomMovePlat,//自定义动作的平台，不打螺丝
	}ProdTypeEnum;//产品类型	
	
	typedef enum
	{
		MotorDown,	//电机下压锁螺丝
		QGDown,		//汽缸下压锁螺丝
		THDown,		//弹簧下压锁螺丝
	}LockScrewTypeEnum;//锁螺丝模式枚举
	
	typedef enum
	{
		SuckScrew,	//吸螺丝
		BlowScrew,	//吹螺丝
		OutScrew,	//自动出螺丝
	}GetScrewTypeEnum;//取螺丝模式枚举

	typedef struct
	{
		/********客户铭牌信息**********/
		char ProdName[32];                          //产品名称
		char ProdModel[16];                         //产品型号
        char ProdID[16];		                    //产品序列号        
		char ProduDate[16];                   		//生产日期	
		char Tel[16];								//联系电话
		char URL[32];								//公司网址
		char Company[32];							//生产单位
		char CompanyAddr[80];                       //生产单位地址
		/***********************************/		
		
		/**************设备参数*************/
		u8 DeviceAddr;								//本机地址
		u32 Baud;									//设备波特率   
		CommChannelEnum	CommChannel;				//串口通信通道
		float PulseK[MotorPortNum];					//脉冲当量
		float AxisLength[MotorPortNum];				//行程长度							
		float RunSpeed[MotorPortNum];				//匀速运行速度 mm/s
		float AccSpeed[MotorPortNum];				//加速度 mm/s
		float DecSpeed[MotorPortNum];				//减速度 mm/s	
		LimitModeEnum LimitMode[MotorPortNum];		//限位模式
		float OffsetZeroPoint[MotorPortNum];		//偏移零点,相对原点偏移于限位零点的距离		
		float LinkageSafeSmooth;					//联动时的安全平滑度 %
		LockScrewTypeEnum LockScrewType;			//锁螺丝模式
		GetScrewTypeEnum GetScrewType;				//取螺丝模式
		bool BakByte;//IsUseFSGG;					//是否启用浮锁光感,预留此字节，更新时不用重写参数文件
		bool IsShowDebugMsg;						//是否显示调试信息
		/***********************************/		
		
		float WorkHeight;							//工作高度(安全高度)
		float ScrewLockSpeed;						//锁螺丝时的速度 %
		float ResetSpeed;							//复位速度,mm/s
		ScrewParaStruct ScrewPara;					//镙丝参数
		GetScrewParaStruct GetScrewPara;		    //取镙丝相关参数
		u32 AutoResetNum;							//自动复位的次数(N次复位)
		float StartWorkDelay; 					    //使用“放件启动时”的启动延时
		FaultModeEnum FaultMode;		    		//故障处理模式
		bool IsGetScrew;							//是否取料
		bool IsScrewCheck;							//是否真空检测(螺丝吸紧检测)
		bool IsWorkpieceCheck;		        		//工件是否检测
		bool IsFJQD;			    				//放件是否启动
		GJQFModeEnum QFMode;				        //取放工件工作模式
		AlarmLampModeEnum AlarmLampMode;		    //报警灯模式---常亮or闪烁
		float AlarmBuzzerTime;						//蜂鸣器报警时间长短设置，单位秒s
		float GainWorkpieceDelayTiem;				//取件后延时,s
		char AdminiPass[10];						//管理员密码，可修改的
		char UserPass[10];							//用户密码
		float ZKTQLength;							//真空提前开启的距离,单位为mm
		float DPTQLength;							//电批提前开启的距离,单位为mm
		float PTSpeed;								//点动速度百分比 %
		
		float WorkSpeedPercent;                     //工作速度百分比 %
		ProdTypeEnum ProdType;                      //当前系统的类型  
        char WorkFileName[32];                      //当前工作的文件名	
		bool IsFGBJ;								//检测到浮高后是否报警
		bool IsFYBJ;								//检测到滑牙后是否报警
		float Y1MoveCoord;							//Y1动作坐标，动作文件编程用
		float Y2MoveCoord;							//Y2动作坐标，动作文件编程用
		float NLDelay;								//扭力延时，s
		u16 ScrewTimeDown;							//打螺丝所用时间下限,ms
		u16 ScrewTimeUP;							//打螺丝所用时间上限,ms		
		float LockQGDelay;							//锁付气缸打开延时
		float PushDelay;							//推料控制延时
		float BlowDelay;							//吹料控制延时
        char ParaVer[7];							//参数版本，用于自动升级参数扇区或当扇区损坏时重写         
	} ParameterStruct;		//参数结构体

    //当前权限
    extern PermissionTypeEnum _currPermission; 
	//参数实例
	extern ParameterStruct Parameter;
	extern char* _paraFileName;
	extern char* _paraFileNameBK;
	
	//获取当前用户权限
	PermissionTypeEnum GetCurrPermission(void);	
	//当前录入的权限判断。destPer：目标权限
	bool JudgePermission(char* passWord,PermissionTypeEnum destPer);
	//获取指定权限的字符说明
	char* GetPerStr(PermissionTypeEnum per);
	//获取当前权限的字符说明
	char* GetCurrPerStr(void);	
	//保存取螺丝坐标
	void SaveGainScrewCoord(CoordStruct getScrewCoord);
	//获取取料坐标的某一轴坐标
	float GetScrewOneAxisCoord(AxisIDEnum axisID);
	//从初始工作参数
	bool IniParameter(void);
	//把Parameter内存值写入存储器
	void WriteParameter(EventHandlerNoPara popOkDialog);
	//恢复出厂设置
	void WritDefaultPara(void);
    //判断磁盘是否未格式化，如果是，则格式化
    void PDFormatFlash(void);

#ifdef __cplusplus
}
#endif

#endif 
