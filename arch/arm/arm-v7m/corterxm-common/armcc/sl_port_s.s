;/*********************** (C) COPYRIGHT 2017 Sourcelink ********************************
;* File Name	: sl_os_s.s
;* Author		: Sourcelink 
;* Version		: V1.0
;* Date 		: 2017/2/4
;* Description	: 2017年2月4日: 完成任务级上下文切换功能;
;*				  2017年2月15日: 发现系统有HardFault_Handler,经过长时间的debug每次在SysTick_Handler
;*								 中对任务进行切换会产生fault,但是单步执行时发现只要执行完SysTick_Handler
;*								 中的函数在执行上下文切换没有出现问题，怀疑是两个中断优先级抢占出现问题
;*								 pend_sv的优先级没有设置为最低，经查看汇编代码发现优先级设置出错。
;***************************************************************************************/

  	;IMPORT  SL_OS_MSPBase											; External references
  	IMPORT  sl_current_process
  	IMPORT  sl_ready_process
	IMPORT  sl_interrput_sum
	IMPORT  sl_running_flag

  	EXPORT SL_OS_START
  	EXPORT PendSV_Handler
  	EXPORT SL_OS_CTXSW
  	EXPORT SL_ENTER_CRITICAL
  	EXPORT SL_EXIT_CRITICAL



NVIC_INT_CTRL   	EQU     0xE000ED04                              ; Interrupt control state register.
NVIC_SYSPRI3    	EQU     0xE000ED20                              ; System priority register (priority 14).
NVIC_PENDSV_PRI 	EQU     0xFFFF0000                              ; PendSV priority value (lowest).
NVIC_PENDSVSET  	EQU     0x10000000                              ; bit[28]:Value to trigger PendSV exception.

  	PRESERVE8 														; 堆栈8字节对齐
  	AREA |.text|, CODE, READONLY
  	THUMB



;/***************************************************************************************
;* 函数名称: SL_ENTER_CRITICAL
;*
;* 功能描述: 进入临界区 
;*            
;* 参    数: None
;*
;* 返 回 值: None
;*****************************************************************************************/ 

SL_ENTER_CRITICAL
	CPSID	I														; Disable all the interrputs

	PUSH	{R4,R5} 

	LDR		R4, =sl_interrput_sum			
	LDRB	R5, [R4]
	ADD		R5, R5, #1
	STRB 	R5, [R4]												; sl_interrput_sum++

	POP 	{R4,R5}

	BX		LR



;/***************************************************************************************
;* 函数名称: SL_EXIT_CRITICAL
;*
;* 功能描述: 退出临界区 
;*            
;* 参    数: None
;*
;* 返 回 值: None
;*****************************************************************************************/

SL_EXIT_CRITICAL
	PUSH	{R4,R5}

	LDR		R4, =sl_interrput_sum
	LDRB	R5, [R4]
	SUB		R5, R5, #1
	STRB	R5, [R4]

	MOV		R4, #0
	CMP		R5, #0

	MSREQ	PRIMASK, R4												; if sl_interrput_sum = 0 enable interrupt  PRIMASK 写1屏蔽所有可配置优先级中断
																	; MSREQ = MSR EQ    EQ:标志位z=1																
	POP 	{R4, R5}																
	
	BX		LR
	

;/**************************************************************************************
;* 函数名称: SL_OS_CtxSw
;*
;* 功能描述: 任务级上下文切换         
;*
;* 参    数: None
;*
;* 返 回 值: None
;***************************************************************************************/

SL_OS_CTXSW
	PUSH	{R4, R5}

    LDR     R4, =NVIC_INT_CTRL
    LDR     R5, =NVIC_PENDSVSET
    STR     R5, [R4]

	POP		{R4, R5}
	
    BX      LR                                                  	; Enable interrupts at processor level


;/**************************************************************************************
;* 函数名称: SL_OS_Start
;*
;* 功能描述: 启动OS         
;*
;* 参    数: None
;*
;* 返 回 值: None
;***************************************************************************************/

SL_OS_START
    LDR     R4, =NVIC_SYSPRI3                                  		; Set the PendSV exception priority
    LDR     R5, =NVIC_PENDSV_PRI
    STR    R5, [R4]

    MOVS    R4, #0                                              	; Set the PSP to 0 for initial context switch call
    MSR     PSP, R4

	LDR     R4, =sl_running_flag                         			; 启动切换
	MOV     R5, #1
	STRB    R5, [R4] 											

																	; 切换到第一个task
    LDR     R4, =NVIC_INT_CTRL                                  	; Trigger the PendSV exception (causes context switch)
    LDR     R5, =NVIC_PENDSVSET
    STR     R5, [R4]

    CPSIE   I                                                   	; Enable interrupts at processor level

SL_OS_WHILE
    B       SL_OS_WHILE                                         	; Should never get here
    
    
;/**************************************************************************************
;* 函数名称: OSPendSV
;*
;* 功能描述: OSPendSV is used to cause a context switch.
;*
;* 参    数: None
;*
;* 返 回 值: None
;***************************************************************************************/

PendSV_Handler
    CPSID   I                                                   	; Prevent interruption during context switch
    MRS     R0, PSP                                             	; PSP is process stack pointer
    CBZ     R0, SWITCH_PROCESS                     					; Skip register save the first time if R0=0 bl OS_CPU_PendSVHandler_nosave
   
    SUBS    R0, R0, #0x20                                       	; Save remaining regs r4-11 on process stack
    STM     R0, {R4-R11}

    LDR     R1, =sl_current_process                                 ; sl_current_process->stack_pointer = SP;
    LDR     R1, [R1]         
    STR     R0, [R1]                                          		; R0 is SP of process being switched out

                                                                	; At this point, entire context of process has been saved
SWITCH_PROCESS
;	 LDR     R0, =sl_current_priority                                ; sl_current_priority = sl_high_priority;
;    LDR     R1, =sl_high_priority
;    LDRB    R2, [R1]
;    STRB    R2, [R0]
	
	
    LDR     R0, =sl_current_process                                 ; sl_current_process  = sl_ready_process;
    LDR     R1, =sl_ready_process
    LDR     R2, [R1]												; R2 = &tcb
    STR     R2, [R0]												; sl_current_process = &tcb

    LDR     R0, [R2]                                            	; R0 is new process SP;
  
    LDM     R0, {R4-R11}                                        	; Restore r4-11 from new process stack
    ADDS    R0, R0, #0x20
            
    MSR     PSP, R0                                             	; Load PSP with new process SP
    ORR     LR, LR, #0x04                                       	; Ensure exception return uses process stack
    
    CPSIE   I
    BX      LR                                                  	; Exception return will restore remaining context
  
    END
