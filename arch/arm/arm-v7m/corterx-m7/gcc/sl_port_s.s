;/*********************** (C) COPYRIGHT 2019 Sourcelink ********************************
;* File Name	: sl_os_s.s
;* Author		: Sourcelink 
;* Version		: V1.0
;* Date 		: 2019/10/02
;* Description	: 
;***************************************************************************************/

.extern sl_current_process
.extern sl_ready_process
.extern sl_interrput_sum
.extern sl_running_flag

.global port_os_start
.global PendSV_Handler
.global port_os_ctxsw
.global port_enter_critical
.global port_exit_critical

.equ NVIC_INT_CTRL,   0xE000ED04                                /* Interrupt control state register. */
.equ NVIC_SYSPRI3,    0xE000ED20                                /* System priority register (priority 14). */
.equ NVIC_PENDSV_PRI, 0x00FF0000                                /* PendSV priority value (lowest). */
.equ NVIC_PENDSVSET,  0x10000000                                /* bit[28]:Value to trigger PendSV exception. */

.text
.align 8
.thumb
.syntax unified

;/***************************************************************************************
;* 函数名称: port_enter_critical
;*
;* 功能描述: 进入临界区 
;*            
;* 参    数: None
;*
;* 返 回 值: None
;*****************************************************************************************/ 
.thumb_func
.type port_enter_critical, %function
port_enter_critical:
	cpsid	i														/* Disable all the interrputs */

	push	{r4,r5} 

	ldr		r4, =sl_interrput_sum			
	ldrb	r5, [R4]
	add		r5, r5, #1
	strb 	r5, [r4]												/* sl_interrput_sum++ */

	pop 	{r4,r5}

	bx		lr



;/***************************************************************************************
;* 函数名称: port_exit_critical
;*
;* 功能描述: 退出临界区 
;*            
;* 参    数: None
;*
;* 返 回 值: None
;*****************************************************************************************/
.thumb_func
.type port_exit_critical, %function
port_exit_critical: 
	push	{r4,r5}

	ldr		r4, =sl_interrput_sum
	ldrb	r5, [r4]
	sub		r5, r5, #1
	strb	r5, [r4]

    mov     r4, #0

    cmp		r5, #0
	msreq	PRIMASK, r4											/* if sl_interrput_sum = 0 enable interrupt  PRIMASK 写1屏蔽所有可配置优先级中断 */	
																/* MSREQ = MSR EQ    EQ:标志位z=1 */															
	pop 	{r4, r5}																
	
	bx		lr
	

;/**************************************************************************************
;* 函数名称: port_os_ctxsw
;*
;* 功能描述: 任务级上下文切换         
;*
;* 参    数: None
;*
;* 返 回 值: None
;***************************************************************************************/
.thumb_func
.type port_os_ctxsw, %function
port_os_ctxsw:
	push	{r4, r5}
    ldr     r4, =NVIC_INT_CTRL
    ldr     r5, =NVIC_PENDSVSET
    str     r5, [r4]
	pop		{r4, r5}
	
    bx      lr                                                  	/* Enable interrupts at processor level */


;/**************************************************************************************
;* 函数名称: port_os_start
;*
;* 功能描述: 启动OS         
;*
;* 参    数: None
;*
;* 返 回 值: None
;***************************************************************************************/
.thumb_func
.type port_os_start, %function
port_os_start:
	cpsid   i
    ldr     r4, =NVIC_SYSPRI3                                  		/* Set the PendSV exception priority */
    ldr     r5, =NVIC_PENDSV_PRI
    str     r5, [r4]

    movs    r4, #0                                              	/* Set the PSP to 0 for initial context switch call */
    msr     psp, r4

	ldr     r4, =sl_running_flag                         			/*  启动切换 */
	mov     r5, #1
	strb    r5, [r4] 											

																	/* 切换到第一个task */
    ldr     r4, =NVIC_INT_CTRL                                  	/* Trigger the PendSV exception (causes context switch) */
    ldr     r5, =NVIC_PENDSVSET
    str     r5, [r4]

    cpsie   i                                                 	/* Enable interrupts at processor level */

os_while:
    b       os_while                                         	/* Should never get here */
    
    
;/**************************************************************************************
;* 函数名称: OSPendSV
;*
;* 功能描述: OSPendSV is used to cause a context switch.
;*
;* 参    数: None
;*
;* 返 回 值: None
;***************************************************************************************/
.thumb_func
.type PendSV_Handler, %function
PendSV_Handler:
    cpsid   i                                                      	/* Prevent interruption during context switch */
    mrs     r0, psp                                             	/* PSP is process stack pointer */
    cbz     r0, SWITCH_PROCESS                     					/* Skip register save the first time if R0=0 bl OS_CPU_PendSVHandler_nosave */
   
    subs    r0, r0, #0x20                                       	/* Save remaining regs r4-11 on process stack */
    stm     r0, {r4-r11}

    ldr     r1, =sl_current_process                                 /* sl_current_process->stack_pointer = SP; */
    ldr     r1, [r1]    
    str     r0, [r1]                                          		/* R0 is SP of process being switched out */
                                                            	    /* At this point, entire context of process has been saved */
SWITCH_PROCESS:	
    ldr     r0, =sl_current_process                                 /* sl_current_process  = sl_ready_process; */
    ldr     r1, =sl_ready_process
    ldr     r2, [r1]												/*  R2 = &tcb */
    str     r2, [r0]												/* sl_current_process = &tcb */

    ldr     r0, [r2]                                            	/* R0 is new process SP; */
  
    ldm     r0, {r4-r11}                                        	/* Restore r4-11 from new process stack */
    adds    r0, r0, #0x20
            
    msr     psp, r0                                             	/* Load PSP with new process SP */
    orr     lr, lr, #0x04                                       	/* Ensure exception return uses process stack */
    
    cpsie   i
    bx      lr                                                  	/* Exception return will restore remaining context */

.end
