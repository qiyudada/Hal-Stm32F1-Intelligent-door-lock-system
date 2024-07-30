#ifndef __Delay_h
#define __Delay_h

#include "stm32f1xx_hal.h"

void Delay_us(uint32_t us);
void Delay_ms(uint32_t nms);
void Delay_s(uint32_t xs);

#endif