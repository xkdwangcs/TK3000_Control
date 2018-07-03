#ifndef _Public_H
#define _Public_H

#ifdef __cplusplus
 extern "C" {
#endif 
	 
#include "stm32f4xx.h"
#include "string.h"
#include <stdio.h>
#include "stdlib.h"
#include "stdbool.h"

#define _dicLeng  20//字典元素的最大个数
	 
//字典元素结构
typedef struct
{
	char key[40];
	char value[80];
} Dictionary;

//列表元素结构，是否采用 *char[40]的方式
typedef struct
{
	char item[40];
} LIST;

typedef union
{
	float f;
	unsigned char c4[4];
} floatBytesTypedef;

typedef union
{
	int val32;
	unsigned char c4[4];
} intBytesTypedef;

typedef union
{
	s16 val16;
	unsigned char c2[2];
} s16BytesTypedef;

typedef union
{
	u16 val16;
	unsigned char c2[2];
} u16BytesTypedef;

typedef union
{
	u32 u32Data;
	u8 u8Ary4[4];
} u32BytesType;

extern Dictionary _dic[_dicLeng];

//解析一个字符串，返回一个字典。字符形式："MID|Y" OR "MID|X,DIR|FFX,SPD|-12"
Dictionary* ConvertToDic(char *tpara, u8 *itemCount);
//分解一个字符串，返回一个列表。字符形式："AA/BB/CC"
LIST* SplitString(char *tpara,char* split, u8* itemCount); 
//解析一个字符串，返回一个列表。字符形式："1.0，1.3，1.6"
LIST* GetLIST(char *tpara, u8 *itemCount);
//根据一个浮点数组得到一个字符串，元素间用逗号隔开
char* ConvertFloatArrToStr(float* datas,u8 count);
//指定字典，一个key。查询并返回value
char *GetValue(Dictionary *dic, char *key);
//指定字典，字典元素个数，一个key。查询并返回value。此函数比<GetValue>安全
char *GetValue_n(Dictionary *dic, u8 itemCount, char *key);
//指定字典，一个value,返回对应的key
char* GetKey(Dictionary* dic, u8 itemCount, char* value);
//比较两个字符，要以'\0'结尾，如果相等则返回1,否则返回0
bool StrCMP(const char *s1, const char *s2);
//比较两个字符,指定长度，如果相等则返回1,否则返回0
bool StrCMP_n(const char *s1, const char *s2,u8 length);
//删除后面的空格
void TrimTail(char s[]);
//转换为u32
u32 ConvertAsciiTou32(char *s);
//ascii字节转换为浮点数
float ConvertAsciiToFloat(char *s);
//浮点数转ASCII码
char* ConvertFloatToAscii(float f);
//浮点数转ASCII码,带格式化
char* ConvertFloatToAsciiFormat(float f, char* formatStr);
//u32数转ASCII码
char* ConvertU32ToAscii(u32 d);
//浮点数转为4字节char
char* ConvertFloatTo4Byte(float data);
//四字节转换为浮点数
float Convert4ByteToFloat(char *s);
//四字节转换为浮点数
float Convert4ByteToFloat_offset(char *s, int startIndex);
//在destStr的尾部添加指定的字符串
void AppendStr(char *destStrArr, char *tailStr);
//在destStr的尾部添加指定的字符串,并在两字符串中插入指定的字符
void AppendStrAddSplit(char *destStrArr, char *split, char *tailStr);
//联接三个字符串
void LinkThreeStr(char* destStrArr,char* str1, char* str2, char* str3);
//在destArr的尾部添加指定的字符串，destArr要用数组。
//destOffset:destArr开始复制的索引；tailCount:要复制的个数
void CopyBytes(u8* destArr, int destOffset, u8* sourceArr, int copyCount);
//在destArr的尾部添加指定的字符串，destArr要用数组。
//destOffset:destArr开始复制的索引；tailCount:要复制的个数
void CopyBytes1(u8* destArr, int destOffset, u8* sourceArr,int sourceOffset, int copyCount);
//二维字符数组降序排序。
//strArray:指向二维字符数组的指针,如str[3][10]；itemCount:数组行数(元素个数);itemByteNum：元素字节数(一行多少字节)
void StringArrayShortDES(char* strArray,u8 itemCount,u8 itemByteNum);
//二维字符数组升序排序
//strArray:指向二维字符数组的指针,如str[3][10]；itemCount:数组行数(元素个数);itemByteNum：元素字节数(一行多少字节)
void StringArrayShortASC(char* strArray,u8 itemCount,u8 itemByteNum);
//取指定值的符号
s8 Sign(float v);


void charRightAlign(char *str, u32 alignLen);
void charLeftAlign(char *str, u32 alignLen);
bool addPercentCharAfterDigitChar(char *str);
bool isValidIntStr(char *str);
bool isValidFloatStr(char *str, int strMaxLen);
bool isValidwithSymFloatStr(char *str, int strMaxLen);

#ifdef __cplusplus
}
#endif

#endif
