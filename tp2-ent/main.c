#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include "lcd.h"
#include "keypad.h"
#include <stdlib.h>

typedef enum {
	STATE_INIT,
	STATE_GENERATE_NUMBER,
	STATE_GET_GUESS,
	STATE_CHECK_GUESS,
	STATE_WIN
} GameState;

void Timer_init(){
	TCCR0B = (1 << CS01) | (1 << CS00);
	TCNT0 = 6;
	TIMSK0 |= (1<<TOIE0);
}

uint32_t tiempo = 0;

ISR(TIMER0_OVF_vect){
	TCNT0 = 6;
	PORTB ^= (1 << PORTB5);
	tiempo++;
}

int main(void) {
	LCDinit();
	_delay_ms(1);

	LCDclr();
	_delay_ms(1);

	KEYPAD_Init();
	_delay_ms(1);
	
	Timer_init();
	DDRB |= (1<<PORTB5);
	
	uint8_t tecla;
	uint8_t presiono;

	//srand(time(NULL));
	GameState currentState = STATE_INIT;
	uint8_t secret_number_array[2];
	
	uint8_t msj_init[] = "P/JUGAR PULSE A";
	uint8_t msj_jugando[] = "JUGANDO";
	uint8_t msj_ingrese_num[] = "INGRESE NUM";
	uint8_t msj_lower[] = ">";
	uint8_t msj_higher[] = "<";
	uint8_t msj_fuera_de_rango[] = "FUERA DE RANGO";
	uint8_t msj_ganador[] = "GANADOR";
	uint8_t msj_time[] = "TIME: ";
	
	char guess_buffer[4];
	uint8_t buffer_index = 0;

	while (1) {

		switch(currentState){
			case STATE_INIT:
			{
				LCDGotoXY(0, 0);
				LCDstring(msj_init, sizeof(msj_init)-1);
				presiono = KEYPAD_Scan(&tecla);
				if (presiono){
					if (tecla == 65){
						currentState = STATE_GENERATE_NUMBER;
						} else {
						LCDGotoXY(0,1);
						LCDsendChar(tecla);
					}
				}
			}
			break;
			case STATE_GENERATE_NUMBER:
			{

				LCDclr();
				LCDGotoXY(0,0);
				LCDstring(msj_jugando, sizeof(msj_jugando)-1);
				LCDGotoXY(0,1);
				LCDstring(msj_ingrese_num, sizeof(msj_ingrese_num)-1);
				_delay_ms(100);
				tiempo = 0;
				sei();
				srand(TCNT0);
				secret_number_array[0] = rand() % 10;
				secret_number_array[1] = rand() % 10;
				_delay_ms(3000);
				LCDclr();
				currentState = STATE_GET_GUESS;
			}
			break;
			case STATE_GET_GUESS:
			{
				
				presiono = KEYPAD_Scan(&tecla);
				if (presiono){
					if(tecla>=48 && tecla<=57){
						LCDGotoXY(buffer_index,0);
						LCDsendChar(tecla);
						guess_buffer[buffer_index] = tecla;
						buffer_index++;
						if (buffer_index == 3){
							LCDGotoXY(0,0);
							LCDstring(msj_fuera_de_rango, sizeof(msj_fuera_de_rango)-1);
							buffer_index = 0;
						}
					}
					if (tecla == 65)
					{
						buffer_index = 0;
						currentState = STATE_CHECK_GUESS;
					}
					if (tecla == 68)
					{
						buffer_index = 0;
						currentState = STATE_INIT;
					}
					_delay_ms(300);
				}
			}
			break;
			case STATE_WIN:
			{
				LCDclr();
				LCDGotoXY(0, 0);
				LCDstring(msj_ganador, sizeof(msj_ganador)-1);
				LCDGotoXY(0, 1);

				uint32_t milliseconds = tiempo % 1000;
				uint32_t seconds = (tiempo / 1000) % 60;
				uint32_t minutes = (tiempo / (1000 * 60)) % 60;

				LCDstring(msj_time, sizeof(msj_time)-1);
				LCDescribeDato(minutes, 2);
				LCDsendChar(':');
				LCDescribeDato(seconds, 2);
				LCDsendChar(':');
				LCDescribeDato(milliseconds, 3);
				cli();
				_delay_ms(3000);
				LCDclr();
				currentState = STATE_INIT;
				
			}
			break;

			case STATE_CHECK_GUESS:
			{
				uint8_t guess[2];
				guess[0] = guess_buffer[0] - '0';
				guess[1] = guess_buffer[1] - '0';

				if (guess[0] == secret_number_array[0] && guess[1] == secret_number_array[1]) {
					currentState = STATE_WIN;
					} else {
					uint8_t firstDigitChecked = 0;
					if (guess[0] < secret_number_array[0]) {
						LCDstring(msj_lower, sizeof(msj_lower) - 1);
						LCDcursorLeft(1);
						firstDigitChecked = 1;
						} else if (guess[0] > secret_number_array[0]) {
						LCDstring(msj_higher, sizeof(msj_higher) - 1);
						LCDcursorLeft(1);
						firstDigitChecked = 1;
					}

					if (!firstDigitChecked) {
						if (guess[1] < secret_number_array[1]) {
							LCDstring(msj_lower, sizeof(msj_lower) - 1);
							} else if (guess[1] > secret_number_array[1]) {
							LCDstring(msj_higher, sizeof(msj_higher) - 1);
						}
					}
					currentState = STATE_GET_GUESS;
				}
			}
			break;
		}
	}
}