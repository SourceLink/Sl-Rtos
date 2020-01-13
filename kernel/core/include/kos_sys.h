#ifndef _KOS_SYS_H_
#define _KOS_SYS_H_

#include "kos_proc.h"


extern struct kos_proc *kos_curr_proc;
extern struct kos_proc *kos_ready_proc;
extern unsigned int kos_running;


unsigned int kos_sys_is_running(void);
unsigned int kos_sysy_is_inirq(void);

void kos_sys_init(void);
void kos_sys_start(void);
void kos_sys_enter_irq(void);
void kos_sys_exit_irq(void);

void systick_handle(void);

#endif