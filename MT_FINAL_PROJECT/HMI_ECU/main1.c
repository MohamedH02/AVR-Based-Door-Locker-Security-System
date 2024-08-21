

#include "lcd.h"
#include "keypad.h"
#include <avr/io.h>
#include "util/delay.h"

#define receive_NewPass 10
#define open_Door 11
#define wrong_Pass 12
#define check_pwd 13


uint8 pwd1[6],pwd2[6];
uint8 g_compare=0;
uint8 g_checkpwd=0;
uint8 g_unlock = 0;
uint8 g_tick=0;
uint8 g_changePass;
Timer1_ConfigType timer_config;


void MainOptions (void);

void tick(void){
	g_tick++;
}



void CreatePwd(void){

	uint8 i;

	LCD_displayString("Enter New Pass");
	LCD_moveCursor(1, 0);
	_delay_ms(250);
	for (i=0; i<5; i++){
		pwd1[i]=KEYPAD_getPressedKey()+48;
		_delay_ms(250);
		LCD_displayCharacter('*');
		_delay_ms(250);
	}
	_delay_ms(250);
	while(KEYPAD_getPressedKey() != '=');
	pwd1[i]='#';

	UART_sendByte(receive_NewPass);

	UART_sendString(pwd1);
	_delay_ms(10);
	LCD_clearScreen();

	LCD_moveCursor(0, 0);
	LCD_displayString("Plz Re-enter The");
	LCD_moveCursor(1, 0);
	LCD_displayString("Same Pass: ");
	_delay_ms(250);
	for (i=0; i<5; i++){
		pwd2[i]=KEYPAD_getPressedKey()+48;
		_delay_ms(250);
		LCD_displayCharacter('*');
		_delay_ms(250);
	}
	_delay_ms(250);
	while(KEYPAD_getPressedKey() != '=');
	pwd2[i]='#';



	UART_sendString(pwd2);
	_delay_ms(10);
	LCD_clearScreen();



	g_compare = UART_recieveByte();
	_delay_ms(15);
	if(g_compare == 1){
		return; }
	else if(g_compare == 0)
		CreatePwd();


}

void EnterPwd(void){

	LCD_clearScreen();
	LCD_displayString("Plz Enter Pass");
	_delay_ms(100);
	LCD_moveCursor(1, 0);
	uint8 i;
	_delay_ms(250);
	for (i=0; i<5; i++){
		pwd1[i]=KEYPAD_getPressedKey()+48;
		_delay_ms(250);
		LCD_displayCharacter('*');
		_delay_ms(250);
	}

	_delay_ms(250);
	while(KEYPAD_getPressedKey() != '=');
	pwd1[i]='#';

	UART_sendByte(check_pwd);

	UART_sendString(pwd1);

	g_compare = UART_recieveByte();

	while(g_checkpwd < 2){
		if(g_compare == 1){
			g_checkpwd = 0;
			return;
			}
		else if(g_compare == 0) {
			g_checkpwd++;
			EnterPwd();
		}
		if (g_checkpwd == 2)
		{
			UART_sendByte(wrong_Pass);

			LCD_clearScreen();
			LCD_displayStringRowColumn(0,0,"ERROR, wait!");
			LCD_displayStringRowColumn(1,3,"seconds");
			g_tick = 0;
			while (g_tick < 60)
			{
				LCD_displayStringRowColumn(0,0,"ERROR, wait!");
				LCD_displayStringRowColumn(1,3,"seconds");
				LCD_moveCursor(1,0);
				if ((60 - g_tick) > 10)
				{
				LCD_intgerToString(60 - g_tick);
				}
				else
				{
					LCD_displayStringRowColumn(1,0,"  ");
					LCD_moveCursor(1,0);
					LCD_intgerToString(60 - g_tick);
				}
			}
			g_checkpwd = 0;
			LCD_clearScreen();
			MainOptions();

		}

	}



}

void MainOptions (void){

	 g_changePass = 1;

	LCD_displayString("+ : Open Door");
	LCD_moveCursor(1, 0);
	LCD_displayString("- : Change pass");

	if (KEYPAD_getPressedKey() == '+'){

		_delay_ms(200);
		EnterPwd();

		if (g_changePass == 0){
			return;
		}

		UART_sendByte(open_Door);
		g_tick=0;

		LCD_clearScreen();

		LCD_displayString("door unlocking");

		while (g_tick <= 15){}


        g_tick=0;
		LCD_clearScreen();
		LCD_displayString("....");
		while (g_tick <= 3);

		g_tick=0;

		LCD_clearScreen();
		LCD_displayString("door locking");
		while (g_tick <=15){}

		LCD_clearScreen();


	}

	else if (KEYPAD_getPressedKey() == '-'){

		g_changePass = 0;
		EnterPwd();
		LCD_clearScreen();
		CreatePwd();



	}

}


int main(){
	SREG|=(1<<7);


	LCD_init();

	UART_ConfigType uart_config;
	uart_config.baud_rate=9600;
	uart_config.bit_data=bit_8;
	uart_config.parity=Disabled;
	uart_config.stop_bit=one;
	UART_init(&uart_config);

	timer_config.initial_value=0;
	timer_config.mode=COMPARE_MODE;
	timer_config.prescaler=PRESCALER_1024;
	timer_config.compare_value=7812;
	Timer1_init(&timer_config);
	Timer1_setCallBack(tick);

	CreatePwd();


	while(1){

		MainOptions();



	}
}
