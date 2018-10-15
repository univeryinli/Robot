#ifndef __CAR_H
#define __CAR_H
#include "sys.h"


#define IN1 PBout(12)
#define IN2 PBout(13)
#define IN3 PBout(14)
#define IN4 PBout(15)
#define ENA PAout(6)
#define ENB PAout(7)



void CAR_Init(void);
void go(void);
void back(void);
void left(void);
void right(void);
void stop(void);

void go_speed(u16 arr,u16 psc,u16 ccr);
void back_speed(u16 arr,u16 psc,u16 ccr);
void left_speed(u16 arr,u16 psc,u16 ccr);
void right_speed(u16 arr,u16 psc,u16 ccr);

#endif

