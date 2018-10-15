#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "car.h"
#include "led.h"
#include "lanya.h"
////////////////////////////////////////////////////////////////////////////////// 	 

 
#if EN_USART2_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART2_RX_BUF[USART2_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART2_RX_STA=0;       //����״̬���	  
  
void uart2_init(u32 bound)
{
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	//USART1_TX   GPIOA.2
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
  //USART1_RX	  GPIOA.3��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
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

  USART_Init(USART2, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���1 

}


void USART2_IRQHandler(void)                	//����1�жϷ������
	{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{
		Res =USART_ReceiveData(USART2);	//��ȡ���յ�������
		
		if((USART2_RX_STA&0x8000)==0)//����δ���
			{
			if(USART2_RX_STA&0x4000)//���յ���0x0d
				{
				if(Res!=0x0a)USART2_RX_STA=0;//���մ���,���¿�ʼ
				else USART2_RX_STA|=0x8000;	//��������� 
				}
			else //��û�յ�0X0D
				{	
				if(Res==0x0d)USART2_RX_STA|=0x4000;
				else
					{
					USART2_RX_BUF[USART2_RX_STA&0X3FFF]=Res ;
					USART2_RX_STA++;
					if(USART2_RX_STA>(USART2_REC_LEN-1))USART2_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
					}		 
				}
			}   		 
     } 
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
} 

void USART2_rx(void)
{
	u8 t;
	u16 len;
	if(USART2_RX_STA&0x8000)
		{					   
			len=USART2_RX_STA&0x3fff;    //��ȡ���ݵĳ���
	//		printf("\r\n�����͵���ϢΪ:\r\n\r\n");
			for(t=0;t<len ;t++)
			{
				USART_SendData(USART2, USART2_RX_BUF[t]);   //���ͻ�ȡ�������ݵ����ڣ�����������Ϊȫ�ֱ���
				while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);  //�ȴ�������ɣ�������ɺ����ѭ���������¸�����
			}
			check2(len); 	
			printf("\r\n\r\n");
			USART2_RX_STA=0;
		}		
		delay_ms(500);
	
}

void check2(u16 len)
{
	u16 t=0;
	
		if(USART2_RX_BUF[t]==0xc7&&USART2_RX_BUF[t+1]==0xb0&&USART2_RX_BUF[t+2]==0xBD&&USART2_RX_BUF[t+3]==0xf8)
		{
			go_speed(900,0,450);
		}
  	 if(USART2_RX_BUF[t]==0xBA&&USART2_RX_BUF[t+1]==0xF3&&USART2_RX_BUF[t+2]==0xCD&&USART2_RX_BUF[t+3]==0xCB)
		{
			back_speed(900,0,450);
		}
		 if(USART2_RX_BUF[t]==0xD7&&USART2_RX_BUF[t+1]==0xF3&&USART2_RX_BUF[t+2]==0xD7&&USART2_RX_BUF[t+3]==0xAA)
		{
			left_speed(900,0,250);
		}
		 if(USART2_RX_BUF[t]==0xd3&&USART2_RX_BUF[t+1]==0xd2&&USART2_RX_BUF[t+2]==0xd7&&USART2_RX_BUF[t+3]==0xaa)
		{
			right_speed(900,0,250);
		}
		 if(USART2_RX_BUF[t]==0xcd&&USART2_RX_BUF[t+1]==0xa3&&USART2_RX_BUF[t+2]==0xd6&&USART2_RX_BUF[t+3]==0xb9)
		{
			stop();
		}
		if(USART2_RX_BUF[t]==0xd2&&USART2_RX_BUF[t+1]==0xbb&&USART2_RX_BUF[t+2]==0xb5&&USART2_RX_BUF[t+3]==0xb5&&USART2_RX_BUF[t+4]==0xc7&&USART2_RX_BUF[t+5]==0xb0&&USART2_RX_BUF[t+6]==0xbd&&USART2_RX_BUF[t+7]==0xf8)
		{
			go_speed(900,0,350);   //һ��ǰ��
		}
		if(USART2_RX_BUF[t]==0xb6&&USART2_RX_BUF[t+1]==0xfe&&USART2_RX_BUF[t+2]==0xb5&&USART2_RX_BUF[t+3]==0xb5&&USART2_RX_BUF[t+4]==0xc7&&USART2_RX_BUF[t+5]==0xb0&&USART2_RX_BUF[t+6]==0xbd&&USART2_RX_BUF[t+7]==0xf8)
		{
			go_speed(900,0,250);   //����ǰ��
		}
		if(USART2_RX_BUF[t]==0xc8&&USART2_RX_BUF[t+1]==0xfd&&USART2_RX_BUF[t+2]==0xb5&&USART2_RX_BUF[t+3]==0xb5&&USART2_RX_BUF[t+4]==0xc7&&USART2_RX_BUF[t+5]==0xb0&&USART2_RX_BUF[t+6]==0xbd&&USART2_RX_BUF[t+7]==0xf8)
		{
			go_speed(900,0,50);   //����ǰ��
		}
		if(USART2_RX_BUF[t]==0xd2&&USART2_RX_BUF[t+1]==0xbb&&USART2_RX_BUF[t+2]==0xb5&&USART2_RX_BUF[t+3]==0xb5&&USART2_RX_BUF[t+4]==0xba&&USART2_RX_BUF[t+5]==0xf3&&USART2_RX_BUF[t+6]==0xcd&&USART2_RX_BUF[t+7]==0xcb)
		{
			back_speed(900,0,350);   //һ������
		}
		if(USART2_RX_BUF[t]==0xb6&&USART2_RX_BUF[t+1]==0xfe&&USART2_RX_BUF[t+2]==0xb5&&USART2_RX_BUF[t+3]==0xb5&&USART2_RX_BUF[t+4]==0xba&&USART2_RX_BUF[t+5]==0xf3&&USART2_RX_BUF[t+6]==0xcd&&USART2_RX_BUF[t+7]==0xcb)
		{
			back_speed(900,0,250);   //��������
		}
		if(USART2_RX_BUF[t]==0xc8&&USART2_RX_BUF[t+1]==0xfd&&USART2_RX_BUF[t+2]==0xb5&&USART2_RX_BUF[t+3]==0xb5&&USART2_RX_BUF[t+4]==0xba&&USART2_RX_BUF[t+5]==0xf3&&USART2_RX_BUF[t+6]==0xcd&&USART2_RX_BUF[t+7]==0xcb)
		{
			back_speed(900,0,50);    //��������
		}		
		
		
	
}

#endif	

