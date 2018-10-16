#include "hcsr04.h"
#include "usart.h"
#include "delay.h"

#define HCSR04_PORT     GPIOB
#define HCSR04_CLK      RCC_APB2Periph_GPIOB
#define HCSR04_TRIG     GPIO_Pin_9
#define HCSR04_ECHO     GPIO_Pin_4|GPIO_Pin_5
 
#define TRIG_Send  PBout(5)
#define ECHO_Reci  PBin(4)
 
u16 msHcCount = 0;//ms计数

/*
if you want detect obj,so you can use flag is_obj. 
*/
int is_obj;

void Hcsr04Init()
{  
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;     //生成用于定时器设置的结构体
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(HCSR04_CLK, ENABLE);
     
        //IO初始化
    GPIO_InitStructure.GPIO_Pin =HCSR04_TRIG;       //发送电平引脚
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
    GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(HCSR04_PORT,HCSR04_TRIG);
     
    GPIO_InitStructure.GPIO_Pin =   HCSR04_ECHO;     //返回电平引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);  
		GPIO_ResetBits(HCSR04_PORT,HCSR04_ECHO);	
	 
			//定时器初始化 使用基本定时器TIM6
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);   //使能对应RCC时钟
		//配置定时器基础结构体
		TIM_DeInit(TIM6);
		TIM_TimeBaseStructure.TIM_Period = (1000-1); //设置在下一个更新事件装入活动的自动重装载寄存器周期的值         计数到1000为1ms
		TIM_TimeBaseStructure.TIM_Prescaler =(72-1); //设置用来作为TIMx时钟频率除数的预分频值  1M的计数频率 1US计数
		TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;//不分频
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
		TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位		 
		
		TIM_ClearFlag(TIM6, TIM_FLAG_Update);   //清除更新中断，免得一打开中断立即产生中断
		TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);    //打开定时器更新中断
		hcsr04_NVIC();
    TIM_Cmd(TIM6,DISABLE);      
}
 
//tips：static函数的作用域仅限于定义它的源文件内，所以不需要在头文件里声明
void OpenTimerForHc()        //打开定时器
{
        TIM_SetCounter(TIM6,0);//清除计数
        msHcCount = 0;
        TIM_Cmd(TIM6, ENABLE);  //使能TIMx外设
//	      printf("ok1\n");
}
 
void CloseTimerForHc()        //关闭定时器
{
        TIM_Cmd(TIM6, DISABLE);  //使能TIMx外设
//	      printf("ok2\n");
}
 
 
 //NVIC配置
void hcsr04_NVIC()
{
			NVIC_InitTypeDef NVIC_InitStructure;
			NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
			NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;             //选择串口1中断
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //抢占式中断优先级设置为1
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;         //响应式中断优先级设置为1
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;        //使能中断
			NVIC_Init(&NVIC_InitStructure);
}
 
 
//定时器6中断服务程序
void TIM6_IRQHandler(void)   //TIM3中断
{
//  printf("ok\n");
	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
        {
//						printf("ok\n");
						TIM_ClearITPendingBit(TIM6, TIM_IT_Update  );  //清除TIMx更新中断标志 
						msHcCount++;
//					  printf("%d\n",msHcCount);
        }
}
 
 
//获取定时器时间
u32 GetEchoTimer(void)
{
        u32 t = 0;
        t = msHcCount*1000;//得到MS
        t += TIM_GetCounter(TIM6);//得到US
//				printf("%d\n",msHcCount);
	      TIM6->CNT = 0;  //将TIM2计数寄存器的计数值清零
//	      printf("ok3\n");
				delay_ms(30);
        return t;
}
 

//冒泡排序，从大到小
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
//一次获取超声波测距数据 两次测距之间需要相隔一段时间，隔断回响信号
//为了消除余震的影响，取五次数据的平均值进行加权滤波。
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
		printf("%d号超声波开始检测\n\n",j+1);
//		printf("%djjj",s);
		while(i<5)
		{
		PBout(5) = 1;      //发送口高电平输出
		delay_us(20);
		PBout(5) = 0;
		
		while(PBin((0+4)) == 0);      //等待接收口高电平输出
		OpenTimerForHc();        //打开定时器
		printf("ok");
		i = i + 1;
		while(PBin((0+4)) == 1);
		CloseTimerForHc();        //关闭定时器
		t = GetEchoTimer();        //获取时间,分辨率为1US
		lengthTemp = ((float)t/58.0);//cm
		sum = lengthTemp + sum ;
		}
		lengthTemp = sum/5.0;
		length[j]=lengthTemp;
	  printf("%d号超声波数值=%.3fcm\n\n",j+1,lengthTemp);		  
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

//一次获取超声波测距数据 两次测距之间需要相隔一段时间，隔断回响信号
//为了消除余震的影响，取五次数据的平均值进行加权滤波。
float Hcsr04GetLength(int j)
{
	u32 t = 0;
	int i;
	float sum=0,lengthTemp;
	for(i=0;i<5;i++)
	{
		PBout(9) = 1;      //发送口高电平输出
		delay_us(20);
		PBout(9) = 0;
		while(PBin((4+j)) == 0);      //等待接收口高电平输出
		OpenTimerForHc();        //打开定时器
		while(PBin((4+j)) == 1);
		CloseTimerForHc();        //关闭定时器
		t = GetEchoTimer();        //获取时间,分辨率为1US
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
//		printf("%d号超声波开始检测\n\n",j+1);
		length=Hcsr04GetLength(j);
		length_num[j]=length;
//		printf("%d号超声波数值=%.3fcm\n\n",j+1,length);	
		
	}
	sortA2(length_num, size);
	for (k=0;k<min_size;k++)
	{
		sum=sum+length_num[size-1-k];
	}
	min=sum/min_size;
	return min;
}
