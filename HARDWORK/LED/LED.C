#include "led.h"
#include "stm32f10x.h"

void LED_Init(void)
{
	
  GPIO_InitTypeDef GPIO_Initstruct;	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
	
	GPIO_Initstruct.GPIO_Pin=GPIO_Pin_8;
	GPIO_Initstruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Initstruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA,&GPIO_Initstruct);
	GPIO_SetBits(GPIOA,GPIO_Pin_8);
	
	GPIO_Initstruct.GPIO_Pin=GPIO_Pin_2;
	GPIO_Initstruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Initstruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD,&GPIO_Initstruct);
	GPIO_SetBits(GPIOD,GPIO_Pin_2);
}


