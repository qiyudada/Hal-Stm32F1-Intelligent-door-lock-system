#include "Key.h"

int Input_PS[4] = {0};	   // Input password
int Initation_PS[4] = {0}; // Initation password
int Add_FP[4] = {1, 1, 4, 4};
/**********************************************************************
 * @name Key_Init
 * @note Key Initation(already complete on gpio.c)
 * @param None
 * @return None
 ***********************************************************************/
void Key_Init(void)
{
}

/**********************************************************************
 * @name Key_GetNum
 * @note Get Key press number
 * @param None
 * @return (uint8_t)Keynum(delcared in main.h)
 ***********************************************************************/
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
/**********************************************************************
 * @name Password_Match
 * @note Match the password between input and origin
 * @param None
 * @return (PassWord_True or PassWord_False | 7 or 8)
 ***********************************************************************/
uint8_t Password_Match(void)
{
	int Back_num1 = -3;
	int Back_num2 = -3;
	Back_num1 = memcmp(Initation_PS, Input_PS, sizeof(Input_PS)); // Compare the password
	Back_num2 = memcmp(Add_FP, Input_PS, sizeof(Input_PS));
	memset(Input_PS, 0, sizeof(Input_PS));
	if (Back_num1 == 0 || Back_num2 == 0)
	{
		if (Back_num1 == 0)
		{
			printf("Password Match\r\n");
			return PassWord_True;
		}
		if (Back_num2 == 0)
		{
			printf("Add Fingerprint\r\n");
			return PassWord_FingerAdd;
		}
	}
	else
	{
		printf("Password Not Match\r\n");
		return PassWord_False;
	}
}
/**********************************************************************
 * @name Write_PassWord
 * @note Writing Password
 * @param None
 * @return None
 ***********************************************************************/
void Write_PassWord(void)
{
	uint32_t Total_Timeout = Key_Timeout;
	uint32_t Small_Timeout = Key_Delay;
	uint8_t WritePointer = 0; // double pointer
	uint8_t CurrentPointer = 0;
	uint8_t Key_Input = 0;
	while (Total_Timeout--) // waitting timeout for quit menu
	{
		Key_Input = Key_GetNum();
		if (Key_Input != KEY_NoPRES && Key_Input != KEY_MOVE_PRES && Key_Input != KEY_Confirm)
		{
			Input_PS[WritePointer] = Key_Input;
			LCD_ShowIntNum(24 + WritePointer * 16, 96, Input_PS[WritePointer], 1, BLACK, WHITE, LCD_8x16);
			Delay_ms(200);
			LCD_Fill(24 + WritePointer * 16, 96, (24 + WritePointer * 16) + 8, 96 + 16, WHITE);
			LCD_ShowChar(24 + WritePointer * 16, 96, '*', BLACK, WHITE, LCD_8x16, 0);
			WritePointer++;
			CurrentPointer = WritePointer;
			Total_Timeout = Key_Timeout;
			Key_Input = 0;
		}
		if (Key_Input == KEY_MOVE_PRES) // Enter modify mode
		{
			CurrentPointer = (++CurrentPointer) % 4;									 // prevent the array index out of range
			LCD_ShowChar(24 + CurrentPointer * 16, 112, '^', BLACK, WHITE, LCD_8x16, 0); // locate the position
			while (Small_Timeout--)
			{
				Key_Input = Key_GetNum();
				if (Key_Input == KEY_MOVE_PRES)
				{
					Key_Input = KEY_NoPRES;
					LCD_Fill(24 + CurrentPointer * 16, 112, (24 + CurrentPointer * 16) + 8, 112 + 16, WHITE);
					CurrentPointer = (++CurrentPointer) % 4;
					LCD_ShowChar(24 + CurrentPointer * 16, 112, '^', BLACK, WHITE, LCD_8x16, 0);
					Small_Timeout = Key_Delay; // reset the timeout
					Key_Input = Key_GetNum();  // Speed up the modify expediently
				}
				if (Key_Input != KEY_NoPRES && Key_Input != KEY_MOVE_PRES && Key_Input != KEY_Confirm && Input_PS[CurrentPointer] != 0)
				{
					Input_PS[CurrentPointer] = Key_Input; // change the passwords which user is pitches on
					LCD_ShowIntNum(24 + CurrentPointer * 16, 96, Input_PS[CurrentPointer], 1, BLACK, WHITE, LCD_8x16);
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
/**********************************************************************
 * @name KEY_Change
 * @note Change Password
 * @param None
 * @return None
 ***********************************************************************/
void KEY_Change(void)
{
	uint8_t Key_Input = 0;
	int Temp1_Password[4] = {0}; // first input password which user want to change
	int Temp2_Password[4] = {0}; // second input password which user want to change
	uint32_t Total_Timeout = Key_Timeout;
	uint32_t Small_Timeout = Key_Delay;
	uint8_t WritePointer = 0;
	uint8_t CurrentPointer = 0;
	LCD_Fill(-10, -10, 240, 240, WHITE);
	LCD_ShowString(0, 64, "First Password:", BLACK, WHITE, LCD_8x16, 0);
	/**********First Password*******************/
	while (Total_Timeout--)
	{
		Key_Input = Key_GetNum();
		if (Key_Input != KEY_NoPRES && Key_Input != KEY_MOVE_PRES && Key_Input != KEY_Confirm)
		{
			Temp1_Password[WritePointer] = Key_Input; // writepointer fellow the key input
			LCD_ShowIntNum(24 + WritePointer * 16, 96, Temp1_Password[WritePointer], 1, BLACK, WHITE, LCD_8x16);
			Delay_ms(200);
			LCD_Fill(24 + WritePointer * 16, 96, (24 + WritePointer * 16) + 8, 96 + 16, WHITE);
			LCD_ShowChar(24 + WritePointer * 16, 96, '*', BLACK, WHITE, LCD_8x16, 0);
			WritePointer++;
			CurrentPointer = WritePointer;
			Total_Timeout = Key_Timeout;
			Key_Input = 0;
		}
		if (Key_Input == KEY_MOVE_PRES) // Enter modify mode(other input add will be ignored and just change already input)
		{
			CurrentPointer = (++CurrentPointer) % 4; // prevent overflow
			LCD_ShowChar(24 + CurrentPointer * 16, 112, '^', BLACK, WHITE, LCD_8x16, 0);
			while (Small_Timeout--) // No control will be exit
			{
				Key_Input = Key_GetNum();
				if (Key_Input == KEY_MOVE_PRES)
				{
					Key_Input = KEY_NoPRES;
					LCD_Fill(24 + CurrentPointer * 16, 112, (24 + CurrentPointer * 16) + 8, 112 + 16, WHITE);
					CurrentPointer = (++CurrentPointer) % 4;
					LCD_ShowChar(24 + CurrentPointer * 16, 112, '^', BLACK, WHITE, LCD_12x24, 0);
					Small_Timeout = Key_Delay; // reset time
					Key_Input = Key_GetNum();
				}
				if (Key_Input != KEY_NoPRES && Key_Input != KEY_MOVE_PRES && Key_Input != KEY_Confirm && Temp1_Password[CurrentPointer] != 0)
				{
					Temp1_Password[CurrentPointer] = Key_Input;
					LCD_ShowIntNum(24 + CurrentPointer * 16, 96, Temp1_Password[CurrentPointer], 1, WHITE, BLACK, LCD_8x16);
					Delay_ms(200);
					LCD_Fill(24 + CurrentPointer * 16, 96, (24 + CurrentPointer * 16) + 8, 96 + 16, WHITE);
					LCD_ShowChar(24 + CurrentPointer * 16, 96, '*', BLACK, WHITE, LCD_8x16, 0);
					Small_Timeout = Key_Delay;
					Key_Input = KEY_NoPRES;
				}
				else if (Key_Input == KEY_Confirm)
				{
					LCD_Fill(0, 112, 240, 112 + 24, WHITE);
					CurrentPointer = WritePointer;
					Key_Input = KEY_NoPRES;
					Delay_ms(200);
					break;
				}
			}
		}
		if (WritePointer >= 4 || Key_Input == KEY_Confirm)
		{
			Key_Input = 0;
			WritePointer = 0;
			CurrentPointer = 0;
			Total_Timeout = Key_Timeout; // Time reset to avoid second input exit too fast
			Small_Timeout = Key_Delay;
			break;
		}
	}
	/**********Second Password*******************/
	LCD_Fill(-10, -10, 240, 240, WHITE);
	LCD_ShowString(0, 64, "Second Password:", BLACK, WHITE, LCD_8x16, 0);
	while (Total_Timeout--) // twice input
	{
		LCD_Fill(0, 112, 240, 112 + 16, WHITE);
		Key_Input = Key_GetNum();
		if (Key_Input != KEY_NoPRES && Key_Input != KEY_MOVE_PRES && Key_Input != KEY_Confirm)
		{
			Temp2_Password[WritePointer] = Key_Input;
			LCD_ShowIntNum(24 + WritePointer * 16, 96, Temp2_Password[WritePointer], 1, BLACK, WHITE, LCD_8x16);
			Delay_ms(200);
			LCD_Fill(24 + WritePointer * 16, 96, (24 + WritePointer * 16) + 8, 96 + 16, WHITE);
			LCD_ShowChar(24 + WritePointer * 16, 96, '*', BLACK, WHITE, LCD_8x16, 0);
			WritePointer++;
			CurrentPointer = WritePointer;
			Total_Timeout = Key_Timeout;
			Key_Input = 0;
		}
		if (Key_Input == KEY_MOVE_PRES) // Enter modify mode
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
				if (Key_Input != KEY_NoPRES && Key_Input != KEY_MOVE_PRES && Key_Input != KEY_Confirm && Temp2_Password[CurrentPointer] != 0)
				{
					Temp2_Password[CurrentPointer] = Key_Input;
					LCD_ShowIntNum(24 + CurrentPointer * 16, 96, Temp2_Password[CurrentPointer], 1, BLACK, WHITE, LCD_8x16);
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
			Key_Input = 0;
			WritePointer = 0;
			CurrentPointer = 0;
			break;
		}
	}
	if (memcmp(Temp1_Password, Temp2_Password, sizeof(Temp1_Password)) == 0) // Compare the two passwords
	{
		LCD_Fill(0, 64, 240, 80, WHITE);
		LCD_ShowString(0, 64, "Change Success!", BLACK, WHITE, LCD_8x16, 0);
		for (int i = 0; i < 4; i++)
		{
			Initation_PS[i] = Temp1_Password[i]; // Save the password
		}
		STMFLASH_Write(FLASH_SAVE_ADDR, (uint16_t *)Initation_PS, 8); // write to flash
		Delay_ms(2000);
		LCD_Fill(-10, -10, 240, 240, WHITE);
		LCD_ShowString(0, 64, "Your Password:", BLACK, WHITE, LCD_8x16, 0);
	}
}
/**********************************************************************
 * @name KEY_Judge_Unlock
 * @note Juage  Password right or not and decide to unlock or not
 * @param None
 * @return Password_Match(Origin_Password)
 ***********************************************************************/
uint8_t KEY_Judge_Unlock(void)
{
	if (Key_Change_num == KEY_Confirm)
	{
		Key_Change_num = 0;
		LCD_Fill(-10, -10, 240, 240, WHITE);
		LCD_ShowString(0, 64, "Change Password:", BLACK, WHITE, LCD_8x16, 0);
		KEY_Change();
	}
	Write_PassWord();
	return Password_Match();
}
/**********************************************************************
 * @name KEY_FeedBack
 * @note Control door open , the feedback of key
 * @param None
 * @return None
 ***********************************************************************/
void KEY_Feedback(void)
{
	uint8_t ret = 0;
	printf("Please input the password:");
	LCD_Fill(-10, -10, 240, 240, WHITE);
	LCD_ShowString(0, 64, "Your Password:", BLACK, WHITE, LCD_8x16, 0);
	ret = KEY_Judge_Unlock();
	if (ret == PassWord_True)
	{
		Door_Open();
		LCD_Fill(-10, -10, 240, 240, WHITE);
	}
	if (ret == PassWord_False)
	{
		Door_Error();
		LCD_Fill(-10, -10, 240, 240, WHITE);
	}
	if (ret == PassWord_FingerAdd)
	{
		Add_FR();
	}
}
/**********************************************************************
 * @name KEY_TEST
 * @note KEY test function
 * @param void
 * @return None
 * @details pay attention the Keynum need to declare in main.c
 ***********************************************************************/
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
