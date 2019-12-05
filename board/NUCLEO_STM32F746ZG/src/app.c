#include "sys.h"
#include "stdio.h"
#include "core_cm7.h"

#include "sl_port.h"


unsigned char sl_running_flag = 0;									/* 等于1时出于多进程状态 */
struct kos_pcb *sl_current_process; 								/* 当前运行的进程指针 */
struct kos_pcb *sl_ready_process;									/* 下一个要运行的进程指针 */


#define EN_USART2_RX		1


int _write(int file, char *ptr, int len)
{
    for (int t = 0; t < len; t++)
    {
        while ((USART2->ISR & 0x40) == 0);
        USART2->TDR = ptr[t];
    }
    return len;
}

void uart_init(u32 pclk2,u32 bound)
{  	 
	u32	temp;	   
	temp=(pclk2 * 1000000 + bound / 2) / bound;	
	RCC->AHB1ENR |= 1 << 3 ;   					// port a 
	RCC->APB1ENR |= 1 << 17;  					// uart2
	gpio_set(GPIOD, PIN5 | PIN6, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_50M, GPIO_PUPD_PU);
 	gpio_af_set(GPIOD, 5, 7);			
	gpio_af_set(GPIOD, 6, 7);			  	   
	
 	USART2->BRR = temp; 					
	USART2->CR1 = 0;		 			
	USART2->CR1 &= ~(1 << 28);	 		
	USART2->CR1 &= ~(1 << 12);	 		
	USART2->CR1 &= ~(1 << 15); 			 
	USART2->CR1	|= 1 << 3;  			
#if EN_USART2_RX		  				
	USART2->CR1 |= 1<< 2;  				
	USART2->CR1 |= 1<< 5;    				    	
	system_nvic_init(3, 3, USART2_IRQn, 2); 
#endif
	USART2->CR1 |= 1 << 0;  			 
}

int main(void)
{
	system_clock_config(432, 8, 2, 9);
	cache_enable();

	uart_init(54, 115200);

    printf("systme init ok\r\n");
	
	RCC->AHB1ENR |= 1 << 1;			 
	gpio_set(GPIOB, PIN14 | PIN7 | PIN0, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU); //PB0

	SysTick_Config(216000000 / 1000);
	

	port_os_start();

    return 0;
}



void HardFault_Handler(void)
{
	while(1);
}


void USART2_IRQHandler(void)
{	
	if (USART2->ISR&(1<<5))						
	{	 
		USART2->RQR |= (1 << 3);				
	} 
} 
 

void SysTick_Handler(void)
{
    //SysTick_ISR();
}


