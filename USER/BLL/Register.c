#include "Register.h"
#include "CMD.h"

u32 _cpuID=0;//CPUID
const u8 _devNumLeng=8;//机器码长度
u8 _regNumLeng=16; //注册码长度
char _devNum[_devNumLeng];
//char* _keyStr="1a2b3c4d";

//获取CPU的唯一标识码,此标识码把96位移位相加得出
u32 GetCPUID(void)
{
    //F103的ID读取
//	u32 id1=*(vu32*)(0x1FFFF7E8);
//	u32 id2=*(vu32*)(0x1FFFF7EC);
//	u32 id3=*(vu32*)(0x1FFFF7F0);
    //F207/F40X的读取
    u32 id1=*(vu32*)(0x1FFF7A10);
	u32 id2=*(vu32*)(0x1FFF7A14);
	u32 id3=*(vu32*)(0x1FFF7A18);
	_cpuID=id2+id3-id1;//(id1>>1)+(id2>>2)+(id3>>3); 
	return _cpuID;
}

//计算出机器码
char* CalcDeviceNumber()
{
	memset(_devNum,0,sizeof(_devNum));
    //char tempBuff[16]={0};
    u32 id = GetCPUID();
 	sprintf(_devNum,"%08X",id);
    //Encrypt_XYQ(tempBuff,_devNum); //不要加密
	return _devNum;
}

//去掉注册码中的横线
void RemoveRegNumLine(char* regNum)
{
	u8 regLeng=strlen(regNum);
	if(regLeng>_regNumLeng && regNum[4]=='-')//如果有横线
	{
		for(u8 i=0;i<4;i++)
		{
			regNum[i+4]=regNum[i+5];
		}
		for(u8 i=0;i<4;i++)
		{
			regNum[i+8]=regNum[i+10];
		}
		for(u8 i=0;i<4;i++)
		{
			regNum[i+12]=regNum[i+15];
		}
		memset(regNum+16,0,regLeng-_regNumLeng);
	}
}

//在设备上进行注册
bool RegisterDevice(char* regNum)
{
	RemoveRegNumLine(regNum);
    u8 regLeng=strlen(regNum);
	if(regLeng!=_regNumLeng)
	{
		SetCurrStatus(RegWait,"注册码长度错误!");
		return false;
	}
    char regPlainStr[32]={0};//注册码明文
    //Decipher((u8*)regStr,(u8*)_keyStr,(u8*)regNum);//DES解密
    if(!Decipher_XYQ(regNum,regPlainStr))//XYQ方式解密
    {
		SetCurrStatus(RegWait,"注册码错误!");
		return false; 
    }
	char* devNumCalc=CalcDeviceNumber();//计算出本机器码
	char devNumReg[8]={0};
	CopyBytes1((u8*)devNumReg,0,(u8*)regPlainStr,0,8);
	//char devNumReg[8]={0};
	//Decipher_XYQ(devNumReg1,devNumReg);//不需要双重解密机器码，因为注册机已经是解密机器码
	if(!StrCMP_n(devNumReg,devNumCalc,8))
	{
		SetCurrStatus(RegWait,"机器码不匹配!");
		return false;
	}
    char* yxDate = DateFormat(regPlainStr+8);//注册码中的有效日期
	char* currDate=GetCurrDateStr();
	if(DateTimeCMP(yxDate,currDate)!=Greater)
	{
		SetCurrStatus(RegWait,"注册码已过期!");
		return false;
	}
    memset(SysParameter.RegNum,0,sizeof(SysParameter.RegNum));
    strcpy(SysParameter.RegNum,regNum);
    memset(SysParameter.RegDate,0,sizeof(SysParameter.RegDate));
    strcpy(SysParameter.RegDate,currDate);
    memset(SysParameter.YXDate,0,sizeof(SysParameter.YXDate));
    strcpy(SysParameter.YXDate,yxDate);
    memset(SysParameter.DevNum,0,sizeof(SysParameter.DevNum));
    strcpy(SysParameter.DevNum,devNumCalc);
	if(DateTimeCMP(yxDate,"2999-01-01")==Equal) //无限制
	{
		SysParameter.RegState=Unlimited;
	}
	else
	{
		SysParameter.RegState=Limit;//有时间限制的注册
	}
    WriteSysParameter();
	SetCurrStatus(DevReady,"设备注册完成");
    return true;
}

//检查机器码是否匹配
bool CheckDeviceNum(void)
{
	char* devNumReg=SysParameter.DevNum;//注册码中得到的机器码
    char* devNumCalc=CalcDeviceNumber();//计算出本机器码
    if(!StrCMP_n(devNumReg,devNumCalc,8))
	{
		return false;
	}
	return true;
}

//启动程序时,检查注册码,如果通过检查返回true
bool CheckRegister()//(char* msg)
{
    if(SysParameter.RegState==NoReg)
    {
        //strcpy(msg,"设备未注册!");
        //return false;
		return true; //设备没注册不弹出注册框
    }
	if(!CheckDeviceNum())
	{
		//strcpy(msg,"机器码不匹配!");
		return false;
	}
	if(SysParameter.RegState==Unlimited) //已经有无限制的注册
		return true;
    char* currDate=GetCurrDateStr();
    char* yxDate=SysParameter.YXDate;
    //if(DateTimeCMP(yxDate,currDate)==Less)
	int tdd = CalcDateDiff_String(yxDate,currDate);
	//if(tdd<3)//注册到期前3天提示
	if(tdd<1) //注册码已到期
	{
		//strcpy(msg,"注册码已过期!");
		return false;
	}
    return true;
}