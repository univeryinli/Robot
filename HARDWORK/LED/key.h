#ifndef __key_h
#define __key_h

#define key0  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5)
#define key1  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15)
#define wake_up GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)

#define key0_press  1
#define key1_press  2
#define wake_up_press  3

void key_init(void);
u8 key_scan(u8);

#endif







