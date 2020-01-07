#ifndef _KOS_SYS_H_
#define _KOS_SYS_H_

#include "kos_proc.h"


extern struct kos_proc *kos_curr_proc;
extern struct kos_proc *kos_ready_proc;
extern unsigned int kos_running;


unsigned int kos_sys_is_running(void);

void kos_sys_init(void);
void kos_sys_start(void);


#endif