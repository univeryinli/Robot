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
	delay_init();	    	 //��ʱ������ʼ?
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2	  	
	LED_Init();		  		//��ʼ����LED���ӵ�Ӳ���ӿ�
  uart1_init(9600);	 //���ڳ�ʼ��Ϊ9600
	Remote_Init();			//������ճ�ʼ��	
  CAR_Init ();
  Hcsr04Init();
	My_USART3_Init();
	uart2_init(9600);	 //���ڳ�ʼ��Ϊ9600
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



