#include "kos_sched.h"
#include "kos_proc.h"

struct kos_ready_queue kos_rq;

/*
*********************************************************************************************************
*    函 数 名: __find_first_bit
*    功能说明: 查找word中被置位的index
*    形    参: word： 被查找的字节
*    返 回 值: 范围是0～31
*********************************************************************************************************
*/
static unsigned int __find_first_bit(unsigned int word)
{
	int num = 0;

	if ((word & 0xffff) == 0) {
		num += 16;
		word >>= 16;
	}
	if ((word & 0xff) == 0) {
		num += 8;
		word >>= 8;
	}
	if ((word & 0xf) == 0) {
		num += 4;
		word >>= 4;
	}
	if ((word & 0x3) == 0) {
		num += 2;
		word >>= 2;
	}
	if ((word & 0x1) == 0)
		num += 1;

	return num;
}

static unsigned int find_first_bit(const unsigned int *_bitmap)
{
    unsigned int index = 0;
    unsigned int i;
    unsigned int bitmask = 0;

    for (i = 0; i < KOS_PRIO_TABLE_SIZE; i++) {
        if (_bitmap[i]) {
            bitmask = 1;
            break;
        }
    }

    if (bitmask) {
        index = KOS_PRIO_SLOT_SIZE * i;
    }

    return (__find_first_bit(_bitmap[i]) + index);
}

static void __register_prio(unsigned int _prio)
{
    kos_rq.prio_bitmap[KOS_PRIO_OFFSET(_prio)] |= KOS_PRIO_BIT(_prio);
}


static void __unregister_prio(unsigned int _prio)
{
    kos_rq.prio_bitmap[KOS_PRIO_OFFSET(_prio)] &= ~KOS_PRIO_BIT(_prio);
}


void register_prio(unsigned int _prio)
{
    __register_prio(_prio);
    if (_prio < kos_rq.highest_prio) {
        kos_rq.highest_prio = _prio;
    }
}


void kos_rq_init(void)
{
    unsigned int i;

    // 1. 初始化highest_prio
    kos_rq.highest_prio = KOS_CONFIG_LOWEST_PRIO;
    // 2. 初始化链表
    for (i = 0; i < KOS_READ_LIST_SIZE; i++) {
        list_head_init(&kos_rq.queue[i]);
    }
    // 3. 初始化bitmap
    for (i = 0; i < KOS_PRIO_TABLE_SIZE; i++) {
        kos_rq.prio_bitmap[i] = 0;
    }
}


void kos_rq_add_head(struct kos_proc *_proc)
{
    unsigned int prio = _proc->priority;
    struct list_head *rq_list = &kos_rq.queue[prio];

    if (list_empty(rq_list)) {
        register_prio(prio);
    }

    list_add(rq_list, &_proc->slot_list);
}


void kos_rq_add_tail(struct kos_proc *_proc)
{
    unsigned int prio = _proc->priority;
    struct list_head *rq_list = &kos_rq.queue[prio];

    if (list_empty(rq_list)) {
        register_prio(prio);
    }

    list_add_tail(rq_list, &_proc->slot_list);
}


unsigned int kos_rq_highest_prio(void)
{
    return find_first_bit(kos_rq.prio_bitmap) + 1;
}


void kos_rq_delete(struct kos_proc *_proc)
{
    unsigned int prio = _proc->priority;
    struct list_head *rq_list = &kos_rq.queue[prio]; 

    list_delete_init(&_proc->slot_list);

    if (list_empty(rq_list)) {
        __unregister_prio(prio);
    }

    if (prio == kos_rq.highest_prio) {
       kos_rq.highest_prio =  kos_rq_highest_prio();
    }
}

struct kos_proc *kos_rq_highest_ready_proc(void)
{
    struct kos_proc *rproc = NULL;

    struct list_head *node = kos_rq.queue[kos_rq.highest_prio].next;

    rproc = kos_list_entry(node, struct kos_proc, slot_list);

    return rproc;
}