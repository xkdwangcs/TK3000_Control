#include "Register.h"
#include "CMD.h"

u32 _cpuID=0;//CPUID
const u8 _devNumLeng=8;//�����볤��
u8 _regNumLeng=16; //ע���볤��
char _devNum[_devNumLeng];
//char* _keyStr="1a2b3c4d";

//��ȡCPU��Ψһ��ʶ��,�˱�ʶ���96λ��λ��ӵó�
u32 GetCPUID(void)
{
    //F103��ID��ȡ
//	u32 id1=*(vu32*)(0x1FFFF7E8);
//	u32 id2=*(vu32*)(0x1FFFF7EC);
//	u32 id3=*(vu32*)(0x1FFFF7F0);
    //F207/F40X�Ķ�ȡ
    u32 id1=*(vu32*)(0x1FFF7A10);
	u32 id2=*(vu32*)(0x1FFF7A14);
	u32 id3=*(vu32*)(0x1FFF7A18);
	_cpuID=id2+id3-id1;//(id1>>1)+(id2>>2)+(id3>>3); 
	return _cpuID;
}

//�����������
char* CalcDeviceNumber()
{
	memset(_devNum,0,sizeof(_devNum));
    //char tempBuff[16]={0};
    u32 id = GetCPUID();
 	sprintf(_devNum,"%08X",id);
    //Encrypt_XYQ(tempBuff,_devNum); //��Ҫ����
	return _devNum;
}

//ȥ��ע�����еĺ���
void RemoveRegNumLine(char* regNum)
{
	u8 regLeng=strlen(regNum);
	if(regLeng>_regNumLeng && regNum[4]=='-')//����к���
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

//���豸�Ͻ���ע��
bool RegisterDevice(char* regNum)
{
	RemoveRegNumLine(regNum);
    u8 regLeng=strlen(regNum);
	if(regLeng!=_regNumLeng)
	{
		SetCurrStatus(RegWait,"ע���볤�ȴ���!");
		return false;
	}
    char regPlainStr[32]={0};//ע��������
    //Decipher((u8*)regStr,(u8*)_keyStr,(u8*)regNum);//DES����
    if(!Decipher_XYQ(regNum,regPlainStr))//XYQ��ʽ����
    {
		SetCurrStatus(RegWait,"ע�������!");
		return false; 
    }
	char* devNumCalc=CalcDeviceNumber();//�������������
	char devNumReg[8]={0};
	CopyBytes1((u8*)devNumReg,0,(u8*)regPlainStr,0,8);
	//char devNumReg[8]={0};
	//Decipher_XYQ(devNumReg1,devNumReg);//����Ҫ˫�ؽ��ܻ����룬��Ϊע����Ѿ��ǽ��ܻ�����
	if(!StrCMP_n(devNumReg,devNumCalc,8))
	{
		SetCurrStatus(RegWait,"�����벻ƥ��!");
		return false;
	}
    char* yxDate = DateFormat(regPlainStr+8);//ע�����е���Ч����
	char* currDate=GetCurrDateStr();
	if(DateTimeCMP(yxDate,currDate)!=Greater)
	{
		SetCurrStatus(RegWait,"ע�����ѹ���!");
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
	if(DateTimeCMP(yxDate,"2999-01-01")==Equal) //������
	{
		SysParameter.RegState=Unlimited;
	}
	else
	{
		SysParameter.RegState=Limit;//��ʱ�����Ƶ�ע��
	}
    WriteSysParameter();
	SetCurrStatus(DevReady,"�豸ע�����");
    return true;
}

//���������Ƿ�ƥ��
bool CheckDeviceNum(void)
{
	char* devNumReg=SysParameter.DevNum;//ע�����еõ��Ļ�����
    char* devNumCalc=CalcDeviceNumber();//�������������
    if(!StrCMP_n(devNumReg,devNumCalc,8))
	{
		return false;
	}
	return true;
}

//��������ʱ,���ע����,���ͨ����鷵��true
bool CheckRegister()//(char* msg)
{
    if(SysParameter.RegState==NoReg)
    {
        //strcpy(msg,"�豸δע��!");
        //return false;
		return true; //�豸ûע�᲻����ע���
    }
	if(!CheckDeviceNum())
	{
		//strcpy(msg,"�����벻ƥ��!");
		return false;
	}
	if(SysParameter.RegState==Unlimited) //�Ѿ��������Ƶ�ע��
		return true;
    char* currDate=GetCurrDateStr();
    char* yxDate=SysParameter.YXDate;
    //if(DateTimeCMP(yxDate,currDate)==Less)
	int tdd = CalcDateDiff_String(yxDate,currDate);
	//if(tdd<3)//ע�ᵽ��ǰ3����ʾ
	if(tdd<1) //ע�����ѵ���
	{
		//strcpy(msg,"ע�����ѹ���!");
		return false;
	}
    return true;
}