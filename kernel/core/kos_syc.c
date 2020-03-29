#include "kos_compiler.h"
#include "kos_config.h"
#include "kos_cpu.h"
#include "kos_proc.h"
#include "kos_sched.h"
#include "kos_sys.h"
#include "kos_tick.h"

struct kos_proc *kos_curr_proc = NULL;
struct kos_proc *kos_ready_proc = NULL;
unsigned int kos_running = 0;
unsigned char kos_irq_nesting = 0;

#define IDLE__PROC_STACK_SIZE 64
static struct kos_proc idle_proc;
static unsigned int idle_proc_stack[IDLE__PROC_STACK_SIZE];
static long int idle_proc_count = 0;

void *kos_proc_idle(void *_arg)
{
    unsigned int state = 0;
    while (1) {
	state = kos_cpu_enter_critical();
	idle_proc_count++;
	kos_cpu_exit_critical(state);
    }
    // return NULL;
}

static void _idle_proc_create(void)
{
    kos_proc_create(&idle_proc, idle_proc_stack, IDLE__PROC_STACK_SIZE,
		    KOS_CONFIG_LOWEST_PRIO, kos_proc_idle, NULL, "idle_proc");
}

unsigned int kos_sys_is_running(void)
{
    return (kos_running == 1);
}

unsigned int kos_sysy_is_inirq(void)
{
    return (kos_irq_nesting > 0);
}

void kos_sys_init(void)
{
    /* 1. 初始化就绪队列 */
    kos_rq_init();
    /* 2. 初始化启动标志 */
    kos_running = 0;
    /* 3. 空闲进程的创建 */
    _idle_proc_create();
    /* 4. 中断嵌套 */
    kos_irq_nesting = 0;
}

void kos_sys_start(void)
{
    kos_ready_proc = kos_rq_highest_ready_proc();
    kos_cpu_sched_start();
}

void kos_sys_enter_irq(void)
{
    unsigned int state = kos_cpu_enter_critical();
    if (kos_sys_is_running() && kos_irq_nesting < 255) {
	kos_irq_nesting++;
    }
    kos_cpu_exit_critical(state);
}

void kos_sys_exit_irq(void)
{
    unsigned int state;

    if (kos_sys_is_running()) {
	state = kos_cpu_enter_critical();
	if (kos_sysy_is_inirq())
	    kos_irq_nesting--;

	kos_cpu_exit_critical(state);

	kos_sched();
    }
}

void systick_handle(void)
{
    kos_sys_enter_irq();
    kos_tick_poll();
    kos_sys_exit_irq();
}
