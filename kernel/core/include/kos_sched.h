#ifndef _KOS_SCHED_H_
#define _KOS_SCHED_H_

#include "kos_config.h"
#include "kos_list.h"

#define KOS_PRIO_SLOT_SIZE      32
#define KOS_PRIO_TABLE_SIZE     ((KOS_CONFIG_LOWEST_PRIO + KOS_PRIO_SLOT_SIZE - 1) / KOS_PRIO_SLOT_SIZE)
#define KOS_READ_LIST_SIZE      (KOS_CONFIG_LOWEST_PRIO + 1)

#define KOS_PRIO_OFFSET(prio)  (prio / KOS_PRIO_SLOT_SIZE)
#define KOS_PRIO_BIT(prio) (1 << ((prio & (KOS_PRIO_SLOT_SIZE - 1)) - 1))

struct kos_ready_queue {
    unsigned int prio_bitmap[KOS_PRIO_TABLE_SIZE];
    struct list_head queue[KOS_READ_LIST_SIZE];
    unsigned int highest_prio;
};


#endif