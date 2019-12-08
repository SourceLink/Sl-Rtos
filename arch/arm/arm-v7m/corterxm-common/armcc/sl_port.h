#ifndef _SL_OS_H_
#define _SL_OS_H_




/************************************************************************************************/

extern volatile unsigned int sl_interrput_sum;
extern struct process_control_block* sl_current_process; 
extern struct process_control_block* sl_ready_process;


typedef void (*sl_process)(void);

/************************************************************************************************/

unsigned int *sl_stack_init(sl_process process,unsigned int *stack);
void 		  SL_OS_START(void);
void 		  SL_OS_CTXSW(void);
void		  SL_ENTER_CRITICAL(void);
void 		  SL_EXIT_CRITICAL(void);





#endif

