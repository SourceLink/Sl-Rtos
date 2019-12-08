#ifndef _PORT_C_H_
#define _PORT_C_H_

#include <stdio.h>

/************************************************************************************************/

extern volatile unsigned int sl_interrput_sum;
extern struct kos_pcb *sl_current_process; 
extern struct kos_pcb *sl_ready_process;

/************************************************************************************************/

unsigned int *port_proc_stack_init(void *entry, void *arg, unsigned int *stack_addr, size_t stack_szie);
void port_cpu_int_disable(void);
void port_cpu_int_enable(void);
unsigned int port_cpu_primask_get(void);
void port_cpu_primask_set(unsigned int priMask);
void port_os_start(void);
void port_os_ctxsw(void);
unsigned int port_enter_critical(void);
void port_exit_critical(unsigned int _state);





#endif

