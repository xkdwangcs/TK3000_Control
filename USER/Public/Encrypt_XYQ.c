#include "Encrypt_XYQ.h"
#include "RTC_F4xx.h"
#include <stdlib.h>
#include <time.h>

const u8 _charLeng=16;
char _plainChars[_charLeng]={"0123456789ABCDEF"};//原文字符集
char _chiphChars1[_charLeng]={"PX9WK2HMD83FGY5A"};//密文字符集1
char _chiphChars2[_charLeng]={"BQN4JTE6RUC7VLZS"};//密文字符集2

//指定一个明文字符，获得对应的密文字符。
//机器密
char GetChiphChar(char plainChar)
{
	char* chiphChars=_chiphChars1;
	if(GetCurrDataTime()->Sec%2==0)//要用ms的变化才有好效果
		chiphChars=_chiphChars2;	
	//srand(time(NULL)); //这样不行，要获取系统时间，在Linux/Uinux可行
//	srand(GetCurrDataTime()->Sec);//效果跟第一种一样的，要用ms的变化才有好效果
//	u16 randValue = rand()%100+1;
//	if(randValue%2==0)
//		chiphChars=_chiphChars2;
    for(u8 index=0;index<_charLeng;index++)
    {
        if(_plainChars[index]==plainChar)
            return chiphChars[index] ;
    }
    return 0;        
}

//指定一个密文字符，获得对应的明文字符
char GetPlainChar(char chiphChar)
{
    for(u8 index=0;index<_charLeng;index++)
    {
        if(_chiphChars1[index]==chiphChar)
            return _plainChars[index];  
		if(_chiphChars2[index]==chiphChar)
            return _plainChars[index];  
    }
    return 0;    
}

//加密数据。plainStr:明文，ciphStr:密文
void Encrypt_XYQ(char* plainStr, char* ciphStr)
{
    u8 leng=strlen(plainStr);
    for(u8 i=0;i<leng-1;)
    {
        ciphStr[i+1]=GetChiphChar(plainStr[i]);
        i=i+2;
    }
    for(u8 i=1;i<leng;)
    {
        ciphStr[i-1]=GetChiphChar(plainStr[i]);
        i=i+2;
    }
    if(leng%2 >0)
    {
        ciphStr[leng-1]=GetChiphChar(plainStr[leng-1]);
    }
}

//解密数据。ciphStr:密文,plainStr:明文，
bool Decipher_XYQ(char* ciphStr,char* plainStr)
{
    u8 leng=strlen(ciphStr);
    for(u8 i=0;i<leng-1;)
    {
        char cc = GetPlainChar(ciphStr[i]);
        if(cc==0)
            return false;
        plainStr[i+1]=cc;
        i=i+2;
    }
    for(u8 i=1;i<leng;)
    {
        char cc = GetPlainChar(ciphStr[i]);
        if(cc==0)
            return false;
        plainStr[i-1]=GetPlainChar(ciphStr[i]);
        i=i+2;
    }  
    if(leng%2 >0)
    {
        char cc = GetPlainChar(ciphStr[leng-1]);
        if(cc==0)
            return false;
        plainStr[leng-1]=cc;
    }
    return true;
}