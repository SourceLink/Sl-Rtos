#ifndef _KOS_CPU_H_
#define _KOS_CPU_H_


void kos_cpu_int_disable(void);
void kos_cpu_int_enable(void);
unsigned int kos_cpu_primask_get(void);
void kos_cpu_primask_set(unsigned int priMask);
void kos_cpu_sched_start(void);
void kos_cpu_ctxsw(void);
unsigned int kos_cpu_enter_critical(void);
void kos_cpu_exit_critical(unsigned int _state);
unsigned int *kos_proc_stack_init(void *entry, void *arg, void *exit, unsigned int *stack_addr, unsigned int stack_szie);


#endif