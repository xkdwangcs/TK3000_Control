#ifndef SYSPARAMETER_H_
#define SYSPARAMETER_H_

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f4xx.h"
#include "stdbool.h"
#include "SST25V.h"
	 
extern char _superPass[10];

typedef enum
{
	NoReg=0,        //未注册，指出厂时的状态
	Unlimited,      //已注册，无限制的
	Limit,          //已注册，有时间限制的	
}RegisterStateEnum; //注册状态枚举

typedef struct
{
	char DevicePass[10];		//设备管理密码,不能放在Parameter中，因为恢复出厂设置时会复位
	char DevNumCalc[20];		//本机设备码，计算出的真实码  2018.07.05增加，方便主控调用
	char RegNum[32];            //注册码密文
    char RegDate[20];           //注册日期，
    char YXDate[20];            //有效日期，由注册码中得出。
    char DevNum[20];            //注册码中的机器码
	RegisterStateEnum RegState; //注册状态
    char ParaVer[7];			//参数版本，用于自动升级参数扇区 
}SysParameterStruct;            //固定参数结构体

//系统参数实例
extern SysParameterStruct SysParameter;
//初始化系统参数
void IniSysParameter(void);
//把系统参数写入存储器
void WriteSysParameter(void);


/********控制器铭牌，客户不能修改的*******/	
typedef struct
{
	char ProdName[32];                          //产品名称
	char ProdModel[16];                         //产品型号
	char SoftVer[32];		                    //软件版本
	char HardWareVer[10];	                    //硬件版本
	char ProdID[16];		                    //控制器的序列号        
	char ProduDate[16];                   		//生产日期	
	char Tel[16];								//联系电话
	char URL[32];								//公司网址
	char Company[32];							//生产单位
	char CompanyAddr[80];                       //生产单位地址	
	bool IsWrited;								//是否已经写入过出厂信息
    char ParaVer[7];							//参数版本，用于自动升级参数扇区 
}ControlMPStruct;            //控制器铭牌信息结构体

//控制器铭牌实例
extern ControlMPStruct ControlMPMsg;
//初始化控制器铭牌
void IniControlMPMsg(void);
//把控制器铭牌入存储器
void WriteControlMPMsg(void);

#ifdef __cplusplus
}
#endif

#endif
