#include "Key.h"

void Key_Init(void)
{
}

uint8_t Key_GetNum(void)
{
	uint8_t KEY_NUM = 0;

	if (KEY1_READ == 0)
	{
		Delay_ms(20);
		while (KEY1_READ == 0)
			;
		Delay_ms(20);
		KEY_NUM = KEY1_PRES;
	}
	if (KEY2_READ == 0)
	{
		Delay_ms(20);
		while (KEY2_READ == 0)
			;
		Delay_ms(20);
		KEY_NUM = KEY2_PRES;
	}
	if (KEY3_READ == 0)
	{
		Delay_ms(20);
		while (KEY3_READ == 0)
			;
		Delay_ms(20);
		KEY_NUM = KEY3_PRES;
	}
	if (KEY4_READ == 0)
	{
		Delay_ms(20);
		while (KEY4_READ == 0)
			;
		Delay_ms(20);
		KEY_NUM = KEY4_PRES;
	}
	if (KEY_MOVE_READ == 0)
	{
		Delay_ms(20);
		while (KEY_MOVE_READ == 0)
			;
		Delay_ms(20);
		KEY_NUM = KEY_MOVE_PRES;
	}
	if (KEY_DELETE_READ == 0)
	{
		Delay_ms(20);
		while (KEY_DELETE_READ == 0)
			;
		Delay_ms(20);
		KEY_NUM = KEY_DELETE_PRES;
	}

	return KEY_NUM;

}


void KEY_TEST(void)
{
	Keynum = Key_GetNum();
    if (Keynum == KEY1_PRES)
    {
      LED_TEST();
    }
    if(Keynum == KEY2_PRES)
    {
      LED1_ON();
      Delay_ms(3000);
      LED1_OFF();
    }
    if(Keynum==KEY3_PRES)
    {
      LED2_ON();
      Delay_ms(3000);
      LED2_OFF();
    }
    if(Keynum==KEY4_PRES)
    {
      LED3_ON();
      Delay_ms(3000);
      LED3_OFF();
    }
    if(Keynum==KEY_MOVE_PRES)
    {
      LED1_ON();
      LED2_ON();
      Delay_ms(3000);
      LED1_OFF();
      LED2_OFF();
    }
    if(Keynum==KEY_DELETE_PRES)
    {
      LED3_ON();
      LED2_ON();
      Delay_ms(3000);
      LED3_OFF();
      LED2_OFF();
    }
}
