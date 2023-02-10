/*
 * GccApplication4.c
 *
 * Created: 2023-02-10 3:52:06 PM
 * Author : vicga
 */ 
#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "uart.h"

char String[25];

char word[6] = {'V', 'I', 'C', 'T', 'O', 'R'};
	
#include <string.h>

char* encode(char c) {
	switch (c) {
		case 'A': return ".-";
		case 'B': return "-...";
		case 'C': return "-.-.";
		case 'D': return "-..";
		case 'E': return ".";
		case 'F': return "..-.";
		case 'G': return "--.";
		case 'H': return "....";
		case 'I': return "..";
		case 'J': return ".---";
		case 'K': return "-.-";
		case 'L': return ".-..";
		case 'M': return "--";
		case 'N': return "-.";
		case 'O': return "---";
		case 'P': return ".--.";
		case 'Q': return "--.-";
		case 'R': return ".-.";
		case 'S': return "...";
		case 'T': return "-";
		case 'U': return "..-";
		case 'V': return "...-";
		case 'W': return ".--";
		case 'X': return "-..-";
		case 'Y': return "-.--";
		case 'Z': return "--..";
		case '0': return "-----";
		case '1': return ".----";
		case '2': return "..---";
		case '3': return "...--";
		case '4': return "....-";
		case '5': return ".....";
		case '6': return "-....";
		case '7': return "--...";
		case '8': return "---..";
		case '9': return "----.";
		default: return "";
	}
}


void Initialize(){

	cli();
	DDRB |= (1<<DDB1); //Configure PB1 to output
	sei();
}




int main(void)
{
    /* Replace with your application code */
	Initialize();

	
    while (1) 
    {
		
		for(int i = 0; i < sizeof(word); ++i){
	
			
			char* morse = encode(word[i]);
			for(int j = 0; j < strlen(morse); ++j){
				if(morse[j] == '-'){
					PORTB |= (1<<PORTB1); //Set PB1 to high
					_delay_ms(500); //500ms pulse for a dash
					PORTB &= ~(1<<PORTB1); //Set PB1 to low
				}
				else{
					PORTB |= (1<<PORTB1); //Set PB1 to high
					_delay_ms(100); //100ms pulse for a dot
					PORTB &= ~(1<<PORTB1); //Set PB1 to low
				}
				_delay_ms(100); //Space between pulses
			}
			_delay_ms(500); //Space between letters
		}
		_delay_ms(5000); //Space between iterations
    }
}
