#ifndef _HANDLERS_H_
#define _HANDLERS_H_

#ifdef __cplusplus
extern "C" {
#endif 

#include "stm32f4xx.h"
#include "stdbool.h"

//带一个字符串作为参数的函数指针
typedef void (*OneStrParameterHandler)(char* para);  	
//带一个整形作为参数的函数指针
typedef void (*OneIntParameterHandler)(int para);  
//无参数的函数指针
typedef void (*EventHandlerNoPara)(void);
//单击事件函数指针
typedef void (*PCLICK)(u16 keyCode);
//录入框确定函数指针
typedef void (*PInputBoxEnter)(u16 keyCode, char* text);    
    
#ifdef __cplusplus
}
#endif

#endif 
