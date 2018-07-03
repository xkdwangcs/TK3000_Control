/*
* Public.c
*
*  Created on: 2014年7月8日
*      Author: Administrator
*/
#include "PublicFunction.h"
#include "MemoryAlloc.h"

char _charArr[20]; //转换为字符指针的全局变量
Dictionary _dic[_dicLeng] __at(DicBuffAddr); //字典全局变量
const u8 _listLeng = 10; //列表元素的最大个数
LIST _list[_listLeng]; //列表全局字典
floatBytesTypedef _floatData;

//解析一个字符串，返回一个字典。字符形式："MID|Y" OR "MID|X,DIR|FFX,SPD|-12"
Dictionary* ConvertToDic(char *tpara, u8 *itemCount)
{
    int index = 0;
    char *split = "|,";
    char* tp=strtok(tpara,split);
    int dicIndex = 0;
    while (tp != NULL)
    {
        if (index % 2 == 0)
        {
            strcpy(_dic[dicIndex].key, tp);
        }
        else
        {
            strcpy(_dic[dicIndex].value, tp);
            dicIndex++;
        }
        tp = strtok(NULL, split);
        index++;
    }
    *itemCount = dicIndex;
    return _dic;
}

//指定字典，字典元素个数，一个key。查询并返回value。此函数比<GetValue>安全
char *GetValue_n(Dictionary *dic, u8 itemCount, char *key)
{
    for (int i = 0; i < itemCount; i++)
    {
        if (StrCMP(dic[i].key, key))
        {
            return dic[i].value;
        }
    }
    return NULL;
}

//指定字典，一个value,返回对应的key
char* GetKey(Dictionary* dic, u8 itemCount, char* value)
{
    for (int i = 0; i < itemCount; i++)
    {
        if (StrCMP(dic[i].value, value))
        {
            return dic[i].key;
        }
    }
    return NULL;
}

//指定字典，一个key。查询并返回value
char* GetValue(Dictionary *dic, char *key)
{
    return GetValue_n(dic, _dicLeng, key);
}

//分解一个字符串，返回一个列表。字符形式："AA/BB/CC"
LIST* SplitString(char *tpara,char* split, u8* itemCount)
{
    int index = 0;
    char para[strlen(tpara)];
    strcpy(para, tpara);
    char *tp = strtok(para, split); //para必须为数组，否则出错
    while (tp != NULL)
    {
        strcpy(_list[index].item, tp);
        tp = strtok(NULL, split);
        index++;
    }
    *itemCount = index;
    return _list;
}

//解析一个字符串，返回一个列表。字符形式："1.0，1.3，1.6"
LIST* GetLIST(char *tpara, u8 *itemCount)
{
    return SplitString(tpara,",",itemCount);
}

char _arrTostrBuff[50];
//根据一个浮点数组得到一个字符串，元素间用逗号隔开
char* ConvertFloatArrToStr(float* datas,u8 count)
{
    memset(_arrTostrBuff,0,sizeof(_arrTostrBuff));
    for(u8 i=0; i<count; i++)
    {
        u8 leng=strlen(_arrTostrBuff);
        sprintf(_arrTostrBuff+leng,"%g,",datas[i]);
    }
    u8 leng=strlen(_arrTostrBuff);
    _arrTostrBuff[leng-1]=0x00;
    return _arrTostrBuff;
}

//比较两个字符，要以'\0'结尾，如果相等则返回1,否则返回0
bool StrCMP(const char *s1, const char *s2)
{
    if (strcmp(s1, s2) == 0)
        return true;
    return false;
}

//比较两个字符,指定长度，如果相等则返回1,否则返回0
bool StrCMP_n(const char *s1, const char *s2,u8 length)
{
    if (strncmp(s1, s2,length)==0)
        return true;
    return false;
}

//移除文本后的换行符
void TrimTail(char* str)
{
    u8 leng;
    do
    {
        leng=strlen(str);
        if(str[leng-2]==0x0D && str[leng-1]==0x0A)
        {
            str[leng-2]=0;
            str[leng-1]=0;
        }
        else
        {
            break;
        }
    } while(leng>0);
}

//转换为u32
u32 ConvertAsciiTou32(char *s)
{
    int v = atoi(s);
    return v;
}

//ascii字节转换为浮点数
float ConvertAsciiToFloat(char *s)
{
    float v = atof(s);
    return v;
}

//浮点数转ASCII码
char* ConvertFloatToAscii(float f)
{
    sprintf(_charArr, "%g", f); //%f精度不够
    return _charArr;
}

//浮点数转ASCII码,带格式化
char* ConvertFloatToAsciiFormat(float f, char* formatStr)
{
    sprintf(_charArr, formatStr, f);
    return _charArr;
}

//u32数转ASCII码
char* ConvertU32ToAscii(u32 d)
{
    //char a[20];//定义局部变量是不行的，因为返回后就被释放了
    sprintf(_charArr, "%d", d);
    return _charArr;
}

//四字节转换为浮点数
float Convert4ByteToFloat(char *s)
{
    float fv;
    unsigned char i;
    u8 *pf = (u8*) &fv;
    for (i = 0; i < 4; i++)
    {
        *(pf + i) = *(s + i);
    }
    return fv;
}

//四字节转换为浮点数
float Convert4ByteToFloat_offset(char *s, int startIndex)
{
    s = s + startIndex;
    return Convert4ByteToFloat(s);
    /*float fv;
    unsigned char i;
    u8 *pf = (u8*) &fv;
    for (i = startIndex; i < startIndex + 4; i++)
    {
    *(pf + i) = *(s + i);
    }
    return fv;*/
}

//浮点数转为4字节char
char* ConvertFloatTo4Byte(float data)
{
    _floatData.f = data;
    return (char*) _floatData.c4; //这里要注意类型
}

//在data的尾部添加指定的字符串，destStrArr要用数组
void AppendStr(char *destStrArr, char *tailStr)
{
    strcat(destStrArr, tailStr);

    /*u16 leng = strlen(destStr);//这样做可能比较危险
    u16 addLeng = strlen(tailStr);
    u16 i = 0;
    for (i = 0; i < addLeng; i++)
    {
    destStr[leng + i] = *tailStr++;
    }
    destStr[leng + i] = '\0';*/
}

//在destStr的尾部添加指定的字符串,并在两字符串中插入指定的字符
void AppendStrAddSplit(char *destStrArr, char *split, char *tailStr)
{
    AppendStr(destStrArr, split);
    AppendStr(destStrArr, tailStr);
}

//联接三个字符串
void LinkThreeStr(char* destStrArr,char* str1, char* str2, char* str3)
{
    sprintf(destStrArr,"%s%s%s",str1,str2,str3);
}

//在destArr的尾部添加指定的字符串，destArr要用数组。
//destOffset:destArr开始复制的索引；tailCount:要复制的个数
void CopyBytes(u8* destArr, int destOffset, u8* sourceArr, int copyCount)
{
    for (int i = 0; i < copyCount; i++)
    {
        destArr[destOffset + i] = sourceArr[i];
    }
}

//在destArr的尾部添加指定的字符串，destArr要用数组。
//destOffset:destArr开始复制的索引；tailCount:要复制的个数
void CopyBytes1(u8* destArr, int destOffset, u8* sourceArr,int sourceOffset, int copyCount)
{
    for (int i = 0; i < copyCount; i++)
    {
        destArr[destOffset + i] = sourceArr[sourceOffset+i];
    }
}

//按文件名降序排序比较器
int comp_strdes(const void* a,const void* b)
{
    return strcmp(b, a);
}

//按文件名升序排序比较器
int comp_strasc(const void* a,const void* b)
{
    return strcmp(a, b);
}

//二维字符数组降序排序。
//strArray:指向二维字符数组的指针,如str[3][10]；itemCount:数组行数(元素个数);itemByteNum：元素字节数(一行多少字节)
void StringArrayShortDES(char* strArray,u8 itemCount,u8 itemByteNum)
{
    qsort(strArray,itemCount,itemByteNum,comp_strdes);
}

//二维字符数组升序排序
//strArray:指向二维字符数组的指针,如str[3][10]；itemCount:数组行数(元素个数);itemByteNum：元素字节数(一行多少字节)
void StringArrayShortASC(char* strArray,u8 itemCount,u8 itemByteNum)
{
    qsort(strArray,itemCount,itemByteNum,comp_strasc);
}

//取指定值的符号
s8 Sign(float v)
{
    if(v>0)
        return 1;
    if(v<0)
        return -1;
    return 0;
}

/*字符右对齐, 如果不足指定字符数, 左边补空格*/
void charRightAlign(char *str, u32 alignLen)
{
    int i;
    int len;

    len = strlen(str);
    if (len < alignLen)
    {
        for (i = 0; i < len; i++)
            str[alignLen - 1 - i] = str[len - 1 - i]; //字符右移

        for (i = 0; i < alignLen - len; i++)
            str[i] = ' ';   //左边补空格

        str[alignLen] = 0;
    }
    else if (len > alignLen)
    {
        for (i = alignLen; i < len; i++)
            str[i] = 0;
    }
}

/*字符左对齐, 如果不足指定字符数, 右边边补空格*/
void charLeftAlign(char *str, u32 alignLen)
{
    int i;
    int len;

    len = strlen(str);
    if (len < alignLen)
    {
        for (i = len; i < alignLen; i++)
            str[i] = ' ';

        str[alignLen] = 0;
    }
    else if (len > alignLen)
    {
        for (i = alignLen; i < len; i++)
            str[i] = 0;
    }
}

/*在字符串中整数字符个数*/
int intCharNum(char *str)
{
    int i;
    int len;
    int intCharnum = 0;
    len = strlen(str);

    for (i = 0; i < len; i++)
    {
        if (str[i] >= '0' && str[i] <= '9')
            intCharnum++;
        else if (str[i] == '.')
            break;
    }
    return intCharnum;
}

bool isValidIntStr(char *str)
{
    int len = strlen(str);
    int i;

    for (i = 0; i < len; i++)
    {
        if (str[i] < '0' || str[i] > '9')
            break;
    }
    if (i == len)
        return true;
    else
        return false;
}

/*字符串是否是一个有效的浮点数字符串*/
bool isValidFloatStr(char *str, int strMaxLen)
{
    int len;
    int i;
    int point;

    len = strlen(str);
    if (len > strMaxLen || len == 0)
        return false;

    /*检查小数点个数*/
    point = 0;
    for (i = 0; i < len; i++)
    {
        if (str[i] == '.')
            point++;
    }
    if (point > 1)
        return false;

    /*检查是否是数字字符*/
    for (i = 0; i < len; i++)
    {
        if (!((str[i] >= '0' && str[i] <= '9') || str[i] == '.'))
            return false;
    }
    /*第一个字符不能为小数点*/
    if (str[0] == '.')
        return false;

    if (len == strMaxLen && len > 0 && str[len - 1] == '.')
        return false;
    return true;
}

bool isValidwithSymFloatStr(char *str, int strMaxLen)
{
    bool ret = false;

    if (str[0] == '-' || str[0] == '+') //第一位是符号位，从第二位开始
        ret = isValidFloatStr(&str[1], strMaxLen - 1);
    else
        ret = isValidFloatStr(str, strMaxLen);
    return ret;
}

bool isValidSoftVerStr(char *str, int strMaxLen)
{
    int len;
    int i;

    len = strlen(str);
    if (len > strMaxLen || len == 0)
        return false;

    /*首字符不能是小数点*/
    if (str[0] == '.')
        return false;

    /*检查是否是数字字符*/
    for (i = 0; i < len; i++)
    {
        if (!((str[i] >= '0' && str[i] <= '9') || str[i] == '.'))
            return false;
    }

    /*不能有连续的两个小数点*/
    for (i = 0; i < len - 1; i++)
    {
        if (str[i] == '.' && str[i + 1] == '.')
            return false;
    }

    if (len == strMaxLen && len > 0 && str[len - 1] == '.')
        return false;

    return true;
}

bool isValidZeroPointInput(char *str, int strMaxLen)
{
    bool ret = false;

    if (str[0] == '-' || str[0] == '+') //第一位是符号位，从第二位开始
        ret = isValidFloatStr(&str[1], strMaxLen - 1);
    else
        ret = isValidFloatStr(str, strMaxLen);

    return ret;
}

bool isValidDateStr(char *str)
{
    int len;
    int i;
    char tmp[8];
    int year, month, day;
    int leapyear = 0;

    len = strlen(str);
    if (len != 8)
        return false;

    for (i = 0; i < len; i++)
    {
        if (str[i] < '0' || str[i] > '9')
            return false;
    }

    memset(tmp, 0, sizeof(tmp));
    for (i = 0; i < 4; i++)
        tmp[i] = str[i];
    year = atoi(tmp);

    memset(tmp, 0, sizeof(tmp));
    for (i = 4; i < 6; i++)
        tmp[i - 4] = str[i];
    month = atoi(tmp);

    memset(tmp, 0, sizeof(tmp));
    for (i = 6; i < 8; i++)
        tmp[i - 6] = str[i];
    day = atoi(tmp);

    if ((year % 4) == 0 && (year % 100) != 0)
        leapyear = 1;

    if (month < 1 || month > 12)
        return false;
    if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12)
    {
        if (day < 1 || day > 31)
            return false;
    }
    else if (month == 4 || month == 6 || month == 9 || month == 11)
    {
        if (day < 1 || day > 30)
            return false;
    }
    else
    {
        if (leapyear)
        {
            if (day < 1 || day > 29)
                return false;
        }
        else
        {
            if (day < 1 || day > 28)
                return false;
        }
    }

    return true;

}

bool addPercentCharAfterDigitChar(char *str)
{
    int i;
    int len;
    bool ret = false;
    len = strlen(str);
    for (i = 0; i < len; i++)
    {
        if (str[i] == ' ')
            break;
    }
    if (i < len)
    {
        str[i] = '%';
        ret = true;
    }
    return ret;
}
