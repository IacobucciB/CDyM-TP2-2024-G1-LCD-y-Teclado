// keypad.c

#include "keypad.h"

static const uint8_t caracteres[4][4] = {
	{'1', '2', '3', 'A'} ,
	{'4', '5', '6', 'B'} ,
	{'7', '8', '9', 'C'} ,
	{'*', '0', '#', 'D'}
};

static const uint8_t filas[3] = {PINB4, PINB3, PINB0};
static const uint8_t columnas[4] = {PORTD3, PORTD5, PORTD4, PORTD2};

void KEYPAD_Init(){
	// PULL-UP
	DDRB &= ~((1<<PORTB0) | (1<<PORTB3) | (1<<PORTB4));
	DDRD &= ~(1<<PORTD7);
	PORTB |= (1<<PORTB0) | (1<<PORTB3) | (1<<PORTB4);
	PORTD |= (1<<PORTD7);
	
	DDRD |= (1 << PORTD2) | (1<<PORTD3) | (1<<PORTD4) | (1<<PORTD5);
	PORTD &= ~((1<< PORTD2) | (1<<PORTD3) | (1<<PORTD4) | (1<<PORTD5));
}

uint8_t KeypadUpdate(void){
	uint8_t r,c;
	PORTB |= 0X0F;
	for(c=0;c<4;c++){
		DDRB&=~(0xFF);
		DDRB|=(0x80>>c);
		for(r=0;r<4;r++){
			if(!(PINB & (0x08>>r))){
				return (r*4+c);
			}
		}
	}
	return 0xFF;
}

uint8_t KEYPAD_Scan_p(uint8_t *pkey) {
	static uint8_t Old_key = 0xFF, Last_valid_key = 0xFF;
	uint8_t Key;
	
	Key = KeypadUpdate();
	
	if (Key == 0xFF) {
		Old_key = 0xFF;
		Last_valid_key = 0xFF;
		return 0;
	}
	
	if (Key == Old_key) {
		if (Key != Last_valid_key) {
			*pkey = Key;
			Last_valid_key = Key;
			return 1;
		}
	}
	
	Old_key = Key;
	return 0;
}

uint8_t KEYPAD_Scan(uint8_t *key){
	uint8_t reposo;
	uint8_t presiono = 0;
	for (uint8_t c=0; c < 4; c++){
		DDRD |= (1<<columnas[c]);
		PORTD &= ~(1<<columnas[c]);
		for (uint8_t f=0; f < 3; f++){
			reposo = PINB & (1<<filas[f]);
			if (!reposo){
				*key = caracteres[f][c];
				presiono = 1;
			}
		}
		reposo = PIND & (1<<PIND7);
		if (!reposo){
			*key = caracteres[3][c];
			presiono = 1;
		}
		PORTD |= (1<<columnas[c]);
		DDRD &= ~(1<<columnas[c]);
	}
	return presiono;
}
