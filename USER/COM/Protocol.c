#include "Protocol.h"
#include "CMD.h"
#include "COM_3.h"
#include "Parameter.h"

char *_cmdERR = "CMDERR";
char *_crcERR = "CRCERR";
char *_tailERR = "TAILERR";
char *_funcERR = "FUNCERR";

u8 _head = 0xAB;
u8 _tail = 0xBA;
u8 _destAddr = 0x01;
u8 _sourAddr = 0x08;
u16 _poly = 0x1021; //crc生成多项式
u8 _dataOverBytes=33; //数据以外的字节数

//命令接收缓存
ProtocolBuffStruct _cmdRecvBuff;
//命令发送缓存
ProtocolBuffStruct _cmdSendBuff;
u16 _recvStartIndex=0;

//获取环形队列中的真实地址
u16 GetAddrReal(u16 addr)
{
	if(addr<_com3BuffLeng)
		return addr;
	return addr-_com3BuffLeng;
}

//计算CRC16
u16 CRC16ByPoly(u8 *data, u16 startIndex, u16 length)
{
	u16 crc = 0;
	u16 i = 0;
	u8 bt, b1, b2;
	while (length-- > 0)
	{
		bt = data[startIndex++];
		for (i = 0; i < 8; i++)
		{
			b1 = (crc & 0x8000) != 0;
			b2 = (bt & 0x80) != 0;
			if (b1 != b2)
				crc = (u16) ((crc << 1) ^ _poly);
			else
				crc <<= 1;
			bt <<= 1;
		}
	}
	return crc;
}

//AB LEN1 LEN2 ADDR1 ADDR2 PNUM PID FUN1 FUN2 FUN3 CNAME1..CNAME20 data1....dataN CRC1 CRC2 BA
//组包，返回组好包的包长
u16 Package(char* func,char* cmdName,u8* cmdData,u16 dataLeng)
{
	u16 length = dataLeng + _dataOverBytes;
	memset((u8*)&_cmdSendBuff,0,sizeof(_cmdSendBuff));
	_cmdSendBuff.Head = _head;
	_cmdSendBuff.Length = length;
	_cmdSendBuff.DestAddr= _destAddr;
	_cmdSendBuff.SourAddr = _sourAddr;
	_cmdSendBuff.PackNum=0x00;
	_cmdSendBuff.PackID=0x00;
	strcpy(_cmdSendBuff.FunCode,func);
  strcpy(_cmdSendBuff.CmdName,cmdName);
  CopyBytes((u8*)&_cmdSendBuff.CmdData,0,cmdData,dataLeng);
	u16 crc16 = CRC16ByPoly((u8*)&_cmdSendBuff, 1, length - 4);
	_cmdSendBuff.CmdData[dataLeng] = crc16 / 256;
	_cmdSendBuff.CmdData[dataLeng + 1] = crc16 % 256;
	_cmdSendBuff.CmdData[dataLeng + 2] = _tail;
  _cmdSendBuff.DataLeng=dataLeng;
	return length;
}

void DisPackage(void)
{
	u16 packLeng = 0;
	u16 tailAddr=0;
	_recvStartIndex=0;
  for(int i=0;i<5;i++)
  {
      u16 num=0;
      bool isHavePack=false;
      while(num<_com3BuffLeng)
      {
        num++;
        if (_DMARecvBuff_COM3[_recvStartIndex] == _head)
        {
          packLeng = (u16) (_DMARecvBuff_COM3[GetAddrReal(_recvStartIndex+2)] << 8 | _DMARecvBuff_COM3[GetAddrReal(_recvStartIndex+1)]);
          tailAddr=GetAddrReal(_recvStartIndex+packLeng - 1);
          if (_DMARecvBuff_COM3[tailAddr] == _tail) //包尾正确
          {
            isHavePack=true;
            break;
          }
//          else
//          {
//            SendCharsPack(_srq,_tailERR);
//          }
        }
        _DMARecvBuff_COM3[_recvStartIndex]=0;
        _recvStartIndex=GetAddrReal(_recvStartIndex+1);
      }
      if(!isHavePack)//如果无有效包
        return;
      memset((u8*)&_cmdRecvBuff,0,sizeof(_cmdRecvBuff));
      u8* cmdRecvP=(u8*)&_cmdRecvBuff;
      for(u16 i=0;i<packLeng;i++)
      {
        u16 addr=GetAddrReal(_recvStartIndex+i);
        *cmdRecvP=_DMARecvBuff_COM3[addr];
        cmdRecvP++;
        _DMARecvBuff_COM3[addr]=0;
      }
      u16 cmdDataLeng=packLeng-_dataOverBytes;
      cmdRecvP=(u8*)&_cmdRecvBuff;
      u16 recCRC = (u16) (cmdRecvP[packLeng - 3] << 8 | cmdRecvP[packLeng - 2]);
      u16 calcCRC = CRC16ByPoly(cmdRecvP, 1, packLeng - 4);
//      if (recCRC != calcCRC)
//      {
//        SendCharsPack(_srq,_crcERR);
//      }
      cmdRecvP[packLeng - 3]=0;
      cmdRecvP[packLeng - 2]=0;
      cmdRecvP[packLeng - 1]=0;
      _cmdRecvBuff.DataLeng=cmdDataLeng;
      _recvStartIndex= GetAddrReal(_recvStartIndex+packLeng);
      CMDAnalysis();
      //试探后面是否还有数据包
      isHavePack=false;
      for(u16 i=0;i<3;i++)//如果3个字节都为0，认为无有效包。
      {
        if(_DMARecvBuff_COM3[_recvStartIndex+i]!=0)
        {			
          isHavePack=true;
          break;
        }
      }
      if(!isHavePack)
        return;
    }
	}

//命令解析,recvPack：接收到；commChannel：当前接收的通信通道
void CMDAnalysis(void)
{
	u16 packLeng = 0;
	if(strncmp(_cmdRecvBuff.FunCode, _rdd, 3) == 0)
	{
		RDDProcess();
	}
	else if(strncmp(_cmdRecvBuff.FunCode, _wrt, 3) == 0)
	{
		WRTProcess();
	}
	else if(strncmp(_cmdRecvBuff.FunCode, _out, 3) == 0)
	{
		OUTProcess();
	}
	else
	{
		//packLeng = PackageChars(_oneSendPackBuff_COM1,_recPacket.func, _funcERR);
		//SendBytes_COM1( _oneSendPackBuff_COM1, packLeng);
	}
}

//作为应答的数据发送
void ResponseSendData(char* func, u16 cmdDataLeng)
{
//	if(Parameter.CommChannel==RS232)
//	{
//		u16 leng=Package(_protocolBuff.CmdBuff,func,cmdDataLeng);
//		SendBytes_COM1((u8*)_protocolBuff.CmdBuff,leng);
//	}
//	else if(_currCommChannelEnum==WIFI)
//	{
//		SendCharsPack_WIFI(func,cmdData);
//	}
}

//向指定的串口发送组包后的数据
void SendBytes_Pack(char* func,char* cmdName, u8* cmdData,u16 cmdLeng)
{
	u16 leng=Package(func,cmdName,cmdData,cmdLeng);
	SendBytes_COM3((u8*)&_cmdSendBuff,leng);
}

//向指定的串口发送组包后的数据
void SendChars_Pack(char* func,char* cmdName, char* cmdData)
{
	u16 leng=Package(func,cmdName,(u8*)cmdData,strlen(cmdData));
	SendBytes_COM3((u8*)&_cmdSendBuff,leng);
}


