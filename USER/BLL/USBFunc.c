#include "USBFunc.h"
#include "FileOperate.h"
#include "main.h"
#include "AppTasks.h"
//#include "MoveFileExImportForm.h"

//TextBox* _txtUSBMsg;
//DicTextBox* _listIsCover; //重名是否覆盖
bool _isCover;  //重名是否覆盖
char* _paraFileNameUSB="1:/CFG/Parameter.cfg";
ParameterStruct ParameterUSB; //USB中的参数文件数据
//与Fat操作相关的任务枚举
USBOperateEnum _usbOperate=NoUSBOP;

//显示当前文件复制信息
void ShowMSG(char* msgStr)
{
	//SetText(_txtUSBMsg,msgStr);
}

char _usbPath[80]={0};
char* GetUSBFullPath(char* path)
{
	memset(_usbPath,0,sizeof(_usbPath));
	sprintf(_usbPath,"%s/%s",path,Parameter.ProdModel);
	return _usbPath;
}

//从设备复制坐标文件到U盘
void CopyDeviceToUSB()
{
    //bool isCover= ConvertAsciiTou32(_listIsCover->currKV->value);	
    bool bl = CopyAllFile_Dir("0:/COORD",GetUSBFullPath("1:/COORD"),_isCover,ShowMSG);
	if(bl) 
	{
		ShowMSG("坐标文件导出成功！");
		//PopForm(2);
	}
	else
	{
		ShowMSG("坐标文件导出失败！");
	}
}

//从U盘复制坐标文件到设备
void CopyUSBToDevice()
{
    bool bl = CopyAllFile_Dir(GetUSBFullPath("1:/COORD"),"0:/COORD",_isCover,ShowMSG);
	if(bl) 
	{
		ShowMSG("坐标文件导入成功！");
		//PopForm(2);
	}
	else
	{
		ShowMSG("坐标文件导入失败！");
	}
} 

//复制日志文件到U盘
void CopyLogFileToUSB()
{
    bool bl = CopyAllFile_Dir("0:/LOG",GetUSBFullPath("1:/LOG"),_isCover,ShowMSG);  
	if(bl) 
	{
		ShowMSG("日志文件导出成功！");
		//PopForm(2);
	}
	else
	{
		ShowMSG("日志文件导出失败！");
	}
}

//固件升级
void FirmwareUpdate(u16 keyCode)
{
	*(__IO uint8_t *) (FirmwareUpdateTagAddr)=1;
	SystemReset();
}

//导出配置文件到U盘，目前主要用于铭牌信息的导入
void ExportParamaterFileToUSB(void)
{
	memset(_usbPath,0,sizeof(_usbPath));
	sprintf(_usbPath,"1:/CFG/%s/Parameter.cfg",Parameter.ProdModel);
	//bool isCover= ConvertAsciiTou32(_listIsCover->currKV->value);
	bool bl= CopyFile(_paraFileName,_paraFileNameUSB,_isCover);//通用的参数用于铭牌信息复制
	bl=bl && CopyFile(_paraFileName,_usbPath,_isCover);//不同型号设备
	if(bl) 
	{
		ShowMSG("参数文件导出成功！");
		//PopForm(2);
	}
	else
	{
		ShowMSG("参数文件导出失败！");
	}
}

void ImportParamaterToDevice(void)
{
	memset(_usbPath,0,sizeof(_usbPath));
	sprintf(_usbPath,"1:/CFG/%s/Parameter.cfg",Parameter.ProdModel);
	bool bl= CopyFile(_usbPath,_paraFileName,true);
	bl=bl && CopyFile(_usbPath,_paraFileNameBK,true);
	if(bl) 
	{
		IniParameter();
		ShowMSG("参数文件导入成功！");
		//PopForm(2);
	}
	else
	{
		ShowMSG("参数文件导入失败！");
	}
}

//导入铭牌信息到设备
void ImportMPtoDev(void)
{
	ReadBytesFormFile(_paraFileNameUSB,(u8*) &ParameterUSB,0,sizeof(ParameterUSB));
	if(StrCMP_n(ParameterUSB.ParaVer,"ver",3))
	{
		strcpy(Parameter.ProdName,ParameterUSB.ProdName);
		strcpy(Parameter.ProdModel,ParameterUSB.ProdModel);
		strcpy(Parameter.Tel,ParameterUSB.Tel);
		strcpy(Parameter.URL,ParameterUSB.URL);
		strcpy(Parameter.Company,ParameterUSB.Company);
		strcpy(Parameter.CompanyAddr,ParameterUSB.CompanyAddr);
		strcpy(Parameter.ProduDate,ParameterUSB.ProduDate);
		strcpy(Parameter.ProdID,ParameterUSB.ProdID);
		//WriteParameter(ShowDialog_OK);
		ShowMSG("铭牌信息导入成功！");
		//PopForm(2);
	}
	else
	{
		ShowMSG("U盘中的参数文件错误！");
	}
}

void PortFileToUSB(void)
{
	memset(_usbPath,0,sizeof(_usbPath));
	sprintf(_usbPath,"1:/CFG/%s/PortMotor.cfg",Parameter.ProdModel);
	bool bl=CopyFile(_portFileName,_usbPath,_isCover);//不同型号设备
	if(bl) 
	{
		ShowMSG("端口文件导出成功！");
		//PopForm(2);
	}
	else
	{
		ShowMSG("端口文件导出失败！");
	}
}

void ImportPortFileToDevice(void)
{
	memset(_usbPath,0,sizeof(_usbPath));
	sprintf(_usbPath,"1:/CFG/%s/PortMotor.cfg",Parameter.ProdModel);
	bool bl= CopyFile(_usbPath,_portFileName,true);
	if(bl) 
	{
		IniPortCFGFile();
		ShowMSG("端口文件导入成功！");
		//PopForm(2);
	}
	else
	{
		ShowMSG("端口文件导入失败！");
	}
}

//USB相关操作的任务
void USBOpereateTask(void)
{
	if(_usbOperate==NoUSBOP)
	{
		return;
	}
	else if(_usbOperate==ExportCoordFile)
	{
		 CopyDeviceToUSB();
	}
	else if(_usbOperate==ImportCoordFile)
	{
		 CopyUSBToDevice();
	}
	else if(_usbOperate==ExportLogFile)
	{
		CopyLogFileToUSB();
	}
	else if(_usbOperate==ExportParaFile)
	{
		ExportParamaterFileToUSB();
	}
	else if(_usbOperate==ImportParaFile)
	{
		ImportParamaterToDevice();
	}
	else if(_usbOperate==ImportMPPara)
	{
		ImportMPtoDev();
	}
	else if(_usbOperate==ExportPortFile)
	{
		PortFileToUSB();
	}
	else if(_usbOperate==ImportPortFile)
	{
		ImportPortFileToDevice();
	}
	_usbOperate=NoUSBOP;
}

//USB操作界面单击按钮事件
void btnOnclick_USBOP(u16 keyCode)
{
	if(keyCode==0x0001)  //复制坐标文件，设备->USB
    {
       _usbOperate=ExportCoordFile;
    }
    else if(keyCode==0x0002)  //复制坐标文件，USB->设备
    {
        _usbOperate=ImportCoordFile;
    }
	else if(keyCode==0x0003)  //参数导出
    {
        _usbOperate=ExportParaFile;
    }
    else if(keyCode==0x0004)  //参数导入
    {
        _usbOperate=ImportParaFile;
    }
	else if(keyCode==0x0005)//日志导出
	{
		_usbOperate=ExportLogFile;
	}
	else if(keyCode==0x0006)//铭牌导入
	{
		_usbOperate=ImportMPPara;
	}
	else if(keyCode==0x000A)//端口文件导出
	{
		_usbOperate=ExportPortFile;
	}
	else if(keyCode==0x000B)//端口文件导入
	{
		_usbOperate=ImportPortFile;
	}
	StartFATFSTask();
}




