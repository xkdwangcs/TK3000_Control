#ifndef _ENCRYPT_H_
#define _ENCRYPT_H_

#ifdef __cplusplus
extern "C" {
#endif 

#include "stm32f4xx.h"
#include "stdbool.h"
#include "string.h"
#include <stdint.h>
    
//加密数据。plainStr:明文，ciphStr:密文
void Encrypt_XYQ(char* plainStr, char* ciphStr);
//解密数据。plainStr:明文，ciphStr:密文
bool Decipher_XYQ(char* ciphStr,char* plainStr);    
    
#ifdef __cplusplus
}
#endif

#endif 
