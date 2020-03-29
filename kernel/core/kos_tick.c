#include "kos_tick.h"
#include "kos_cpu.h"
#include "kos_list.h"
#include "kos_sched.h"

static LIST_HEAD(kos_tick_head);

static void __tick_list_add(struct kos_proc *_proc, unsigned int _tick)
{
    struct list_head *curr_node;
    struct kos_proc *curr_proc = NULL;
    unsigned int prev_tick_wait = 0;
    unsigned int curr_tick_wait;

    _proc->tick_wait = _tick;

    list_for_each(curr_node, &kos_tick_head)
    {
	curr_proc = kos_list_entry(curr_node, struct kos_proc, tick_list);
	curr_tick_wait = prev_tick_wait + curr_proc->tick_wait;

	if (curr_tick_wait > _proc->tick_wait) {
	    break;
	}

	if (curr_tick_wait == _proc->tick_wait &&
	    _proc->priority < curr_proc->priority) {
	    break;
	}

	prev_tick_wait = curr_tick_wait;
    }

    _proc->tick_wait -= prev_tick_wait;

    if (curr_node != &kos_tick_head && curr_proc) {
	curr_proc->tick_wait -= _proc->tick_wait;
    }

    list_add_tail(curr_node, &_proc->tick_list);
}

int kos_tick_add(struct kos_proc *_proc, unsigned int _tick)
{
    if (unlikely(_proc == NULL)) {
	return -1;
    }

    if (proc_state_is_sleep(_proc)) {
	return -1;
    }

    __tick_list_add(_proc, _tick);

    proc_state_clr_and_set(_proc, KOS_PROC_READY, KOS_PROC_SLEEP);
    //_proc->state = KOS_PROC_SLEEP;

    return 0;
}

void kos_tick_poll(void)
{
    struct kos_proc *proc;
    struct list_head *curr_node;
    struct list_head *next_node;

    unsigned state = kos_cpu_enter_critical();

    if (list_empty(&kos_tick_head)) {
	kos_cpu_exit_critical(state);
	return;
    }

    proc = kos_list_first_entry(&kos_tick_head, struct kos_proc, tick_list);

    if (proc->tick_wait > 1) {
	proc->tick_wait--;
	kos_cpu_exit_critical(state);
	return;
    } else {
	proc->tick_wait = 0;
    }

    list_for_each_safe(curr_node, next_node, &kos_tick_head)
    {
	proc = kos_list_entry(curr_node, struct kos_proc, tick_list);
	if (proc->tick_wait > 0) {
	    break;
	}

	/* 将进程从tick_list 中移除 */
	kos_tick_remove(proc);
	/* 将进程加入就绪队列 */
	kos_rq_add(proc);
    }

    kos_cpu_exit_critical(state);
}

void kos_tick_remove(struct kos_proc *_proc)
{
    struct kos_proc *proc;

    /* 如果刚好时间到了则立马删除  */
    if (_proc->tick_wait == 0) {
	goto del_list;
    }

    /* 否则还在链表上的进程等待时间需要重新计算 */
    if (!list_empty(&_proc->tick_list) &&
	_proc->tick_list.next != &kos_tick_head) {
	proc = kos_list_first_entry(&_proc->tick_list, struct kos_proc, tick_list);
	proc->tick_wait += _proc->tick_wait;
    }
del_list:
    list_delete_init(&_proc->tick_list);
    /* 清除休眠状态  */
    proc_state_clr_sleep(_proc);
}

