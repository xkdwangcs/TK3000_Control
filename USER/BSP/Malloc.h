#ifndef __MALLOC_H
#define __MALLOC_H
#include "stm32f4xx.h"
#include "string.h"

//V1.1 20140706
//1,�޸Ĳ��ֺ�������,��ֹ��������������ֳ�ͻ(����LWIP).
//2,�޸��ڴ��Ϊ32�ֽڶ���
//V1.2 20140716
//1,�޸�my_mem_malloc����,���ڴ��ڴ��������bug.

//#define CCMBaseAddr 0x10000000 //CCM�ڴ����ʼ��ַ
//#define CCMSize		0x10000 //CCM�ڴ��С��64KB

#define ExtSRAM_ADDR  		0x60000000 //��չ�ڴ���ʼ��ַ
#define ExtSRAM_SIZE		0x80000 	//��չ�ڴ��С��16*256KbIt,512KB	


//��������ڴ��
#define SRAMEX   0		//�ⲿ�ڴ��
#define SRAMCCM  1		//CCM�ڴ��(�˲���SRAM����CPU���Է���!!!)
#define SRAMBANK 	1	//����֧�ֵ�SRAM����.	


//mem2�ڴ�����趨.mem2���ڴ�ش����ⲿSRAM����
#define ERAM_BLOCK_SIZE			32  	  						//�ڴ���СΪ32�ֽ�
#define ERAM_TABLE_SIZE			ExtSRAM_SIZE/ERAM_BLOCK_SIZE 	//�ڴ���С
		 
//mem3�ڴ�����趨.mem3����CCM,���ڹ���CCM(�ر�ע��,�ⲿ��SRAM,��CPU���Է���!!)
#define CCM_BLOCK_SIZE			32  	  						//�ڴ���СΪ32�ֽ�
#define CCM_TABLE_SIZE			CCMSize/CCM_BLOCK_SIZE 			//�ڴ���С
		 


//�ڴ���������
struct _m_mallco_dev
{
	void (*init)(u8);					//��ʼ��
	u8 (*perused)(u8);		  	    	//�ڴ�ʹ����
	u8 	*membase[SRAMBANK];				//�ڴ�� ����SRAMBANK��������ڴ�
	u16 *memmap[SRAMBANK]; 				//�ڴ����״̬��
	u8  memrdy[SRAMBANK]; 				//�ڴ�����Ƿ����
};

extern struct _m_mallco_dev mallco_dev;	 //��mallco.c���涨��

////////////////////////////////////////////////////////////////////////////////

//�ڴ�����ʼ��  
//memx:�����ڴ��
void IniMalloc(u8 memx);
void myfree(u8 memx,void *ptr);  			//�ڴ��ͷ�(�ⲿ����)
void *mymalloc(u8 memx,u32 size);			//�ڴ����(�ⲿ����)
void *myrealloc(u8 memx,void *ptr,u32 size);//���·����ڴ�(�ⲿ����)
#endif













