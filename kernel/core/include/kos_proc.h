#ifndef _KOS_PROC_H_
#define _KOS_PROC_H_

#include "kos_list.h"
#include "kos_compiler.h"
#include "kos_config.h"

typedef enum kos_proc_state {
    KOS_PROC_READY = 0,
    KOS_PROC_SLEEP,
}kos_proc_state_t;

struct kos_proc {
    unsigned int *stack_pointer;
    
    struct list_head slot_list;
    
    unsigned int  priority;
    
    kos_proc_state_t  state;
    
    const char *proc_name;
    
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

#endif