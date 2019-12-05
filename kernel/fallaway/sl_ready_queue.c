/*
************************************ (C) COPYRIGHT 2017 Sourcelink **************************************
 * File Name	: sl_ready_queue.c
 * Author		: Sourcelink 
 * Version		: V1.0
 * Date 		: 2017/2/28
 * Description	: 2017/3/2 : 检测出add_ready_list_end(struct sl_ready_queue *rq, struct process_control_block *proc_ptr)
 *										list_add_tail(&rq->proc_ready_list[prio], &proc_ptr->run_list); 
 *							 形参new和head写反了导致链表断链
 *				  
 ********************************************************************************************************
*/

#include "sl_kernel.h"

struct sl_ready_queue			sl_rq;														/* 就绪进程实体 */


/* 进行位转换的数组 */
unsigned char const sl_map_table[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

/* 进行最高优先级就绪进程查找的数组 */
unsigned char const sl_unmap_table[] = {
	0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 	  /* 0x00 to 0x0F  */
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 	  /* 0x10 to 0x1F  */
	5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 	  /* 0x20 to 0x2F  */
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 	  /* 0x30 to 0x3F  */
	6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 	  /* 0x40 to 0x4F  */
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 	  /* 0x50 to 0x5F  */
	5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 	  /* 0x60 to 0x6F  */
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 	  /* 0x70 to 0x7F  */
	7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 	  /* 0x80 to 0x8F  */
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 	  /* 0x90 to 0x9F  */
	5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 	  /* 0xA0 to 0xAF  */
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 	  /* 0xB0 to 0xBF  */
	6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 	  /* 0xC0 to 0xCF  */
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 	  /* 0xD0 to 0xDF  */
	5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 	  /* 0xE0 to 0xEF  */
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0		  /* 0xF0 to 0xFF  */
};


/*
*********************************************************************************************************
*    函 数 名: run_queue_init
*    功能说明: 就绪队列初始化
*    形    参: rq : 就绪链表对象
*    返 回 值: 无
*********************************************************************************************************
*/

void run_queue_init(struct sl_ready_queue *rq)
{
	unsigned char i;

	/* 初始化为最低优先级进程 */
	rq->highest_priority	= SL_LOWEST_PRIO;

	/* 遍历初始化就绪队列 */
	for (i = 0; i < SL_READY_LIST_SIZE; i++) {
		list_head_init(&rq->proc_ready_list[i]);
	}
}



/*
*********************************************************************************************************
*    函 数 名: register_ready_proc
*    功能说明: 注册进程到就绪表
*    形    参: rq: 就绪表实体		proc: 进程控制块		
*    返 回 值: 无
*********************************************************************************************************
*/

void register_ready_proc(struct sl_ready_queue *rq, struct process_control_block proc)
{
	rq->proc_ready_grop |= proc.num_grop_bit;
	rq->proc_ready_table[proc.num_grop] |= proc.num_process_bit;
}



/*
*********************************************************************************************************
*    函 数 名: unregister_ready_proc
*    功能说明: 注销就绪表中进程
*    形    参: head: 表头		proc_ptr: 进程控制块		
*    返 回 值: 无
*********************************************************************************************************
*/

void unregister_ready_proc(struct sl_ready_queue *rq, struct process_control_block proc)
{
	if ((rq->proc_ready_table[proc.num_grop] &= ~proc.num_process_bit) == 0) {
		rq->proc_ready_grop &= ~proc.num_grop_bit;
	}
}



/*
*********************************************************************************************************
*    函 数 名: get_highest_priority
*    功能说明: 获取最高优先级
*    形    参: rq: 就绪表实体				
*    返 回 值: 无
*********************************************************************************************************
*/

void update_highest_priority(struct sl_ready_queue *rq)
{
	unsigned char num_grop, num_table;
	
	num_grop = sl_unmap_table[rq->proc_ready_grop];
	num_table = sl_unmap_table[rq->proc_ready_table[num_grop]];

	rq->highest_priority = (num_grop << 3) + num_table;
}



/*
*********************************************************************************************************
*    函 数 名: add_to_priority_list
*    功能说明: 把进程按优先级大小插入到队列中
*    形    参: head: 表头		proc_ptr: 进程控制块		
*    返 回 值: 无
*********************************************************************************************************
*/

static __inline void add_to_priority_list(struct list_head *head, struct process_control_block *proc_ptr)
{
	unsigned char val;

	struct list_head *tmp;
	struct process_control_block *pcb;

	val = proc_ptr->priority;

	list_for_each(tmp, head)
	{
		pcb = kos_list_entry(tmp, struct process_control_block, run_list);
		if (pcb->priority > val) {
			break;
		}
	}

	list_add(tmp, &proc_ptr->run_list);
}



/*
*********************************************************************************************************
*    函 数 名: add_ready_list_head
*    功能说明: 添加节点到表头
*    形    参: rq: 就绪表的实体		proc_ptr: 进程控制块		
*    返 回 值: 无
*********************************************************************************************************
*/

void add_ready_list_head(struct sl_ready_queue *rq, struct process_control_block *proc_ptr)
{
	unsigned char prio	= proc_ptr->priority;

	/* 添加到表头的后一个节点 */
	list_add(&rq->proc_ready_list[prio], &proc_ptr->run_list);

	/* 登记到就绪表 */
	register_ready_proc(rq, *proc_ptr);				

	if (prio < rq->highest_priority) {
		rq->highest_priority = prio;
	}
}



/*
*********************************************************************************************************
*    函 数 名: add_ready_list_end
*    功能说明: 添加节点到表尾
*    形    参: rq: 就绪表的实体		proc_ptr: 进程控制块		
*    返 回 值: 无
*********************************************************************************************************
*/

void add_ready_list_end(struct sl_ready_queue *rq, struct process_control_block *proc_ptr)
{
	unsigned char prio	= proc_ptr->priority;

	/* 添加到表头的前一个节点，既表的最后 */
	list_add_tail(&rq->proc_ready_list[prio], &proc_ptr->run_list);

	/* 登记到就绪表 */
	register_ready_proc(rq, *proc_ptr);

	if (prio < rq->highest_priority) {
		rq->highest_priority = prio;
	}
}



/*
*********************************************************************************************************
*    函 数 名: add_ready_list
*    功能说明: 根据当前优先级判断把进程控制块加入何位置
*    形    参: rq: 就绪表的实体		proc_ptr: 进程控制块		
*    返 回 值: 无
*********************************************************************************************************
*/

void add_ready_list(struct sl_ready_queue *rq, struct process_control_block *proc_ptr)
{
	/* 如果该进程和当前运行进程的优先级相同则放入队尾 */
	if (proc_ptr->priority == sl_current_process->priority) {
		add_ready_list_end(rq, proc_ptr);
	} else {
		/* 如果该优先级的就绪进程为空则放在队头否则放在队尾 */
		if (list_empty(&rq->proc_ready_list[proc_ptr->priority])) {
			add_ready_list_head(rq, proc_ptr);
		} else {
			add_ready_list_end(rq, proc_ptr);
		}
	}
}



/*
*********************************************************************************************************
*    函 数 名: remove_ready_list
*    功能说明: 根据当前优先级判断把进程控制块加入何位置
*    形    参: rq: 就绪表的实体		proc_ptr: 进程控制块		
*    返 回 值: 无
*********************************************************************************************************
*/

void remove_ready_list(struct sl_ready_queue *rq, struct process_control_block *proc_ptr)
{
	unsigned char prio;
	
	prio = proc_ptr->priority;
	
	list_delete(&proc_ptr->run_list);

	/* 改用链表后可运行同优先级进程。
	 * 如果最高优先级下还有就绪进程没有运行则不需要更新最高优先级
	 */
	if (!list_empty(&rq->proc_ready_list[prio])) {
		return;	
	}

	unregister_ready_proc(rq, *proc_ptr);

	/* 如果任务优先级不等于最高优先级，那么我们就不需要更新最高优先级
     * 这种情况发生在当前的高优先级任务抢占低优先级任务
	 */
	if (prio != rq->highest_priority) {
		return;
	}

	/* 更新最高优先级 */
	update_highest_priority(rq);
	
}



/*
*********************************************************************************************************
*    函 数 名: move_to_ready_list_end
*    功能说明: 把该进程移动到就绪表的末尾 
*    形    参: rq: 就绪表的实体		proc_ptr: 进程控制块		
*    返 回 值: 无
*********************************************************************************************************
*/

void move_to_ready_list_end(struct sl_ready_queue *rq, struct process_control_block *proc_ptr)
{
	unsigned char prio = proc_ptr->priority;

	/* 删除原来挂载在就绪进程的位置 */
	list_delete(&proc_ptr->run_list);

	/* 移动到队尾 */
	list_add_tail(&rq->proc_ready_list[prio], &proc_ptr->run_list);
}



/*
*********************************************************************************************************
*    函 数 名: get_ready_process
*    功能说明: 从就绪表中得到一个最高优先级进程控制块
*    形    参: rq: 就绪表的实体		proc_ptr: 进程控制块		
*    返 回 值: 无
*********************************************************************************************************
*/
void get_ready_process(struct sl_ready_queue *rq)
{
	unsigned char prio;
	struct list_head *node;
	
	prio = rq->highest_priority;
	node = rq->proc_ready_list[prio].next;
	
	sl_ready_process = kos_list_entry(node, struct process_control_block, run_list);
}




