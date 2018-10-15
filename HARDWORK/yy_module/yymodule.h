#ifndef __yymodule_h
#define __yymodule_h
#include "usart.h"

extern u8 USART3_RX_BUF;
extern u8 USART3_RX_STA;  
void My_USART3_Init(void);
void choose(u8 a,u8 b,u8 ch);

#endif

