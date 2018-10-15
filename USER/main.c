#include "stm32f10x.h"
#include "led.h"
#include "delay.h"
#include "key.h"
#include "car.h"
#include "sys.h"
#include "remote.h"
#include "timer.h"
#include "usart.h"
#include "hcsr04.h"
#include "yymodule.h"
#include "lanya.h"

extern int is_obj;

int main()
{
	delay_init();	    	 //延时函数初始?
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2	  	
	LED_Init();		  		//初始化与LED连接的硬件接口
  uart1_init(9600);	 //串口初始化为9600
	Remote_Init();			//红外接收初始化	
  CAR_Init ();
  Hcsr04Init();
	My_USART3_Init();
	uart2_init(9600);	 //串口初始化为9600
	TIM4_Int_Init();
 	
//	Hcsr04GetLength();
	
	while(1)
	{
		
//		choose(1,USART3_RX_STA,USART3_RX_BUF);
		USART2_rx();
//		min=Hcsr04GetLength();
//		printf("%dxiao",is_obj);
//		LED0= 1;
		delay_ms(10);
//		LED0=0;
		
	}
	
}



