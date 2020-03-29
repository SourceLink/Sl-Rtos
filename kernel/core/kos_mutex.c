#include "kos_mutex.h"
#include "kos_cpu.h"
#include "kos_sched.h"

static unsigned char owner_is_self(struct kos_proc *_proc)
{
    return (_proc == kos_curr_proc);
}

static void __mutex_new_owner_mark(struct kos_mutex *_mutex, struct kos_proc *_proc)
{
    _mutex->owner = _proc;
    _mutex->owner_orig_prio = _proc->priority;
    _mutex->lock_nesting = 1;
}

static void __mutex_owner_prio_up(struct kos_mutex *_mutex, unsigned char _new_prio)
{
    _mutex->owner->priority = _new_prio;
}

static void __mutex_old_owner_release(struct kos_mutex *_mutex)
{
    _mutex->owner = NULL;
    _mutex->owner->priority = _mutex->owner_orig_prio;
}


static int __mutex_trylock(struct kos_mutex *_mutex)
{
    if (_mutex == NULL) {
	return -1;
    }

    unsigned int state = kos_cpu_enter_critical();
    /* 先判断该锁是否被占用 */
    if (likely(_mutex->owner == NULL)) {
        __mutex_new_owner_mark(_mutex, kos_curr_proc);
        kos_cpu_exit_critical(state);
        return 0;
    }

    /* 如果该锁是否被自己拥有 */
    if (owner_is_self(_mutex->owner) && _mutex->mutex_attr == MUTEX_RECURSIVE) {
        /* 对锁的嵌套层数做限制循环加加不减, 会超出嵌套数(在循环的时候) */
        if (_mutex->lock_nesting == (unsigned char)-1) {
            kos_cpu_exit_critical(state);
            return -2;
        }
        _mutex->lock_nesting++;
        kos_cpu_exit_critical(state);
        return -2;
    }

    kos_cpu_exit_critical(state);

    return -3;
}

int __mutex_lock(struct kos_mutex *_mutex)
{
    unsigned int state = 0;

    if (__mutex_trylock(_mutex) == -3) {
        state = kos_cpu_enter_critical();
        /*
            如果此时有更高优先级的进程就绪, 执行调度
            而且该进程中也请求了对应的锁, 发现当前锁已经被拥有, 且拥有者的进程优先级较低
            此时需要将该高优先级进程挂起, 挂载到该mutex的链表上

            改变互斥锁拥有者优先级, 防止进程调度时, 又被抢占; 优先级反转
        */
        if (likely(_mutex->owner->priority > kos_curr_proc->priority)) {
            /* 改变互斥锁拥有者优先级 */
            __mutex_owner_prio_up(_mutex, kos_curr_proc->priority);
        }

        /*
            将当前进程挂起
            如果设置的是不可重入锁, 则会造成死锁现象, 因为自己将自己挂起, 而且无法找到该挂载链表
            基本上这个进程就变成了一个幽灵进程
        */
        kos_block_proc(&_mutex->block_obj, kos_curr_proc , KOS_TIME_FOREVER);

        kos_cpu_exit_critical(state);

        /* 调度 */
        kos_sched();

        /* 高优先级进程返回, 重新拥有锁 */
        state = kos_cpu_enter_critical();
        __mutex_new_owner_mark(_mutex, kos_curr_proc);
        kos_cpu_exit_critical(state);

    }

    /* 需要处理-2情况 */
    return -1;
}

int kos_mutex_init(struct kos_mutex *_mutex, kos_mutex_attr_t _attr)
{
    if (_mutex == NULL) {
	return -1;
    }

    _mutex->owner = NULL;
    _mutex->lock_nesting = 0;
    _mutex->owner_orig_prio = KOS_CONFIG_LOWEST_PRIO;
    kos_block_obj_init(&_mutex->block_obj, BLOCK_TYPE_MUTEX);

    _mutex->mutex_attr = _attr;

    return -1;
}

int kos_mutex_destory(struct kos_mutex *_mutex)
{
    if (_mutex == NULL) {
	return -1;
    }

    unsigned int state = kos_cpu_enter_critical();
    if (!kos_block_list_is_empty(&_mutex->block_obj)) {
        kos_block_obj_wakeup_all(&_mutex->block_obj);
    }

    kos_block_obj_deinit(&_mutex->block_obj);

    if (_mutex->owner) {
        __mutex_old_owner_release(_mutex);

        if (_mutex->mutex_attr == MUTEX_RECURSIVE) {
            _mutex->lock_nesting = 0;
        }
    }

    kos_cpu_exit_critical(state);

    kos_sched();

    return -1;
}

int kos_mutex_lock(struct kos_mutex *_mutex)
{
    return __mutex_lock(_mutex);
}


int kos_mutex_trylock(struct kos_mutex *_mutex)
{
    return __mutex_trylock(_mutex);
}

int kos_mutex_unlock(struct kos_mutex *_mutex)
{
    if (_mutex == NULL) {
	return -1;
    }

    unsigned int state = kos_cpu_enter_critical();

    /* 判断自己是否为进程拥有者 */
    if (!owner_is_self(_mutex->owner)) {
        kos_cpu_exit_critical(state);
        return -1;
    }

    /* 查看嵌套层数 */
    if (_mutex->mutex_attr == MUTEX_RECURSIVE && --_mutex->lock_nesting > 0) {
        kos_cpu_exit_critical(state);
        return -2;
    }

    /* 释放自己, 比如有更改优先级 */
    __mutex_old_owner_release(_mutex);

    /* 查看mutex上是否有挂载进程 */
    if (kos_block_list_is_empty(&_mutex->block_obj)) {
        kos_cpu_exit_critical(state);
        return 0;
    }

    /* 如果有则将阻塞列表上第一个进程挂载到就绪队列上 */
    kos_block_obj_wakeup_first(&_mutex->block_obj);

    kos_cpu_exit_critical(state);

    /* 调度 */
    kos_sched();

    return 0;
}
