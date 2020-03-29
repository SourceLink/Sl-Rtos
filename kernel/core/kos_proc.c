#include "kos_proc.h"
#include "kos_cpu.h"
#include "kos_sched.h"
#include "kos_sys.h"
#include "kos_tick.h"

static struct kos_proc *kos_proc_tab[KOS_CONFIG_PID_LIST];

static void _proc_exit_handle(void)
{
    unsigned int state = kos_cpu_enter_critical();
    unsigned int pid = kos_curr_proc->pid;

    kos_proc_tab[pid] = NULL;
    kos_rq_delete(kos_curr_proc);
    kos_curr_proc->stack_pointer = NULL;
    kos_curr_proc = NULL;

    kos_cpu_exit_critical(state);

    kos_sched();
}

int kos_proc_create(struct kos_proc *_proc, unsigned int *_stack_addr,
		    unsigned int _stack_size, unsigned int _prio,
		    proc_fun entry, void *_arg, const char *name)
{
    unsigned int state = 0;
    unsigned int i;
    unsigned int pid = 0;

    if (unlikely(_proc == NULL)) {
	return -1;
    }

    if (unlikely(_stack_addr == NULL)) {
	return -1;
    }

    if (unlikely(entry == NULL)) {
	return -1;
    }

    if (unlikely(_prio > KOS_CONFIG_LOWEST_PRIO)) {
	return -1;
    }

    state = kos_cpu_enter_critical();

    /* 1. 获取pid */
    for (i = 1; i <= KOS_CONFIG_PID_LIST; i++) {
	if (kos_proc_tab[i] == NULL) {
	    pid = i;
	    break;
	}
    }

    if (pid == 0) {
	kos_cpu_exit_critical(state);
	return -1;
    }

    kos_proc_tab[pid] = _proc;

    _proc->pid = pid;

    /* 2. 初始化进程堆栈 */
    _proc->stack_pointer = kos_proc_stack_init(
	(void *)entry, _arg, _proc_exit_handle, _stack_addr, _stack_size);

    /* 3. 初始化其他控制变量 */
    _proc->priority = _prio;
    _proc->proc_name = name;
    _proc->tick_wait = 0;

    /* 4. 初始化链表 */
    list_head_init(&_proc->slot_list);
    list_head_init(&_proc->tick_list);

    /* 5. 将进程加入就绪队列 */
    kos_rq_add(_proc);

    kos_cpu_exit_critical(state);

    /* 6. 执行调度 */
    kos_sched();

    return pid;
}

void kos_proc_delay(unsigned int _tick)
{
    unsigned int state = 0;

    if (unlikely(_tick == 0)) {
	return;
    }

    state = kos_cpu_enter_critical();

    kos_rq_delete(kos_curr_proc);

    /* 必须有个机制来处理延时时间 */
    kos_tick_add(kos_curr_proc, _tick);

    kos_cpu_exit_critical(state);

    kos_sched();
}
