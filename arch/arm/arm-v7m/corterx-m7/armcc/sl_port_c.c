/*
************************************ (C) COPYRIGHT 2017 Sourcelink **************************************
 * File Name	: sl_os_c.c
 * Author		: Sourcelink 
 * Version		: V1.0
 * Date 		: 2017/2/4
 * Description	: 2017年2月4日: 完成进程堆栈初始化功能;
 ********************************************************************************************************
*/


#include "sl_port.h"


volatile unsigned int sl_interrput_sum;

/*
*********************************************************************************************************
*    函 数 名: sl_stack_init
*    功能说明: 进程堆栈初始化
*    形    参: process: 进程的函数指针   stack: 用户开辟的堆栈空间
*    返 回 值: 无
*********************************************************************************************************
*/
unsigned int *sl_stack_init(sl_process process,unsigned int *stack)
{
    unsigned int    *user_stack;
    user_stack      = stack;
    user_stack      = (unsigned int *)((unsigned int)(user_stack) & 0xFFFFFFF8ul);
    
    *(--user_stack) = (unsigned int)0x01000000ul;                          //xPSR
    *(--user_stack) = (unsigned int)process;                               // Entry Point
    *(--user_stack) = (unsigned int)0xFFFFFFFEul;                          // R14 (LR)
    *(--user_stack) = (unsigned int)0x12121212ul;                          // R12
    *(--user_stack) = (unsigned int)0x03030303ul;                          // R3
    *(--user_stack) = (unsigned int)0x02020202ul;                          // R2
    *(--user_stack) = (unsigned int)0x01010101ul;                          // R1
    *(--user_stack) = (unsigned int)0x00000000ul;                          // R0
    
    *(--user_stack) = (unsigned int)0x11111111ul;                          // R11
    *(--user_stack) = (unsigned int)0x10101010ul;                          // R10
    *(--user_stack) = (unsigned int)0x09090909ul;                          // R9
    *(--user_stack) = (unsigned int)0x08080808ul;                          // R8
    *(--user_stack) = (unsigned int)0x07070707ul;                          // R7
    *(--user_stack) = (unsigned int)0x06060606ul;                          // R6
    *(--user_stack) = (unsigned int)0x05050505ul;                          // R5
    *(--user_stack) = (unsigned int)0x04040404ul;                          // R4
    
    return user_stack;
}
