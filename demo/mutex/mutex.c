#include "board.h"
#include "kos_sched.h"
#include "kos_sys.h"
#include "kos_mutex.h"

#define PROC1_STACK_SIZE 128
#define PROC2_STACK_SIZE 128

struct kos_proc proc1;
struct kos_proc proc2;

unsigned int proc1_stack[PROC1_STACK_SIZE];
unsigned int proc2_stack[PROC2_STACK_SIZE];

unsigned int g_val = 0;
struct kos_mutex mutex_lock;

void *proc1_fun(void *_arg)
{
    /* int i = 0; */

    while (1) {
	/* 在该模拟情况下除了g_val++操作也没有其他操作且
	 * 它的指令周期比较短，因为系统切换调度的周期为1ms
	 * 所以在不加锁的情况并没有出现计数混乱问题
	 * */
	/* 这之前还有一些其他操作的时候， 假如在切换到进程2之前
	 * 的g_val加操作已经将这个变量的值已经存到了寄存器中，等待做加法操作;
	 * */
	/* mutex_lock */
	kos_mutex_lock(&mutex_lock);
	{
		kos_mutex_lock(&mutex_lock);
		g_val++;
		/* mutex_unlock */
		kos_mutex_unlock(&mutex_lock);
	}
	kos_mutex_unlock(&mutex_lock);
	printf("%s running\r\n", kos_curr_proc->proc_name);
	kos_proc_delay(50);
    }

    return NULL;
}

void *proc2_fun(void *_arg)
{
    /* int i = 0; */
    while (1) {
	/* mutex_lock */
	kos_mutex_lock(&mutex_lock);
	g_val++;
	/* mutex_unlock */
	kos_mutex_unlock(&mutex_lock);
	printf("%s running\r\n", kos_curr_proc->proc_name);
	kos_proc_delay(300);
    }

    return NULL;
}

int main(void)
{
    board_init();

    kos_sys_init();

    printf("hello sourcelink\r\n");

    kos_mutex_init(&mutex_lock, MUTEX_RECURSIVE);

    kos_proc_create(&proc1, proc1_stack, PROC1_STACK_SIZE, 2, proc1_fun, NULL,
		    "proc1");
    kos_proc_create(&proc2, proc2_stack, PROC2_STACK_SIZE, 1, proc2_fun, NULL,
		    "proc2");

    kos_sys_start();

    return 0;
}

void SysTick_Handler(void)
{
    systick_handle();
}
