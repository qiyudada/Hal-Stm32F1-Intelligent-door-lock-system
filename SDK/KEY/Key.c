#include "Key.h"

int Input_PS[4] = {0};
int Initation_PS[4] = {4, 1, 2, 3};
void Key_Init(void)
{
}

uint8_t Key_GetNum(void)
{
	uint8_t KEY_NUM = 0;

	if (KEY1_READ == RESET)
	{
		Delay_ms(20);
		while (KEY1_READ == RESET)
			;
		Delay_ms(20);
		KEY_NUM = KEY1_PRES;
	}
	if (KEY2_READ == RESET)
	{
		Delay_ms(20);
		while (KEY2_READ == RESET)
			;
		Delay_ms(20);
		KEY_NUM = KEY2_PRES;
	}
	if (KEY3_READ == RESET)
	{
		Delay_ms(20);
		while (KEY3_READ == RESET)
			;
		Delay_ms(20);
		KEY_NUM = KEY3_PRES;
	}
	if (KEY4_READ == RESET)
	{
		Delay_ms(20);
		while (KEY4_READ == RESET)
			;
		Delay_ms(20);
		KEY_NUM = KEY4_PRES;
	}
	if (KEY_MOVE_READ == RESET)
	{
		Delay_ms(20);
		while (KEY_MOVE_READ == RESET)
			;
		Delay_ms(20);
		KEY_NUM = KEY_MOVE_PRES;
	}
	if (KEY_Confirm_READ == RESET)
	{
		Delay_ms(20);
		while (KEY_Confirm_READ == RESET)
			;
		Delay_ms(20);
		KEY_NUM = KEY_Confirm;
	}
	return KEY_NUM;
}

uint8_t Password_Match(int *Origin_Password)
{
	int Back_num = -1;
	Back_num = memcmp(Origin_Password, Input_PS, sizeof(Input_PS));
	memset(Input_PS, 0, sizeof(Input_PS));
	if (Back_num == 0)
	{
		printf("Password Match\r\n");
		return PassWord_True;
	}
	else
	{
		printf("Password Not Match\r\n");
		return PassWord_False;
	}
}

void Write_PassWord(void)
{
	uint32_t Total_Timeout = Key_Timeout;
	uint32_t Small_Timeout = Key_Delay;
	uint8_t WritePointer = 0;
	uint8_t CurrentPointer = 0;
	uint8_t Key_Input = 0;
	while (Total_Timeout--)
	{
		Key_Input = Key_GetNum();
		if (Key_Input != KEY_NoPRES && Key_Input != KEY_MOVE_PRES && Key_Input != KEY_Confirm)
		{
			Input_PS[WritePointer] = Key_Input;
			LCD_ShowIntNum(24 + WritePointer * 16, 96, Input_PS[WritePointer], 1, WHITE, BLACK, LCD_8x16);
			Delay_ms(200);
			LCD_Fill(24 + WritePointer * 16, 96, (24 + WritePointer * 16) + 8, 96 + 16, WHITE);
			LCD_ShowChar(24 + WritePointer * 16, 96, '*', BLACK, WHITE, LCD_8x16, 0);
			WritePointer++;
			CurrentPointer = WritePointer;
			Total_Timeout = Key_Timeout;
			Key_Input = 0;
		}
		if (Key_Input == KEY_MOVE_PRES)//Enter modify mode
		{
			CurrentPointer = (++CurrentPointer) % 4;
			LCD_ShowChar(24 + CurrentPointer * 16, 112, '^', BLACK, WHITE, LCD_8x16, 0);
			while (Small_Timeout--)
			{
				Key_Input = Key_GetNum();
				if (Key_Input == KEY_MOVE_PRES)
				{
					Key_Input = KEY_NoPRES;
					LCD_Fill(24 + CurrentPointer * 16, 112, (24 + CurrentPointer * 16) + 8, 112 + 16, WHITE);
					CurrentPointer = (++CurrentPointer) % 4;
					LCD_ShowChar(24 + CurrentPointer * 16, 112, '^', BLACK, WHITE, LCD_8x16, 0);
					Small_Timeout = Key_Delay;
					Key_Input = Key_GetNum();
				}
				if (Key_Input != KEY_NoPRES && Key_Input != KEY_MOVE_PRES && Key_Input != KEY_Confirm && Input_PS[CurrentPointer] != 0)
				{
					Input_PS[CurrentPointer] = Key_Input;
					LCD_ShowIntNum(24 + CurrentPointer * 16, 96, Input_PS[CurrentPointer], 1, WHITE, BLACK, LCD_8x16);
					Delay_ms(200);
					LCD_Fill(24 + CurrentPointer * 16, 96, (24 + CurrentPointer * 16) + 8, 96 + 16, WHITE);
					LCD_ShowChar(24 + CurrentPointer * 16, 96, '*', BLACK, WHITE, LCD_8x16, 0);
					Small_Timeout = Key_Delay;
					Key_Input = KEY_NoPRES;
				}
				else if (Key_Input == KEY_Confirm)
				{
					LCD_Fill(0, 112, 240, 112 + 16, WHITE);
					CurrentPointer = WritePointer;
					Key_Input = KEY_NoPRES;
					Delay_ms(200);
					break;
				}
			}
		}
		if (WritePointer >= 4 || Key_Input == KEY_Confirm)
		{
			WritePointer = 0;
			CurrentPointer = 0;
			break;
		}
	}
}

uint8_t KEY_Judge_Unlock(int *Origin_Password)
{
	Write_PassWord();
	return Password_Match(Origin_Password);
}

void KEY_FeedBack(int *Origin_Password)
{
	uint8_t ret = 0;
	printf("Please input the password:");
	LCD_Fill(-10, -10, 240, 240, WHITE);
	LCD_ShowString(0, 64, "Your Password:", WHITE, BLACK, LCD_8x16, 0);
	ret = KEY_Judge_Unlock(Origin_Password);
	if (ret == PassWord_True)
	{
		Door_Open();
		LCD_Fill(-10, -10, 240, 240, WHITE);
	}
	else
	{
		Door_Error();
		LCD_Fill(-10, -10, 240, 240, WHITE);
	}
}

void KEY_TEST(void)
{
	Keynum = Key_GetNum();
	if (Keynum == KEY1_PRES)
	{
		LED_TEST();
	}
	if (Keynum == KEY2_PRES)
	{
		LED1_ON();
		Delay_ms(3000);
		LED1_OFF();
	}
	if (Keynum == KEY3_PRES)
	{
		LED2_ON();
		Delay_ms(3000);
		LED2_OFF();
	}
	if (Keynum == KEY4_PRES)
	{
		LED3_ON();
		Delay_ms(3000);
		LED3_OFF();
	}
	if (Keynum == KEY_MOVE_PRES)
	{
		LED1_ON();
		LED2_ON();
		Delay_ms(3000);
		LED1_OFF();
		LED2_OFF();
	}
	if (Keynum == KEY_Confirm)
	{
		LED3_ON();
		LED2_ON();
		Delay_ms(3000);
		LED3_OFF();
		LED2_OFF();
	}
}
