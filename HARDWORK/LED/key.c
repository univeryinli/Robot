#include "stm32f10x.h"
#include "key.h"
#include "delay.h"

void key_init(void)
{
	
	 GPIO_InitTypeDef GPIO_instructure;
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC,ENABLE);
	 GPIO_instructure.GPIO_Mode=GPIO_Mode_IPU;
	 GPIO_instructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_15;
	 GPIO_Init(GPIOA,&GPIO_instructure);
	
	 GPIO_instructure.GPIO_Mode=GPIO_Mode_IPD;
	 GPIO_instructure.GPIO_Pin=GPIO_Pin_5;
	 GPIO_Init(GPIOC,&GPIO_instructure);
}
	
u8 key_scan(u8 mode)//模式选择1——支持连续按  0——不支持连续按
{
	 static u8 key_up=1;
	 if(mode)key_up=1;
	 if(key_up&&(key0==0||key1==0||wake_up==1))
	 {
		 delay_ms(100);
     key_up=0;
     if(key0==0) return key0_press;
		 else if(key1==0) return key1_press;
		 else if(wake_up==1) return wake_up_press;	 
	 }
	 else if(key0==1||key1==1||wake_up==0) key_up=1;
	 return 0;
	
	
}
	
	 
	 
	
	 
	
	
	
	
