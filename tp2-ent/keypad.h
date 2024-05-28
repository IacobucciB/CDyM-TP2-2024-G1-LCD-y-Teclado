#ifndef KEYPAD_H_
#define KEYPAD_H_

#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#define F_CPU 16000000UL
#include <util/delay.h>

void KEYPAD_Init();
uint8_t KeypadUpdate(void);
uint8_t KEYPAD_Scan_p(uint8_t *pkey);
uint8_t KEYPAD_Scan(uint8_t *key);

#endif /* KEYPAD_H_ */