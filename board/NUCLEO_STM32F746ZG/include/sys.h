#ifndef __SYS_H
#define __SYS_H


#include "stm32f7xx.h" 
 

//0,不支持OS
//1,支持OS
#define SYSTEM_SUPPORT_OS		0		//定义系统文件夹是否支持OS

///////////////////////////////////////////////////////////////////////////////////
//定义一些常用的数据类型短关键字 
typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;
 
typedef volatile uint32_t  vu32;
typedef volatile uint16_t vu16;
typedef volatile uint8_t  vu8; 

////////////////////////////////////////////////////////////////////////////////// 
//Ex_NVIC_Config专用定义
#define GPIO_A 				0
#define GPIO_B 				1
#define GPIO_C				2
#define GPIO_D 				3
#define GPIO_E 				4
#define GPIO_F 				5
#define GPIO_G 				6 
#define GPIO_H 				7 
#define GPIO_I 				8 

#define FTIR   				1  		//下降沿触发
#define RTIR   				2  		//上升沿触发

//GPIO设置专用宏定义
#define GPIO_MODE_IN    	0		//普通输入模式
#define GPIO_MODE_OUT		1		//普通输出模式
#define GPIO_MODE_AF		2		//AF功能模式
#define GPIO_MODE_AIN		3		//模拟输入模式

#define GPIO_SPEED_2M		0		//GPIO速度2Mhz(低速)
#define GPIO_SPEED_25M		1		//GPIO速度25Mhz(中速)
#define GPIO_SPEED_50M		2		//GPIO速度50Mhz(快速)
#define GPIO_SPEED_100M		3		//GPIO速度100Mhz(高速)

#define GPIO_PUPD_NONE		0		//不带上下拉
#define GPIO_PUPD_PU		1		//上拉
#define GPIO_PUPD_PD		2		//下拉
#define GPIO_PUPD_RES		3		//保留 

#define GPIO_OTYPE_PP		0		//推挽输出
#define GPIO_OTYPE_OD		1		//开漏输出 

//GPIO引脚编号定义
#define PIN0				1<<0
#define PIN1				1<<1
#define PIN2				1<<2
#define PIN3				1<<3
#define PIN4				1<<4
#define PIN5				1<<5
#define PIN6				1<<6
#define PIN7				1<<7
#define PIN8				1<<8
#define PIN9				1<<9
#define PIN10				1<<10
#define PIN11				1<<11
#define PIN12				1<<12
#define PIN13				1<<13
#define PIN14				1<<14
#define PIN15				1<<15 
////////////////////////////////////////////////////////////////////////////////// 
u8 system_clock_config(u32 plln,u32 pllm,u32 pllp,u32 pllq);		//系统时钟设置
void SystemInit(void); 
void system_soft_rest(void);      							//系统软复位
void cache_enable(void);									//使能catch
void system_standby(void);         							//待机模式 	
void nvic_priority_group_config(u8 NVIC_Group);			//设置NVIC分组
void system_nvic_init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group);//设置中断
void exit_nvic_config(u8 GPIOx,u8 BITx,u8 TRIM);				//外部中断配置函数(只对GPIOA~I)
void gpio_af_set(GPIO_TypeDef* GPIOx,u8 BITx,u8 AFx);		//GPIO复用功能设置
void gpio_set(GPIO_TypeDef* GPIOx,u32 BITx,u32 MODE,u32 OTYPE,u32 OSPEED,u32 PUPD);//GPIO设置函数 
void gpio_pin_set(GPIO_TypeDef* GPIOx,u16 pinx,u8 status);	//设置某个IO口的输出状态
u8 gpio_pin_get(GPIO_TypeDef* GPIOx,u16 pinx);				//读取某个IO口的输入状态
//以下为汇编函数
void WFI_SET(void);		//执行WFI指令
void MSR_MSP(u32 addr);	//设置堆栈地址 
#endif











