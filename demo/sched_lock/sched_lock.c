#include "board.h"
#include "kos_sys.h"
#include "kos_sched.h"


#define PROC1_STACK_SIZE	128
#define PROC2_STACK_SIZE	128

struct kos_proc proc1;
struct kos_proc proc2;

unsigned int proc1_stack[PROC1_STACK_SIZE];
unsigned int proc2_stack[PROC2_STACK_SIZE];


void *proc1_fun(void *_arg)
{

	while (1) {
		kos_sched_lock();
		printf("proc 1\r\n");
		kos_sched_unlock();
		kos_proc_delay(500);
	}
	
		

    return (void*)1; // 寄存器r3
	/*
		进程返回以后产生了异常


		函数返回后汇编指令一般都是使用bx lr

		在进程退出后，我们告诉它返回到哪

		所以必须在lr上做处理
	*/
}


void *proc2_fun(void *_arg)
{
	while(1) {
		printf("proc 2\r\n");
		kos_proc_delay(100);
	}
	

    return NULL; 
}


int main(void)
{
	board_init();

	printf("system init ok\r\n");

	kos_sys_init();

	kos_proc_create(&proc1, proc1_stack, PROC1_STACK_SIZE, 2, proc1_fun, NULL, "proc1");
	kos_proc_create(&proc2, proc2_stack, PROC2_STACK_SIZE, 1, proc2_fun, NULL, "proc2");

	kos_sys_start();

    return 0;
}


void SysTick_Handler(void)
{
	systick_handle();
}