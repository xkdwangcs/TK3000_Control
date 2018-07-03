#include "DateTime.h"

#define isPrime(year) ((year%4==0&&year%100!=0)||(year%400==0)) //是否为闰年 

//比较两个日期，可以包括时间（也可以没有），要求两个日期的格式是一致的
CMPValueEnum DateTimeCMP(char* dateTime1,char* dateTime2)
{
	int res =strcmp(dateTime1,dateTime2);
    if(res==0) return Equal;  //相等
    if(res<0 ) return Less;   //dt1<dt2
    if(res>0 ) return Greater;//dt1>dt2
}

char _dateBuff1[20];
//把形式如"20160802"的格式转换为2016-08-02
char* DateFormat(char* dateStr)
{
    memset(_dateBuff1,0,sizeof(_dateBuff1));
    CopyBytes((u8*)_dateBuff1,0,(u8*)dateStr,4);
    _dateBuff1[4]='-';
    CopyBytes1((u8*)_dateBuff1,5,(u8*)dateStr,4,2);
    _dateBuff1[7]='-';
    CopyBytes1((u8*)_dateBuff1,8,(u8*)dateStr,6,2);
    return _dateBuff1;
}

//从字符串分解出日期,不分解时间,格式:2016-08-11
void ConvertToDate(char* dateStr,DataTimeStruct* dateTime)
{
	if(strlen(dateStr)<10)
		return;
	int iY, iM, iD, iH, iMin, iS;
	iY = atoi(dateStr);
	iM = atoi(dateStr+5);
	iD = atoi(dateStr+8);
	dateTime->Year=iY;
	dateTime->Mon=iM;
	dateTime->Day=iD;
}

//从字符串分解出日期及时间，格式:2016-08-11 21:13:15
void ConvertToDateTime(char* dateTimeStr,DataTimeStruct* dateTime)
{
	if(strlen(dateTimeStr)<19)
		return;
	int iY, iM, iD, iH, iMin, iS;
	iY = atoi(dateTimeStr);
	iM = atoi(dateTimeStr+5);
	iD = atoi(dateTimeStr+8);
	iH = atoi(dateTimeStr+11);
	iMin = atoi(dateTimeStr+14);
	iS = atoi(dateTimeStr+17);

	dateTime->Year=iY;
	dateTime->Mon=iM;
	dateTime->Day=iD;
	dateTime->Hour=iH;
	dateTime->Min=iMin;
	dateTime->Sec=iS;
}

//求两个日期的相差天数，date1-date2,有正负
int CalcDateDiff_Struct(DataTimeStruct date1,DataTimeStruct date2)  
{  
    int days=0,j,flag;  
    const int primeMonth[][12]={{31,28,31,30,31,30,31,31,30,31,30,31},{31,29,31,30,31,30,31,31,30,31,30,31}};     
    //交换两个日期函数,将小的日期给date2,将大的日期给date1 
    DataTimeStruct tmp;
	bool isjf=false;//是否交换大小
    if ((date2.Year>date1.Year)||(date2.Year==date1.Year&&date2.Mon>date1.Mon)||
		(date2.Year==date1.Year&&date2.Mon==date1.Mon&&date2.Day>date1.Day))  
    {  
        tmp=date2;  
        date2=date1;  
        date1=tmp; 
		isjf=true;		
    }   
    //从date2.Year开始累加到date1.Year 
    for(j=date2.Year;j<date1.Year;++j)  
        days+=isPrime(j)?366:365;  
      
    //如果date1.Year是闰年,则flag=1,后面调用primeMonth[1][12]  
    flag=isPrime(date1.Year);  
    //加上date1.Mon到1月的天数  
    for (j=1;j<date1.Mon;j++)  
        days+=primeMonth[flag][j-1];  
  
    //减去mindate.Mon到1月的天数
    flag=isPrime(date1.Year);  
    for (j=1;j<date2.Mon;j++)  
		days-=primeMonth[flag][j-1];  
    days=days+date1.Day-date2.Day;  
	if(isjf) days*=-1;
    return days;  
}  

//求两个日期的相差天数，date1-date2,有正负
int CalcDateDiff_String(char* date1,char* date2)  
{  
	DataTimeStruct dts1,dts2;
	ConvertToDate(date1,&dts1);
	ConvertToDate(date2,&dts2);
	return CalcDateDiff_Struct(dts1,dts2);
} 

//指定一个日期，加上天数。返回新的日期
char* DateAddDay(char* oldDate,int days)
{
    return _dateBuff1;
}
