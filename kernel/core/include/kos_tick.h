#ifndef _KOS_TICK_H_
#define _KOS_TICK_H_

#include "kos_proc.h"


int kos_tick_add(struct kos_proc *_proc, unsigned int _tick);
void kos_tick_remove(struct kos_proc *_proc);
void kos_tick_poll(void);


#endif
