#include "kos_cpu.h"
#include "port.h"

void kos_cpu_int_disable(void)
{
    port_cpu_int_disable();
}


void kos_cpu_int_enable(void)
{
    port_cpu_int_enable();
}

unsigned int kos_cpu_primask_get(void)
{

  return port_cpu_primask_get();
}

void kos_cpu_primask_set(unsigned int priMask)
{
    port_cpu_primask_set(priMask);
}


void kos_cpu_sched_start(void)
{
    port_os_start();
}


void kos_cpu_ctxsw(void)
{
    port_os_ctxsw();
}


unsigned int kos_cpu_enter_critical(void)
{
    return port_enter_critical();
}

void kos_cpu_exit_critical(unsigned int _state)
{
    port_exit_critical(_state);
}


unsigned int *kos_proc_stack_init(void *entry, void *arg, void *exit, unsigned int *stack_addr, size_t stack_szie)
{
    return port_proc_stack_init(entry, arg, exit, stack_addr, stack_szie);
}
