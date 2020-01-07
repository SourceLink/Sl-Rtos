/*
************************************ (C) COPYRIGHT 2017 Sourcelink **************************************
 * File Name	: sl_os_c.c
 * Author		: Sourcelink 
 * Version		: V1.0
 * Date 		: 2017/2/4
 * Description	: 2017年2月4日: 完成进程堆栈初始化功能;
 ********************************************************************************************************
*/


#include "port.h"


void port_cpu_int_disable(void)
{
    __asm volatile ("cpsid i" : : : "memory");
}


void port_cpu_int_enable(void)
{
    __asm volatile ("cpsie i" : : : "memory");
}

unsigned int port_cpu_primask_get(void)
{
	unsigned int result;

	__asm volatile ("MRS %0, primask" : "=r" (result));

	return(result);
}

void port_cpu_primask_set(unsigned int priMask)
{
	__asm volatile ("MSR primask, %0" : : "r" (priMask) : "memory");
}


unsigned int port_enter_critical(void)
{
	unsigned int ret = port_cpu_primask_get();
	
	port_cpu_int_disable();
	
	return ret;
} 

void port_exit_critical(unsigned int _state)
{
	port_cpu_primask_set(_state);
}

/*
*********************************************************************************************************
*    函 数 名: sl_stack_init
*    功能说明: 进程堆栈初始化
*    形    参: process: 进程的函数指针   stack: 用户开辟的堆栈空间
*    返 回 值: 无
*********************************************************************************************************
*/
unsigned int *port_proc_stack_init(void *entry, void *arg, void *exit, unsigned int *stack_addr, size_t stack_szie)
{ 
    unsigned int    *user_stack;

    user_stack      = &stack_addr[stack_szie -1];
    user_stack      = (unsigned int *)((unsigned int)(user_stack) & 0xFFFFFFF8ul);
    
    *(--user_stack) = (unsigned int)0x01000000ul;                          //xPSR
    *(--user_stack) = (unsigned int)entry;                                 // Entry Point(PC)
    *(--user_stack) = (unsigned int)exit;                                  // R14 (LR)
    *(--user_stack) = (unsigned int)0x12121212ul;                          // R12
    *(--user_stack) = (unsigned int)0x03030303ul;                          // R3
    *(--user_stack) = (unsigned int)0x02020202ul;                          // R2
    *(--user_stack) = (unsigned int)0x01010101ul;                          // R1
    *(--user_stack) = (unsigned int)arg;                                   // R0
    
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
