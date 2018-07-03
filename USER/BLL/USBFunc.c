#include "USBFunc.h"
#include "FileOperate.h"
#include "main.h"
#include "AppTasks.h"
//#include "MoveFileExImportForm.h"

//TextBox* _txtUSBMsg;
//DicTextBox* _listIsCover; //�����Ƿ񸲸�
bool _isCover;  //�����Ƿ񸲸�
char* _paraFileNameUSB="1:/CFG/Parameter.cfg";
ParameterStruct ParameterUSB; //USB�еĲ����ļ�����
//��Fat������ص�����ö��
USBOperateEnum _usbOperate=NoUSBOP;

//��ʾ��ǰ�ļ�������Ϣ
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

//���豸���������ļ���U��
void CopyDeviceToUSB()
{
    //bool isCover= ConvertAsciiTou32(_listIsCover->currKV->value);	
    bool bl = CopyAllFile_Dir("0:/COORD",GetUSBFullPath("1:/COORD"),_isCover,ShowMSG);
	if(bl) 
	{
		ShowMSG("�����ļ������ɹ���");
		//PopForm(2);
	}
	else
	{
		ShowMSG("�����ļ�����ʧ�ܣ�");
	}
}

//��U�̸��������ļ����豸
void CopyUSBToDevice()
{
    bool bl = CopyAllFile_Dir(GetUSBFullPath("1:/COORD"),"0:/COORD",_isCover,ShowMSG);
	if(bl) 
	{
		ShowMSG("�����ļ�����ɹ���");
		//PopForm(2);
	}
	else
	{
		ShowMSG("�����ļ�����ʧ�ܣ�");
	}
} 

//������־�ļ���U��
void CopyLogFileToUSB()
{
    bool bl = CopyAllFile_Dir("0:/LOG",GetUSBFullPath("1:/LOG"),_isCover,ShowMSG);  
	if(bl) 
	{
		ShowMSG("��־�ļ������ɹ���");
		//PopForm(2);
	}
	else
	{
		ShowMSG("��־�ļ�����ʧ�ܣ�");
	}
}

//�̼�����
void FirmwareUpdate(u16 keyCode)
{
	*(__IO uint8_t *) (FirmwareUpdateTagAddr)=1;
	SystemReset();
}

//���������ļ���U�̣�Ŀǰ��Ҫ����������Ϣ�ĵ���
void ExportParamaterFileToUSB(void)
{
	memset(_usbPath,0,sizeof(_usbPath));
	sprintf(_usbPath,"1:/CFG/%s/Parameter.cfg",Parameter.ProdModel);
	//bool isCover= ConvertAsciiTou32(_listIsCover->currKV->value);
	bool bl= CopyFile(_paraFileName,_paraFileNameUSB,_isCover);//ͨ�õĲ�������������Ϣ����
	bl=bl && CopyFile(_paraFileName,_usbPath,_isCover);//��ͬ�ͺ��豸
	if(bl) 
	{
		ShowMSG("�����ļ������ɹ���");
		//PopForm(2);
	}
	else
	{
		ShowMSG("�����ļ�����ʧ�ܣ�");
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
		ShowMSG("�����ļ�����ɹ���");
		//PopForm(2);
	}
	else
	{
		ShowMSG("�����ļ�����ʧ�ܣ�");
	}
}

//����������Ϣ���豸
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
		ShowMSG("������Ϣ����ɹ���");
		//PopForm(2);
	}
	else
	{
		ShowMSG("U���еĲ����ļ�����");
	}
}

void PortFileToUSB(void)
{
	memset(_usbPath,0,sizeof(_usbPath));
	sprintf(_usbPath,"1:/CFG/%s/PortMotor.cfg",Parameter.ProdModel);
	bool bl=CopyFile(_portFileName,_usbPath,_isCover);//��ͬ�ͺ��豸
	if(bl) 
	{
		ShowMSG("�˿��ļ������ɹ���");
		//PopForm(2);
	}
	else
	{
		ShowMSG("�˿��ļ�����ʧ�ܣ�");
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
		ShowMSG("�˿��ļ�����ɹ���");
		//PopForm(2);
	}
	else
	{
		ShowMSG("�˿��ļ�����ʧ�ܣ�");
	}
}

//USB��ز���������
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

//USB�������浥����ť�¼�
void btnOnclick_USBOP(u16 keyCode)
{
	if(keyCode==0x0001)  //���������ļ����豸->USB
    {
       _usbOperate=ExportCoordFile;
    }
    else if(keyCode==0x0002)  //���������ļ���USB->�豸
    {
        _usbOperate=ImportCoordFile;
    }
	else if(keyCode==0x0003)  //��������
    {
        _usbOperate=ExportParaFile;
    }
    else if(keyCode==0x0004)  //��������
    {
        _usbOperate=ImportParaFile;
    }
	else if(keyCode==0x0005)//��־����
	{
		_usbOperate=ExportLogFile;
	}
	else if(keyCode==0x0006)//���Ƶ���
	{
		_usbOperate=ImportMPPara;
	}
	else if(keyCode==0x000A)//�˿��ļ�����
	{
		_usbOperate=ExportPortFile;
	}
	else if(keyCode==0x000B)//�˿��ļ�����
	{
		_usbOperate=ImportPortFile;
	}
	StartFATFSTask();
}




