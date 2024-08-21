#include "buzzer.h"
#include "external_eeprom.h"
#include "dc_motor.h"
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>

#define receive_NewPass 10
#define open_Door 11
#define wrong_Pass 12
#define check_pwd 13

uint8 pwd1[6],pwd2[6];
uint8 eepromPass[6];
uint8 g_tick=0;
Timer1_ConfigType timer_config;

void tick(void){
	g_tick++;
}

void receiveNewPass(void){


	UART_receiveString(pwd1);
	_delay_ms(15);
	UART_receiveString(pwd2);
    _delay_ms(10);


	if(!(strcmp(pwd1,pwd2))){




		UART_sendByte(1);
		_delay_ms(15);

		uint8 i;

		for(i=0; i<5; i++)
		{
			EEPROM_writeByte(0x0311+i, pwd1[i]);
			_delay_ms(10);

		}
	}

	else
		UART_sendByte(0);

}

void checkpwd(void){

	uint8 eepromPass[6];

	uint8 i;

	UART_receiveString(pwd2);


	for(i=0; i<5 ; i++)
	{
		EEPROM_readByte(0x0311+i, (eepromPass+i));
		_delay_ms(10);

	}
	eepromPass[5]='\0';

	if(!(strcmp(eepromPass,pwd2))){

		UART_sendByte(1); }
	else {
		UART_sendByte(0);

	}

}


void openDoor(void){

	g_tick=0;

	DcMotor_Rotate(CLOCKWISE, 100);
	while(g_tick<=15){}

	g_tick=0;
	DcMotor_Rotate(STOP, 0);
	while(g_tick<= 3){}

	g_tick=0;
	DcMotor_Rotate(ANTI_CLOCKWISE, 100);
	while(g_tick<=15){}


	DcMotor_Rotate(STOP, 0);


}





void wrongPass(void){

	g_tick=0;

	Buzzer_on();
	while(g_tick<=60){	}
	Buzzer_off();

}



int main(){

	DDRC|=(1<<7);    //debug led

	uint8 x = 0;
	SREG|=(1<<7);

	UART_ConfigType uart_config;
	uart_config.baud_rate=9600;
	uart_config.bit_data=bit_8;
	uart_config.parity=Disabled;
	uart_config.stop_bit=one;
	UART_init(&uart_config);

	TWI_ConfigType twi_config;
	twi_config.address = 0x01;
	twi_config.bit_rate = 2;
	TWI_init(&twi_config);

	timer_config.initial_value=0;
	timer_config.mode=COMPARE_MODE;
	timer_config.prescaler=PRESCALER_1024;
	timer_config.compare_value=7812;
	Timer1_init(&timer_config);
	Timer1_setCallBack(tick);


	DcMotor_Init();

	Buzzer_init();


	while(1){

		x = UART_recieveByte();

		switch(x)
		{
		case receive_NewPass:
			receiveNewPass();
			break;

		case open_Door:
			openDoor();
			break;

		case wrong_Pass:
			wrongPass();
			break;

		case check_pwd:
			checkpwd();
			break;
		}



	}

}

