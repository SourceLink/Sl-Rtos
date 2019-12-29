#include "kos_sys.h"

struct kos_proc *kos_curr_proc = NULL;
struct kos_proc *kos_ready_proc = NULL;
unsigned int kos_running = 0;


unsigned int kos_sys_is_running(void)
{
    return (kos_running == 1);
}