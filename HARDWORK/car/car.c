#include "car.h"
#include "stm32f10x.h"
#include "sys.h"
#include "timer.h"

void CAR_Init(void)
{
	
 
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	                //ʹ��PE�˿�ʱ��
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;	//Motor-->PE.7 ...  Motor-->PE.10 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 			  	//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 				//IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 				//�����趨������ʼ��GPIOE
	GPIO_ResetBits(GPIOB,GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
	
}

void go(void)
{
	    ENA=1;
			ENB=1;
			IN1=1;
			IN2=0;
			IN3=0;
			IN4=1;
}

void back(void)
{
	    ENA=1;
			ENB=1;
			IN1=0;
			IN2=1;
			IN3=1;
			IN4=0;
}

void left(void)
{
	    ENA=0;
			ENB=1;
			IN1=0;
			IN2=0;
			IN3=0;
			IN4=1;
}

void right(void)
{
	    ENA=1;
			ENB=0;
			IN1=1;
			IN2=0;
			IN3=0;
			IN4=0;
}

void stop(void)
{
	    ENA=0;
			ENB=0;
			IN1=0;
			IN2=0;
			IN3=0;
			IN4=0;
}

void go_speed(u16 arr,u16 psc,u16 ccr)
{		 					 
	
	TIM3_PWM_Init(arr,psc);
	TIM3->CCR1=ccr;  // ռ�ձ�	
	TIM3->CCR2=ccr;
	go();
}

void back_speed(u16 arr,u16 psc,u16 ccr)
{		 					 
	
	TIM3_PWM_Init(arr,psc);
	TIM3->CCR1=ccr;  // ռ�ձ�	
	TIM3->CCR2=ccr;
	back();
}

void left_speed(u16 arr,u16 psc,u16 ccr)
{		 					 
	
	TIM3_PWM_Init(arr,psc);
	TIM3->CCR1=ccr;  // ռ�ձ�	
	TIM3->CCR2=ccr;
	left();
}

void right_speed(u16 arr,u16 psc,u16 ccr)
{		 					 
	
	TIM3_PWM_Init(arr,psc);
	TIM3->CCR1=ccr;  // ռ�ձ�	
	TIM3->CCR2=ccr;
	right();
}






