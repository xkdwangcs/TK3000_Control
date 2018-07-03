#ifndef _DES_H_
#define _DES_H_

#ifdef __cplusplus
extern "C" {
#endif 

#include "stm32f4xx.h"
#include "stdbool.h"
#include "string.h"
#include <stdint.h>

//加密数据。plainStr:明文，key:加密或解密的键，ciphStr:密文
void Encrypt_DES(u8* plainStr, u8* key, u8* ciphStr);
//解密数据。plainStr:明文，key:加密或解密的键，ciphStr:密文
void Decipher_DES(u8* plainStr, u8* key, u8* ciphStr);
    
#ifdef __cplusplus
}
#endif

#endif 
