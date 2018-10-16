#include "hcsr04.h"
#include "usart.h"
#include "delay.h"

#define HCSR04_PORT     GPIOB
#define HCSR04_CLK      RCC_APB2Periph_GPIOB
#define HCSR04_TRIG     GPIO_Pin_9
#define HCSR04_ECHO     GPIO_Pin_4|GPIO_Pin_5
 
#define TRIG_Send  PBout(5)
#define ECHO_Reci  PBin(4)
 
u16 msHcCount = 0;//ms����

/*
if you want detect obj,so you can use flag is_obj. 
*/
int is_obj;

void Hcsr04Init()
{  
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;     //�������ڶ�ʱ�����õĽṹ��
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(HCSR04_CLK, ENABLE);
     
        //IO��ʼ��
    GPIO_InitStructure.GPIO_Pin =HCSR04_TRIG;       //���͵�ƽ����
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//�������
    GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(HCSR04_PORT,HCSR04_TRIG);
     
    GPIO_InitStructure.GPIO_Pin =   HCSR04_ECHO;     //���ص�ƽ����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);  
		GPIO_ResetBits(HCSR04_PORT,HCSR04_ECHO);	
	 
			//��ʱ����ʼ�� ʹ�û�����ʱ��TIM6
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);   //ʹ�ܶ�ӦRCCʱ��
		//���ö�ʱ�������ṹ��
		TIM_DeInit(TIM6);
		TIM_TimeBaseStructure.TIM_Period = (1000-1); //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ         ������1000Ϊ1ms
		TIM_TimeBaseStructure.TIM_Prescaler =(72-1); //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  1M�ļ���Ƶ�� 1US����
		TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;//����Ƶ
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
		TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ		 
		
		TIM_ClearFlag(TIM6, TIM_FLAG_Update);   //��������жϣ����һ���ж����������ж�
		TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);    //�򿪶�ʱ�������ж�
		hcsr04_NVIC();
    TIM_Cmd(TIM6,DISABLE);      
}
 
//tips��static����������������ڶ�������Դ�ļ��ڣ����Բ���Ҫ��ͷ�ļ�������
void OpenTimerForHc()        //�򿪶�ʱ��
{
        TIM_SetCounter(TIM6,0);//�������
        msHcCount = 0;
        TIM_Cmd(TIM6, ENABLE);  //ʹ��TIMx����
//	      printf("ok1\n");
}
 
void CloseTimerForHc()        //�رն�ʱ��
{
        TIM_Cmd(TIM6, DISABLE);  //ʹ��TIMx����
//	      printf("ok2\n");
}
 
 
 //NVIC����
void hcsr04_NVIC()
{
			NVIC_InitTypeDef NVIC_InitStructure;
			NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
			NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;             //ѡ�񴮿�1�ж�
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռʽ�ж����ȼ�����Ϊ1
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;         //��Ӧʽ�ж����ȼ�����Ϊ1
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;        //ʹ���ж�
			NVIC_Init(&NVIC_InitStructure);
}
 
 
//��ʱ��6�жϷ������
void TIM6_IRQHandler(void)   //TIM3�ж�
{
//  printf("ok\n");
	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
        {
//						printf("ok\n");
						TIM_ClearITPendingBit(TIM6, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
						msHcCount++;
//					  printf("%d\n",msHcCount);
        }
}
 
 
//��ȡ��ʱ��ʱ��
u32 GetEchoTimer(void)
{
        u32 t = 0;
        t = msHcCount*1000;//�õ�MS
        t += TIM_GetCounter(TIM6);//�õ�US
//				printf("%d\n",msHcCount);
	      TIM6->CNT = 0;  //��TIM2�����Ĵ����ļ���ֵ����
//	      printf("ok3\n");
				delay_ms(30);
        return t;
}
 

//ð�����򣬴Ӵ�С
void sortA2(float a[], int length){
    int i, j; 
		float	temp;

    for(i = 0; i < length; ++i){

        for(j = length - 1; j > i; --j){

            if(a[j] > a[j - 1]){
                
                temp = a[j];

                a[j] = a[j - 1];

                a[j - 1] = temp;
            }
        }
    }
}

/*
//һ�λ�ȡ������������� ���β��֮����Ҫ���һ��ʱ�䣬���ϻ����ź�
//Ϊ�����������Ӱ�죬ȡ������ݵ�ƽ��ֵ���м�Ȩ�˲���
float Hcsr04GetLength(void)
{
	u32 t = 0;
	int i = 0,j,s;
	float lengthTemp = 0,min;
	float sum = 0;
	int num=1;
	float length[4];
	for(j=0;j<num;j++)
	{
//		s=2*j;
		printf("%d�ų�������ʼ���\n\n",j+1);
//		printf("%djjj",s);
		while(i<5)
		{
		PBout(5) = 1;      //���Ϳڸߵ�ƽ���
		delay_us(20);
		PBout(5) = 0;
		
		while(PBin((0+4)) == 0);      //�ȴ����տڸߵ�ƽ���
		OpenTimerForHc();        //�򿪶�ʱ��
		printf("ok");
		i = i + 1;
		while(PBin((0+4)) == 1);
		CloseTimerForHc();        //�رն�ʱ��
		t = GetEchoTimer();        //��ȡʱ��,�ֱ���Ϊ1US
		lengthTemp = ((float)t/58.0);//cm
		sum = lengthTemp + sum ;
		}
		lengthTemp = sum/5.0;
		length[j]=lengthTemp;
	  printf("%d�ų�������ֵ=%.3fcm\n\n",j+1,lengthTemp);		  
		i=0;
    lengthTemp = 0;	
    sum = 0;
	}
//	sortA2(length,num);
//	min= (length[num-1]+length[num-2]+length[num-3])/3;
	  
//	printf("%fzuida",min);
	return 1;
}
*/

//һ�λ�ȡ������������� ���β��֮����Ҫ���һ��ʱ�䣬���ϻ����ź�
//Ϊ�����������Ӱ�죬ȡ������ݵ�ƽ��ֵ���м�Ȩ�˲���
float Hcsr04GetLength(int j)
{
	u32 t = 0;
	int i;
	float sum=0,lengthTemp;
	for(i=0;i<5;i++)
	{
		PBout(9) = 1;      //���Ϳڸߵ�ƽ���
		delay_us(20);
		PBout(9) = 0;
		while(PBin((4+j)) == 0);      //�ȴ����տڸߵ�ƽ���
		OpenTimerForHc();        //�򿪶�ʱ��
		while(PBin((4+j)) == 1);
		CloseTimerForHc();        //�رն�ʱ��
		t = GetEchoTimer();        //��ȡʱ��,�ֱ���Ϊ1US
		lengthTemp = ((float)t/58.0);//cm
		sum = lengthTemp + sum ;

	}
	lengthTemp = sum/5.0;
	return lengthTemp;
}

// size is sensor size, and the min_size is to get nums of  min length
float Hcsr04_min_Length()
{
	int j,size=3,k,min_size=1;
	float length,min,sum=0;
	float length_num[3]={0};
	for(j=0;j<size;j++)
	{
//		printf("%d�ų�������ʼ���\n\n",j+1);
		length=Hcsr04GetLength(j);
		length_num[j]=length;
//		printf("%d�ų�������ֵ=%.3fcm\n\n",j+1,length);	
		
	}
	sortA2(length_num, size);
	for (k=0;k<min_size;k++)
	{
		sum=sum+length_num[size-1-k];
	}
	min=sum/min_size;
	return min;
}
