#include "stm32f10x.h"
#include "stdio.h"
#include "led.h"
#include "car.h"
#include "remote.h"


/************************************************
 ALIENTEK ս��STM32F103������ʵ��0
 ����ģ��
 ע�⣬�����ֲ��е��½������½�ʹ�õ�main�ļ� 
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/
u8 USART3_RX_BUF;     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u8 USART3_RX_STA=0;       //����״̬���	  

void My_USART3_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStrue;
	USART_InitTypeDef USART_InitStrue;
	NVIC_InitTypeDef NVIC_InitStrue;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3 ,ENABLE);//��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitStrue.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStrue.GPIO_Pin=GPIO_Pin_10;
	GPIO_InitStrue.GPIO_Speed=GPIO_Speed_10MHz;
  GPIO_Init(GPIOB,&GPIO_InitStrue);//��
	
	GPIO_InitStrue.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStrue.GPIO_Pin=GPIO_Pin_11;
	GPIO_InitStrue.GPIO_Speed=GPIO_Speed_10MHz;
  GPIO_Init(GPIOB,&GPIO_InitStrue);//��
	
	USART_InitStrue.USART_BaudRate=9600;
	USART_InitStrue.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStrue.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;
	USART_InitStrue.USART_Parity=USART_Parity_No;
	USART_InitStrue.USART_StopBits=USART_StopBits_1;
	USART_InitStrue.USART_WordLength=USART_WordLength_8b;
	
	USART_Init(USART3,&USART_InitStrue);//��
	
	USART_Cmd(USART3,ENABLE);//ʹ�ܴ���1
	
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);//���������ж�
	
	NVIC_InitStrue.NVIC_IRQChannel=USART3_IRQn;
	NVIC_InitStrue.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStrue.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStrue.NVIC_IRQChannelSubPriority=1;
	NVIC_Init(&NVIC_InitStrue);
	
	
}


void USART3_IRQHandler(void)
{
	u8 Res3;
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //
		{
		Res3 =USART_ReceiveData(USART3);	//��ȡ���յ�������
		USART3_RX_STA=1;
		USART3_RX_BUF=Res3;
		USART_ClearITPendingBit(USART3, USART_IT_RXNE); 
		LED0=!LED0;
		
     } 
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
}
 

//ѡ�����ģʽ��a=0�Ǻ���ң��ģʽ��a=1����������ģʽ
void choose(u8 a,u8 b,u8 ch)
{
	 u32 key;
	 key =Remote_Scan();	
	 if(a==1)       //ѡ�����
	 { 
		if(key )
		{	  
			switch(key )
			{
//				case 0:str="ERROR";break;			       //����0
//				case 162:str="POWER";break;	         //����POWER
				case 98:go_speed(900,0,450);break;	   //ǰ������
				case 2:stop();break;		               //ֹͣ����
//				case 226:str="ALIENTEK";break;		   //ALIENTEK
				case 194:right_speed(900,0,250);break;	//����   
				case 34:left_speed(900,0,250);break;		// ���� 
//				case 224:str="VOL-";break;		        // VOL-
				case 168:back_speed(900,0,450);break;		 // ���� 
//				case 144:str="VOL+";break;		         //VOL+
				case 104:go_speed(900,0,350);break;		  //1
				case 152:go_speed(900,0,250);break;	    //2
				case 176:go_speed(900,0,50);break;	    //3
				case 48:back_speed(900,0,350);break;		 //4   
				case 24:back_speed(900,0,250);break;		 //5  
				case 122:back_speed(900,0,50);break;		  //6
//				case 16:str="7";break;			   					//7
//				case 56:str="8";break;	                //8
//				case 90:str="9";break;                  //9
//				case 66:str="0";break;                  //0
//				case 82:str="DELETE";break;		          //DELETE
			 }
			
		 }
	 }
	 
	 if(a==1)       //ѡ������
	 { 
		if(b)
		{	  
			switch(ch)
			{   
				case 0x01:go_speed(900,0,450);break;	
				case 0x02:back_speed(900,0,450);break;		
				case 0x03:left_speed(900,0,250);break;		
				case 0x04:right_speed(900,0,250);break;	   				
				case 0x05:stop();break;		 	  
				case 0x06:go_speed(900,0,350);break;		  //1
				case 0x07:go_speed(900,0,250);break;	    //2
				case 0x08:go_speed(900,0,50);break;	    //3
				case 0x09:back_speed(900,0,350);break;		    
				case 0x0a:back_speed(900,0,250);break;		    
				case 0x0b:back_speed(900,0,50);break;		   
			 }
			b=0;
		 }
	 }
	
}

