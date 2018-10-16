#ifndef __hcsr04_h
#define __hcsr04_h
#include "sys.h"



void Hcsr04Init(void);
void hcsr04_NVIC(void);
u32 GetEchoTimer(void);
void sortA2(float a[], int length);
float Hcsr04GetLength(int j);
float Hcsr04_min_Length(void);
#endif