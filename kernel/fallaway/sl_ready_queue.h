#ifndef _SL_READY_QUEUE_H_
#define _SL_READY_QUEUE_H_



void run_queue_init(struct sl_ready_queue *rq);
void register_ready_proc(struct sl_ready_queue *rq, struct process_control_block proc);
void unregister_ready_proc(struct sl_ready_queue *rq, struct process_control_block proc);
void update_highest_priority(struct sl_ready_queue *rq);
void add_ready_list(struct sl_ready_queue *rq, struct process_control_block *proc_ptr);
void remove_ready_list(struct sl_ready_queue *rq, struct process_control_block *proc_ptr);
void get_ready_process(struct sl_ready_queue *rq);





#endif

