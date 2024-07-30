#include "LED.h"
#include "Delay.h"

void LED_Init(void) // initation
{
}

void LED1_ON(void)
{
	LED_RED_ON;
}

void LED1_OFF(void)
{
	LED_RED_OFF;
}

void LED2_ON(void)
{
	LED_WHTIE_ON;
}

void LED2_OFF(void)
{
	LED_WHTIE_OFF;
}

void LED3_ON(void)
{
	LED_GREEN_ON;
}

void LED3_OFF(void)
{
	LED_GREEN_OFF;
}

void LED_TEST(void)
{
	LED1_ON();
	Delay_ms(1000);
	LED1_OFF();
	Delay_ms(1000);
	LED2_ON();
	Delay_ms(1000);
	LED2_OFF();
	Delay_ms(1000);
	LED3_ON();
	Delay_ms(1000);
	LED3_OFF();
	Delay_ms(1000);
}