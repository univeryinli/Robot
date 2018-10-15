#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "car.h"
#include "led.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/8/18
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*ʹ��microLib�ķ���*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  
  
void uart1_init(u32 bound)
{
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
  //USART1_RX	  GPIOA.10��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 

}


void USART1_IRQHandler(void)                	//����1�жϷ������
	{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{
		Res =USART_ReceiveData(USART1);	//��ȡ���յ�������
		
		if((USART_RX_STA&0x8000)==0)//����δ���
			{
			if(USART_RX_STA&0x4000)//���յ���0x0d
				{
				if(Res!=0x0a)USART_RX_STA=0;//���մ���,���¿�ʼ
				else USART_RX_STA|=0x8000;	//��������� 
				}
			else //��û�յ�0X0D
				{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
					{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
					}		 
				}
			}   		 
     } 
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
} 

void USART1_rx(void)
{
	u8 t;
	u16 len;
	if(USART_RX_STA&0x8000)
		{					   
			len=USART_RX_STA&0x3fff;    //��ȡ���ݵĳ���
// 	  	printf("\r\n�����͵���ϢΪ:\r\n\r\n");
			for(t=0;t<len ;t++)
			{
				USART_SendData(USART1, USART_RX_BUF[t]);   //���ͻ�ȡ�������ݵ����ڣ�����������Ϊȫ�ֱ���
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);  //�ȴ�������ɣ�������ɺ����ѭ���������¸�����
			}
			check(len); 	
			printf("\r\n\r\n");
			USART_RX_STA=0;
		}		
		delay_ms(500);
	
}

void check(u16 len)
{
	u16 t=0;
	
		if(USART_RX_BUF[t]==0xc7&&USART_RX_BUF[t+1]==0xb0&&USART_RX_BUF[t+2]==0xBD&&USART_RX_BUF[t+3]==0xf8)
		{
			go_speed(900,0,450);
		}
  	 if(USART_RX_BUF[t]==0xBA&&USART_RX_BUF[t+1]==0xF3&&USART_RX_BUF[t+2]==0xCD&&USART_RX_BUF[t+3]==0xCB)
		{
			back_speed(900,0,450);
		}
		 if(USART_RX_BUF[t]==0xD7&&USART_RX_BUF[t+1]==0xF3&&USART_RX_BUF[t+2]==0xD7&&USART_RX_BUF[t+3]==0xAA)
		{
			left_speed(900,0,250);
		}
		 if(USART_RX_BUF[t]==0xd3&&USART_RX_BUF[t+1]==0xd2&&USART_RX_BUF[t+2]==0xd7&&USART_RX_BUF[t+3]==0xaa)
		{
			right_speed(900,0,250);
		}
		 if(USART_RX_BUF[t]==0xcd&&USART_RX_BUF[t+1]==0xa3&&USART_RX_BUF[t+2]==0xd6&&USART_RX_BUF[t+3]==0xb9)
		{
			stop();
		}
		if(USART_RX_BUF[t]==0xd2&&USART_RX_BUF[t+1]==0xbb&&USART_RX_BUF[t+2]==0xb5&&USART_RX_BUF[t+3]==0xb5&&USART_RX_BUF[t+4]==0xc7&&USART_RX_BUF[t+5]==0xb0&&USART_RX_BUF[t+6]==0xbd&&USART_RX_BUF[t+7]==0xf8)
		{
			go_speed(900,0,350);   //һ��ǰ��
		}
		if(USART_RX_BUF[t]==0xb6&&USART_RX_BUF[t+1]==0xfe&&USART_RX_BUF[t+2]==0xb5&&USART_RX_BUF[t+3]==0xb5&&USART_RX_BUF[t+4]==0xc7&&USART_RX_BUF[t+5]==0xb0&&USART_RX_BUF[t+6]==0xbd&&USART_RX_BUF[t+7]==0xf8)
		{
			go_speed(900,0,250);   //����ǰ��
		}
		if(USART_RX_BUF[t]==0xc8&&USART_RX_BUF[t+1]==0xfd&&USART_RX_BUF[t+2]==0xb5&&USART_RX_BUF[t+3]==0xb5&&USART_RX_BUF[t+4]==0xc7&&USART_RX_BUF[t+5]==0xb0&&USART_RX_BUF[t+6]==0xbd&&USART_RX_BUF[t+7]==0xf8)
		{
			go_speed(900,0,50);   //����ǰ��
		}
		if(USART_RX_BUF[t]==0xd2&&USART_RX_BUF[t+1]==0xbb&&USART_RX_BUF[t+2]==0xb5&&USART_RX_BUF[t+3]==0xb5&&USART_RX_BUF[t+4]==0xba&&USART_RX_BUF[t+5]==0xf3&&USART_RX_BUF[t+6]==0xcd&&USART_RX_BUF[t+7]==0xcb)
		{
			back_speed(900,0,350);   //һ������
		}
		if(USART_RX_BUF[t]==0xb6&&USART_RX_BUF[t+1]==0xfe&&USART_RX_BUF[t+2]==0xb5&&USART_RX_BUF[t+3]==0xb5&&USART_RX_BUF[t+4]==0xba&&USART_RX_BUF[t+5]==0xf3&&USART_RX_BUF[t+6]==0xcd&&USART_RX_BUF[t+7]==0xcb)
		{
			back_speed(900,0,250);   //��������
		}
		if(USART_RX_BUF[t]==0xc8&&USART_RX_BUF[t+1]==0xfd&&USART_RX_BUF[t+2]==0xb5&&USART_RX_BUF[t+3]==0xb5&&USART_RX_BUF[t+4]==0xba&&USART_RX_BUF[t+5]==0xf3&&USART_RX_BUF[t+6]==0xcd&&USART_RX_BUF[t+7]==0xcb)
		{
			back_speed(900,0,50);    //��������
		}		
		
		
	
}

#endif	

