/*
************************************ (C) COPYRIGHT 2017 Sourcelink **************************************
 * File Name	: sys.c
 * Author		: Sourcelink 
 * Version		: V1.0
 * Date 		: 2017/2/13
 * Description	: 
 ********************************************************************************************************
*/

#include "board.h"  
 


/*
*********************************************************************************************************
*    函 数 名: nvic_priority_group_config
*    功能说明: 设置NVIC分组 
*    形    参: NVIC_Group:NVIC分组 0~4 总共5组
*    返 回 值: 无
*********************************************************************************************************
*/

void nvic_priority_group_config(u8 NVIC_Group)	 
{ 
	u32 temp,temp1;	  
	temp1 = (~NVIC_Group) & 0x07;//取后三位
	temp1 <<= 8;
	temp = SCB->AIRCR;  //读取先前的设置
	temp &= 0X0000F8FF; //清空先前分组
	temp |= 0X05FA0000; //写入钥匙
	temp |= temp1;	   
	SCB->AIRCR =temp;   //设置分组	    	  				   
}

 

/*
*********************************************************************************************************
*    函 数 名: system_nvic_init
*    功能说明: 设置NVIC
*				
*    形    参: NVIC_PreemptionPriority:抢占优先级
*				NVIC_SubPriority       :响应优先级
*				NVIC_Channel           :中断编号
*				NVIC_Group             :中断分组 0~4
*					组划分:
*					组0:0位抢占优先级,4位响应优先级
*					组1:1位抢占优先级,3位响应优先级
*					组2:2位抢占优先级,2位响应优先级
*					组3:3位抢占优先级,1位响应优先级
*					组4:4位抢占优先级,0位响应优先级
*    返 回 值: 无
*********************************************************************************************************
*/

void system_nvic_init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group)	 
{ 
	u32 temp;	  
	nvic_priority_group_config(NVIC_Group);						//设置分组
	temp = NVIC_PreemptionPriority << (4 - NVIC_Group);	  
	temp |= NVIC_SubPriority & (0x0f >> NVIC_Group);
	temp &= 0xf;												//取低四位
	NVIC->ISER[NVIC_Channel / 32]|= 1 << NVIC_Channel % 32;		//使能中断位(要清除的话,设置ICER对应位为1即可)
	NVIC->IP[NVIC_Channel] |= temp << 4;						//设置响应优先级和抢断优先级   	    	  				   
} 




/*
*********************************************************************************************************
*    函 数 名: exit_nvic_config
*    功能说明: 外部中断配置函数
*				只针对GPIOA~I;不包括PVD,RTC,USB_OTG,USB_HS,以太网唤醒等
*				该函数一次只能配置1个IO口,多个IO口,需多次调用
*				该函数会自动开启对应中断,以及屏蔽线
*    形    参: GPIOx:0~8,代表GPIOA~I
*			   BITx:需要使能的位
*				TRIM:触发模式,1,下升沿;2,上降沿;3，任意电平触发
*    返 回 值: 无
*********************************************************************************************************
*/
  	    
void exit_nvic_config(u8 GPIOx,u8 BITx,u8 TRIM) 
{ 
	u8 EXTOFFSET = (BITx % 4) * 4;  
	RCC->APB2ENR |= 1 << 14;  								//使能SYSCFG时钟  
	SYSCFG->EXTICR[BITx / 4] &= ~(0x000F << EXTOFFSET);		//清除原来设置！！！
	SYSCFG->EXTICR[BITx / 4] |= (GPIOx << EXTOFFSET);		//EXTI.BITx映射到GPIOx.BITx 
	//自动设置
	EXTI->IMR |= (1 << BITx);								//开启line BITx上的中断(如果要禁止中断，则反操作即可)
	if(TRIM&0x01)EXTI->FTSR |= (1 << BITx);					//line BITx上事件下降沿触发
	if(TRIM&0x02)EXTI->RTSR |= (1 << BITx);					//line BITx上事件上升降沿触发
}


/*
*********************************************************************************************************
*    函 数 名: gpio_af_set
*    功能说明: GPIO复用设置 
*    形    参: GPIOx:GPIOA~GPIOI.
*				BITx:0~15,代表IO引脚编号.
*				AFx:0~15,代表AF0~AF15.
*					AF0~15设置情况(详细的请见429/746硬件参考手册,Table 12):
*					AF0:MCO/SWD/SWCLK/RTC   		AF1:TIM1/TIM2;            		AF2:TIM3~5;               		AF3:TIM8~11
*					AF4:I2C1~I2C4;          		AF5:SPI1~SPI6;            		AF6:SPI3/SAI1;            	 	AF7:SPI2/3/USART1~3/UART5/SPDIFRX;
*					AF8:USART4~8/SPDIFRX/SAI2; 		AF9:CAN1~2/TIM12~14/LCD/QSPI; 	AF10:USB_OTG/USB_HS/SAI2/QSPI  	AF11:ETH
*					AF12:FMC/SDMMC/OTG/HS   		AF13:DCIM                 		AF14:LCD; 						AF15:EVENTOUT
*    返 回 值: 无
*********************************************************************************************************
*/
               		
void gpio_af_set(GPIO_TypeDef* GPIOx,u8 BITx,u8 AFx)
{  
	GPIOx->AFR[BITx >> 3] &= ~(0X0F << ((BITx & 0X07) * 4));
	GPIOx->AFR[BITx >> 3] |= (u32)AFx << ((BITx & 0X07) * 4);
}



/*
*********************************************************************************************************
*    函 数 名: gpio_set
*    功能说明: GPIO通用设置	 
*    形    参: GPIOx:GPIOA~GPIOI.
*				BITx:0X0000~0XFFFF,位设置,每个位代表一个IO,第0位代表Px0,第1位代表Px1,依次类推.比如0X0101,代表同时设置Px0和Px8.
*				OTYPE:0/1;输出类型选择,0,推挽输出;1,开漏输出.
*				OSPEED:0~3;输出速度设置,0,低速;1,中速;2,快速;3,高速. 
*				PUPD:0~3:上下拉设置,0,不带上下拉;1,上拉;2,下拉;3,保留.
*    返 回 值: 引脚状态,0,引脚低电平;1,引脚高电平
*********************************************************************************************************
*/

void gpio_set(GPIO_TypeDef* GPIOx,u32 BITx,u32 MODE,u32 OTYPE,u32 OSPEED,u32 PUPD)
{  
	u32 pinpos = 0, pos = 0, curpin = 0;
	for (pinpos = 0; pinpos < 16; pinpos++) {
		pos = 1 << pinpos;									//一个个位检查 
		curpin = BITx & pos;								//检查引脚是否要设置
		if (curpin == pos)	{ 								//需要设置
			GPIOx->MODER &= ~(3 << (pinpos * 2));			//先清除原来的设置
			GPIOx->MODER |= MODE << (pinpos * 2);			//设置新的模式 
			if((MODE == 0X01) || (MODE == 0X02)) {  		//如果是输出模式/复用功能模式
				GPIOx->OSPEEDR &= ~(3 << (pinpos * 2));		//清除原来的设置
				GPIOx->OSPEEDR |= (OSPEED << (pinpos * 2)); //设置新的速度值  
				GPIOx->OTYPER &= ~(1 << pinpos);			//清除原来的设置
				GPIOx->OTYPER |= OTYPE << pinpos;			//设置新的输出模式
			}  
			GPIOx->PUPDR &= ~(3 << (pinpos * 2));			//先清除原来的设置
			GPIOx->PUPDR |= PUPD << (pinpos * 2);			//设置新的上下拉
		}
	}
} 



/*
*********************************************************************************************************
*    函 数 名: gpio_pin_set
*    功能说明: 设置GPIO某个引脚的输出状态 
*    形    参: GPIOx:GPIOA~GPIOI.
*				pinx:引脚编号,范围:0~15
*				status:引脚状态(仅最低位有效),0,输出低电平;1,输出高电平
*    返 回 值: 无
*********************************************************************************************************
*/

void gpio_pin_set(GPIO_TypeDef* GPIOx,u16 pinx,u8 status)
{
	if (status & 0X01) GPIOx->BSRR = pinx;	//设置GPIOx的pinx为1
	else GPIOx->BSRR = pinx << 16;			//设置GPIOx的pinx为0
}



/*
*********************************************************************************************************
*    函 数 名: gpio_pin_get
*    功能说明: 读取GPIO某个引脚的状态	 
*    形    参: GPIOx:GPIOA~GPIOI.
*				pinx:引脚编号,范围:0~15
*    返 回 值: 引脚状态,0,引脚低电平;1,引脚高电平
*********************************************************************************************************
*/

u8 gpio_pin_get(GPIO_TypeDef* GPIOx,u16 pinx)
{ 
	if (GPIOx->IDR & pinx) return 1;		//pinx的状态为1
	else return 0;							//pinx的状态为0
}





/*
*********************************************************************************************************
*    函 数 名: system_soft_rest
*    功能说明: 系统软复位 
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/

void system_soft_rest(void)
{   
	SCB->AIRCR = 0X05FA0000 | (u32)0x04;	  
}



/*
*********************************************************************************************************
*    函 数 名: cache_enable
*    功能说明: 使能STM32F7的L1-Cache,同时开启D cache的强制透写
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/

void cache_enable(void)
{
    SCB_EnableICache();		//使能I-Cache,函数在core_cm7.h里面定义
    SCB_EnableDCache();		//使能D-Cache,函数在core_cm7.h里面定义 
	SCB->CACR |= 1 << 2;	//强制D-Cache透写,如不开启透写,实际使用中可能遇到各种问题
}



/*
*********************************************************************************************************
*    函 数 名: system_clock_config
*    功能说明: 系统时钟设置
*    形    参: 时钟设置函数
*				Fvco=Fs*(plln/pllm);
*				Fsys=Fvco/pllp=Fs*(plln/(pllm*pllp));
*				Fusb=Fvco/pllq=Fs*(plln/(pllm*pllq));
*
*				Fvco:VCO频率
*				Fsys:系统时钟频率
*				Fusb:USB,SDMMC,RNG等的时钟频率
*				Fs:PLL输入时钟频率,可以是HSI,HSE等. 
*				plln:主PLL倍频系数(PLL倍频),取值范围:50~432.
*				pllm:主PLL和音频PLL分频系数(PLL之前的分频),取值范围:2~63.
*				pllp:系统时钟的主PLL分频系数(PLL之后的分频),取值范围:2,4,6,8.(仅限这4个值!)
*				pllq:USB/SDMMC/随机数产生器等的主PLL分频系数(PLL之后的分频),取值范围:2~15.
*
*				外部晶振为8M的时候,推荐值:plln=432,pllm=8,pllp=2,pllq=9.
*				得到:Fvco=8*(432/8)=432Mhz
*				     Fsys=432/2=216Mhz
*				     Fusb=432/9=48Mhz
*    返 回 值: 0,成功;1,失败。
*********************************************************************************************************
*/

u8 system_clock_config(u32 plln,u32 pllm,u32 pllp,u32 pllq)
{ 
	u16 retry=0;
	u8 status=0;
	RCC->CR |= 1 << 16;				//HSE 开启 
	while (((RCC->CR & (1<<17)) == 0) && (retry < 0X1FFF)) retry++;//等待HSE RDY
	if (retry == 0X1FFF) status = 1;	//HSE无法就绪
	else  {
		RCC->APB1ENR |= 1 << 28;	//电源接口时钟使能
		PWR->CR1 |= 3 << 14; 		//高性能模式,时钟可到180Mhz
		PWR->CR1 |= 1 << 16; 		//使能过驱动,频率可到216Mhz
		PWR->CR1 |= 1 << 17; 		//使能过驱动切换
		RCC->CFGR |= (0 << 4) | (5 << 10) | (4 << 13);//HCLK 不分频;APB1 4分频;APB2 2分频.
		RCC->CR &= ~(1 << 24);		//关闭主PLL
		RCC->PLLCFGR = ((pllm << 0) | (plln << 6) | (((pllp>>1)-1)<<16) | (pllq<<24) | (1<<22));//配置主PLL,PLL时钟源来自HSE
		RCC->CR |= 1 << 24;			//打开主PLL
		while ((RCC->CR & (1 << 25)) == 0);//等待PLL准备好 
		FLASH->ACR |= 1 << 8;		//指令预取使能.
		FLASH->ACR |= 1 << 9;		//使能ART Accelerator 
		FLASH->ACR |= 7 << 0;		//8个CPU等待周期. 
		RCC->CFGR &= ~(3 << 0);		//清零
		RCC->CFGR |= 2 << 0;		//选择主PLL作为系统时钟	 
		while ((RCC->CFGR & (3 << 2)) != (2 << 2));//等待主PLL作为系统时钟成功. 
	} 
	return status;
}  




/*
*********************************************************************************************************
*    函 数 名: SystemInit
*    功能说明: 系统初始化
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/

void SystemInit(void)
{  
	RCC->CR |= 0x00000001;		//设置HISON,开启内部高速RC振荡
	RCC->CFGR = 0x00000000;		//CFGR清零 
	RCC->CR &= 0xFEF6FFFF;		//HSEON,CSSON,PLLON清零 
	RCC->PLLCFGR = 0x24003010;	//PLLCFGR恢复复位值 m0x10 n0xc0 p0 src0 q4
	RCC->CR &= ~(1<<18);		//HSEBYP清零,外部晶振不旁路
	RCC->CIR = 0x00000000;		//禁止RCC时钟中断 

	//配置向量表				  
#ifdef  VECT_TAB_RAM
	SCB->VTOR = SRAM1_BASE | 0x00;
#else   
	SCB->VTOR = FLASH_BASE | 0x00;
#endif 
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



int _write(int file, char *ptr, int len)
{
    for (int t = 0; t < len; t++)
    {
        while ((USART2->ISR & 0x40) == 0);
        USART2->TDR = ptr[t];
    }
    return len;
}


void board_init(void)
{
	system_clock_config(432, 8, 2, 9);
	cache_enable();
	uart_init(54, 115200);

	RCC->AHB1ENR |= 1 << 1;			 
	gpio_set(GPIOB, PIN14 | PIN7 | PIN0, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU); //PB0

	SysTick_Config(216000000 / 1000);
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
 
















