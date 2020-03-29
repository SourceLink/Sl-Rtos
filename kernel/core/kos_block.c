#include "kos_block.h"
#include "kos_sched.h"

/* 将进程插入到阻塞对象中 */
static void __block_list_add(struct block_object *_block_obj,
			     struct kos_proc *_new_proc)
{
    struct list_head *pend_list = &_block_obj->list;
    struct list_head *curr_list = NULL;
    struct kos_proc *curr_proc = NULL;

    /* 从阻塞链表上遍历, 找到合适的位置插入进程 */
    list_for_each(curr_list, pend_list) {
	curr_proc = kos_list_entry(curr_list, struct kos_proc, slot_list);
	if (_new_proc->priority < curr_proc->priority) {
	    break;
	}
    }

    list_add_tail(curr_list, &_new_proc->slot_list);

    /* 记住自己挂载在那个阻塞对象上 */
    _new_proc->block_obj = _block_obj;
}

static void __block_list_remove(struct kos_proc *_proc)
{
    list_delete_init(&_proc->slot_list);
    _proc->block_obj = NULL;
}

void kos_block_obj_init(struct block_object *_block_obj, block_type_t _type)
{
    _block_obj->type = _type;
    list_head_init(&_block_obj->list);
}

void kos_block_obj_deinit(struct block_object *_block_obj)
{
    _block_obj->type = BLOCK_TYPE_NONE;
    list_head_init(&_block_obj->list);
}

unsigned int kos_block_list_is_empty(struct block_object *_block_obj)
{
    return (list_empty(&_block_obj->list));
}

void kos_block_proc_wakeup(struct kos_proc *_proc)
{
    /*
      如果现在一个信号等待时间到了, 先将该信号从阻塞列表移除
      再从tick链表上移除
    */
    if (proc_state_is_sleep(_proc))
	kos_tick_remove(_proc);

    /* 这里可做判断，进程是否被挂block
       (阻塞中休眠，
       进程的状态也改变成休眠，可以使用如下办法判断进程是否被阻塞过)
    */
    if (_proc->block_obj) {
	__block_list_remove(_proc);
    }

    kos_rq_add(_proc);
}

void kos_block_proc(struct block_object *_block_obj, struct kos_proc *_proc,
		    size_t _timeout)
{
    kos_rq_delete(_proc);

    __block_list_add(_block_obj, _proc);

    /* 需要更改进程状态 */
    proc_state_clr_and_set(_proc, KOS_PROC_READY, _block_obj->type);

    if (_timeout != KOS_TIME_FOREVER) {
	kos_tick_add(_proc, _timeout);
    }
}

void kos_block_obj_wakeup_first(struct block_object *_block_obj)
{
    struct kos_proc *fproc = NULL;

    fproc = kos_list_first_entry(&_block_obj->list, struct kos_proc, slot_list);

    kos_block_proc_wakeup(fproc);
}

void kos_block_obj_wakeup_all(struct block_object *_block_obj)
{
    struct list_head *curr_list = NULL;
    struct list_head *next_list = NULL;
    struct kos_proc *curr_proc = NULL;

    /* 将block_obj链表上的进程都迭代出来 */
    list_for_each_safe(curr_list, next_list, &_block_obj->list) {
	curr_proc = kos_list_entry(curr_list, struct kos_proc, slot_list);
	kos_block_proc_wakeup(curr_proc);
    }
}

