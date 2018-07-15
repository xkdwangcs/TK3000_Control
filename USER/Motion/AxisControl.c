#include "AxisControl.h"
#include "Parameter.h"
#include "PortConfigFile.h"
#include "math.h"

//����Ʋ����б�
AxisControlStruct AxisList[AxisNum];
//��ǰ����
MultiAxisCoordStruct _currCoord;
MotorIDEnum _xMotor=M1;
MotorIDEnum _y1Motor=M2;
MotorIDEnum _y2Motor=M4;
MotorIDEnum _zMotor=M3;

MotorIDEnum GetMotorIDFormPortFile(AxisIDEnum axisID)
{
	for(u8 i=0;i<AxisNum;i++)
	{
		if(PortMotorCFG.MotorCFG[i].AxixID==axisID)
			return PortMotorCFG.MotorCFG[i].MotorID;
	}	
}

//���ʼ��,�Ա���Ŀ���õ�ȫ������г�ʼ��
void AxisInit(void)
{
	MotorInit();
	for(u8 i=0;i<AxisNum;i++)
	{
		AxisList[i].AxisID=(AxisIDEnum)i;
		//AxisList[i].MotorID=PortMotorCFG.MotorCFG[i].MotorID;//�������ļ��ж���
	}
	AxisList[X].MotorID=GetMotorIDFormPortFile(X);//�������ļ��ж���
	AxisList[Y1].MotorID=GetMotorIDFormPortFile(Y1);
	AxisList[Y2].MotorID=GetMotorIDFormPortFile(Y2);
	AxisList[Z].MotorID=GetMotorIDFormPortFile(Z);
	
	AxisList[X].AxisType=PlaneAxis;
	AxisList[Y1].AxisType=PlaneAxis;
	AxisList[Y2].AxisType=PlaneAxis;
	AxisList[Z].AxisType=VerticalAxis;
	
	//�ɹ��ⲿֱ�ӵ���	
	_xMotor=AxisList[X].MotorID;
	_y1Motor=AxisList[Y1].MotorID;
	_y2Motor=AxisList[Y2].MotorID;
	_zMotor=AxisList[Z].MotorID;
}

//ȡ��ʵʱ����
MultiAxisCoordStruct* GetRealTimeCoord(void)
{
	_currCoord.X1 = MotorList[_xMotor].RealPulse * Parameter.PulseK[_xMotor];
	_currCoord.Y1 = MotorList[_y1Motor].RealPulse * Parameter.PulseK[_y1Motor];
	_currCoord.Y2 = MotorList[_y2Motor].RealPulse * Parameter.PulseK[_y2Motor];
	_currCoord.Z1 = MotorList[_zMotor].RealPulse * Parameter.PulseK[_zMotor];
	return &_currCoord;
}

//��һ����ŵõ����ַ���
char* GetAxisStrFromID(AxisIDEnum axisID)
{
	switch(axisID)
	{
		case X:return "X";
		case Y1:return "Y1";
		case Y2:return "Y2";
		case Z:return "Z";
	}
}

//�����ַ����õ����
AxisIDEnum GetAxisIDFormStr(char* axisStr)
{
	if(StrCMP_n(axisStr,"X",1))
	{
		return X;
	}
	if(StrCMP_n(axisStr,"Y1",2))
	{
		return Y1;
	}
	if(StrCMP_n(axisStr,"Y2",2))
	{
		return Y2;
	}
	if(StrCMP_n(axisStr,"Z",1))
	{
		return Z;
	}
	u8 leng=strlen(axisStr);
	if(leng==1 && StrCMP_n(axisStr,"Y",leng))
	{
		return Y1;
	}
}

//��ȡָ���������״̬��true:�������У�false:ֹͣ
bool GetAxisRunState(AxisIDEnum axisID)
{
	return MotorList[AxisList[axisID].MotorID].IsRunning;
}

//��ȡָ����ĵ�ǰʵʱ����ֵ�����������ֵ
s32 GetAxisRealPulse(AxisIDEnum axisID)
{
	return MotorList[AxisList[axisID].MotorID].RealPulse;
}

//��㶯����
void AxisPTRun(AxisIDEnum axisID,DirectEnum direct)
{
	MotorPTRun(AxisList[axisID].MotorID,direct);
}

//��㶯����
void AxisPTStop(AxisIDEnum axisID)
{
	MotorPTStop(AxisList[axisID].MotorID);
}

//����ͬʱ���У��Ż�������Ч�ʡ�2018.04.02
void MoveToCoord_Multi(AxisIDEnum* axisList,float* destCoordList,u8 count,bool isSynch)
{
    MotorIDEnum motors[count];
    s32 pulseList[count];
    for(u8 i=0; i<count; i++)
    {
        MotorIDEnum mid=AxisList[axisList[i]].MotorID;
        s32 steps = (s32)(floor(destCoordList[i]/Parameter.PulseK[mid])-MotorList[mid].RealPulse);
        motors[i]=mid;
        pulseList[i]=steps;
    }
    MotorRun(motors,pulseList,count,isSynch);
}

//ָ������ƶ���Ŀ�����꣬û�в��붯������Ϊ�ƶ������깦�ܵĵײ㺯��
//isSynch:�Ƿ�ͬ����ָ�Ƿ�ȴ�����ƶ�����ٷ��أ�����Ϊ�첽
void MoveToCoord(AxisIDEnum axisID,float destCoord,bool isSynch)
{
    AxisIDEnum axisList[1]= {axisID};
    float coordList[1]= {destCoord};
    MoveToCoord_Multi(axisList,coordList,1,isSynch);
}

//ָֹͣ������
void StopAxis(AxisIDEnum axisID)
{
	MotorStop(AxisList[axisID].MotorID);
}

//ֹͣƽ̨���е����˶�
void StopAllAxis(void)
{
	for(u8 i=0;i<AxisNum;i++)
	{
		MotorStop(AxisList[i].MotorID);
	}
}

//�ƶ�������ֱ����λ�ź��ⲿ�жϣ��Ϊ�г̳���
//��λ�ٶ�Ҫ���ã�
//������㣬��ͬ���ź�
void MultiAxisMoveToPositionZero(AxisIDEnum* axisIDs,u8 count)
{
    ResetFlag=true;
    float resetSpeed=Parameter.ResetSpeed;
	u8 countReal=0;
	MotorIDEnum motors[count];
	s32 pulseNum[count];
	float runSpeed[count];
	float accSpeed[count];
	
    for(int i=0; i<count; i++)
    {
        //������λλ�ã��ȴ�����������20mm
        MotorIDEnum motorID=AxisList[axisIDs[i]].MotorID;
        if(!GetInPortState(MotorPortList[motorID].LimitPort))//����Ѿ���ԭ���ԭ��
        {
			motors[countReal]=motorID;
			pulseNum[countReal]=20/Parameter.PulseK[motorID];
			runSpeed[countReal]=resetSpeed;
			accSpeed[countReal]=5*resetSpeed;
			countReal++;
        }
    }
	MotorRun_OutSpeed_Multi(motors,pulseNum,runSpeed,accSpeed,countReal,true);
	
    //���㣬�г̳��ȶ���50mm
    for(int i=0; i<count; i++)
    {
        MotorIDEnum motorID=AxisList[axisIDs[i]].MotorID;
		motors[i]=motorID;
		pulseNum[i]=-(Parameter.AxisLength[motorID]+50)/Parameter.PulseK[motorID];
		runSpeed[i]=resetSpeed;
		accSpeed[i]=5*resetSpeed;
    }
    MotorRun_OutSpeed_Multi(motors,pulseNum,runSpeed,accSpeed,count,true);
	
    //�������߻������
    for(int i=0; i<count; i++)
    {
        MotorIDEnum motorID=AxisList[axisIDs[i]].MotorID;
		motors[i]=motorID;
		pulseNum[i]=-MotorList[motorID].RealPulse*5;
		runSpeed[i]=resetSpeed;
		accSpeed[i]=5*resetSpeed;
    }
	//MotorRun_OutSpeed_Multi(motors,pulseNum,runSpeed,accSpeed,count,isSynch);
	MotorRun_OutSpeed_Multi(motors,pulseNum,runSpeed,accSpeed,count,true);

    //�ڶ����ҹ�����
    for(int i=0; i<count; i++)
    {
        MotorIDEnum motorID=AxisList[axisIDs[i]].MotorID;
		motors[i]=motorID;
		pulseNum[i]=-MotorList[motorID].RealPulse*2;
		runSpeed[i]=resetSpeed/4;
		accSpeed[i]=resetSpeed;
    }
	MotorRun_OutSpeed_Multi(motors,pulseNum,runSpeed,accSpeed,count,true);
	//ԭ����λ��־����
    for(int i=0; i<count; i++)
    {
        MotorIDEnum motorID=AxisList[axisIDs[i]].MotorID;
        MotorList[motorID].LimitOrigin = false;
    }
	
    //�������߻������
    for(int i=0; i<count; i++)
    {
        MotorIDEnum motorID=AxisList[axisIDs[i]].MotorID;
		motors[i]=motorID;
		pulseNum[i]=-MotorList[motorID].RealPulse;
		runSpeed[i]=resetSpeed/4;
		accSpeed[i]=resetSpeed;
    }
    MotorRun_OutSpeed_Multi(motors,pulseNum,runSpeed,accSpeed,count,true);
    ResetFlag=false;
}

//������㣬��ͬ���źţ����ƶ�������λ��
void MultiAxisMoveToPositionZeroAndMove(AxisIDEnum* axisIDs, u8 count)
{
    MultiAxisMoveToPositionZero(axisIDs,count);
    float resetSpeed=Parameter.ResetSpeed;
	MotorIDEnum motors[count];
	s32 pulseNum[count];
	float runSpeed[count];
	float accSpeed[count];
    for(int i = 0; i<count; i++)
    {
        MotorIDEnum motorID=AxisList[axisIDs[i]].MotorID;
        float coord=Parameter.WorkHeight;
        if(AxisList[axisIDs[i]].AxisType!=VerticalAxis)//������Ǵ�ֱ��
        {
            coord=GetScrewOneAxisCoord(axisIDs[i]);
        }
		motors[i]=motorID;
		pulseNum[i]=coord/Parameter.PulseK[motorID];
		runSpeed[i]=resetSpeed;
		accSpeed[i]=5*resetSpeed;
    }
    MotorRun_OutSpeed_Multi(motors,pulseNum,runSpeed,accSpeed,count,true);
}

