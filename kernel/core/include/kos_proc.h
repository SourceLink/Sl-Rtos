#ifndef _KOS_PROC_H_
#define _KOS_PROC_H_

#include "kos_list.h"

struct kos_proc {
    unsigned int *stack_pointer;
    struct list_head slot_list;
    unsigned int  priority;
};

#endif