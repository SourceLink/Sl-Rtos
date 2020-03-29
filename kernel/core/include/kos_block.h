#ifndef _KOS_BLOCK_H_
#define _KOS_BLOCK_H_

#include "kos_compiler.h"
#include "kos_list.h"
#include "kos_proc.h"
#include "kos_tick.h"

typedef enum block_type {
    BLOCK_TYPE_NONE = 0x0000,
    BLOCK_TYPE_MUTEX = KOS_PROC_MUTEX,
} block_type_t;

struct block_object {
    block_type_t type;
    struct list_head list;
};

#define DEFINE_BLOCK_OBJ(name, t)                                              \
    struct block_object name = {.type = t, .list = LIST_HEAD_INIT((name).list)}

#define BLOCK_OBJ_INIT(name, t)                                                \
    {                                                                          \
	.type = t, .list = LIST_HEAD_INIT((name).list)                         \
    }

void kos_block_obj_init(struct block_object *_block_obj,
			block_type_t _type);
void kos_block_obj_deinit(struct block_object *_block_obj);
unsigned int kos_block_list_is_empty(struct block_object *_block_obj);

void kos_block_proc(struct block_object *_block_obj, struct kos_proc *_proc,
		    size_t _timeout);
void kos_block_proc_wakeup(struct kos_proc *_proc);

void kos_block_obj_wakeup_first(struct block_object *_block_obj);
void kos_block_obj_wakeup_all(struct block_object *_block_obj);

#endif
