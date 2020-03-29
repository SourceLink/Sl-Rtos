#ifndef _KOS_MUTEX_H_
#define _KOS_MUTEX_H_

#include "kos_block.h"
#include "kos_sys.h"

typedef enum kos_mutex_attr {
    MUTEX_RECURSIVE = 1, 
    MUTEX_NON_RECURSIVE = ~MUTEX_RECURSIVE
} kos_mutex_attr_t;

struct kos_mutex {
	struct block_object block_obj;
	struct kos_proc *owner;
	unsigned int owner_orig_prio;
	unsigned char lock_nesting;
	kos_mutex_attr_t mutex_attr;
};

int kos_mutex_init(struct kos_mutex *_mutex, kos_mutex_attr_t _attr);
int kos_mutex_destory(struct kos_mutex *_mutex);
int kos_mutex_lock(struct kos_mutex *_mutex);
int kos_mutex_trylock(struct kos_mutex *_mutex);
int kos_mutex_unlock(struct kos_mutex *_mutex);


#endif
