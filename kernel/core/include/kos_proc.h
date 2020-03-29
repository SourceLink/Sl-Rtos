#ifndef _KOS_PROC_H_
#define _KOS_PROC_H_

#include "kos_list.h"
#include "kos_compiler.h"
#include "kos_config.h"

typedef enum kos_proc_state {
    KOS_PROC_NONE = 0,
    KOS_PROC_READY = (1 << 1),
    KOS_PROC_SLEEP = (1 << 2),
    KOS_PROC_MUTEX = (1 << 3),
} kos_proc_state_t;

struct kos_proc {
    unsigned int *stack_pointer;
    struct list_head slot_list;
    unsigned int  priority;
    kos_proc_state_t  state;
    const char *proc_name;
    struct block_object *block_obj;
    struct list_head tick_list;
    unsigned int     tick_wait;
    unsigned int pid;
};


typedef void *(*proc_fun)(void *arg);


int kos_proc_create(struct kos_proc *_proc, 
                            unsigned int *_stack_addr, 
                            unsigned int _stack_size,
                            unsigned int _prio,
                            proc_fun    entry,
                            void *_arg,
                            const char *name);

void kos_proc_delay(unsigned int _tick);


#define PROC_STATE_MASK (KOS_PROC_READY | KOS_PROC_SLEEP | KOS_PROC_MUTEX)


static inline void proc_state_clr_and_set(struct kos_proc *_proc, unsigned int _clr, unsigned int _set)
{
    _proc->state &= ~(_clr & PROC_STATE_MASK);
    _proc->state |= (_set & PROC_STATE_MASK);
}


static inline unsigned int proc_state_is_ready(struct kos_proc *_proc)
{
    return (_proc->state & KOS_PROC_READY);
}


static inline unsigned int proc_state_is_sleep(struct kos_proc *_proc)
{
    return (_proc->state & KOS_PROC_SLEEP);
}

static inline void proc_state_set_sleep(struct kos_proc *_proc)
{
    _proc->state |= KOS_PROC_SLEEP;
}

static inline void proc_state_clr_sleep(struct kos_proc *_proc)
{
    _proc->state &= ~KOS_PROC_SLEEP;
}



#endif
