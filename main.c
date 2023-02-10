#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

/*--------------------Libraries---------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>


#include "uart.h"

/*--------------------Variables---------------------------*/
char String[25];
volatile unsigned long int edge1 = 0;
volatile unsigned long int edge2 = 0;
volatile unsigned long int duration = 0;
volatile int overflow = 0;
volatile unsigned long int space_ms = 0;
volatile unsigned long int space = 0;
int isSpace = 0;

volatile int counter = 0;

char ANS = '0';

char letter[6] = {0, 0, 0, 0, 0, 0};
char dash = '-';
char dot = '.';

/*-----------------------------------------------------------*/

//Function that takes in a char array of dots and dashes, returns a letter
char decode(char arr[5]){
	
	if (strcmp(arr, ".-") == 0) return 'A';
	if (strcmp(arr, "-...") == 0) return 'B';
	if (strcmp(arr, "-.-.") == 0) return 'C';
	if (strcmp(arr, "-..") == 0) return 'D';
	if (strcmp(arr, ".") == 0) return 'E';
	if (strcmp(arr, "..-.") == 0) return 'F';
	if (strcmp(arr, "--.") == 0) return 'G';
	if (strcmp(arr, "....") == 0) return 'H';
	if (strcmp(arr, "..") == 0) return 'I';
	if (strcmp(arr, ".---") == 0) return 'J';
	if (strcmp(arr, "-.-") == 0) return 'K';
	if (strcmp(arr, ".-..") == 0) return 'L';
	if (strcmp(arr, "--") == 0) return 'M';
	if (strcmp(arr, "-.") == 0) return 'N';
	if (strcmp(arr, "---") == 0) return 'O';
	if (strcmp(arr, ".--.") == 0) return 'P';
	if (strcmp(arr, "--.-") == 0) return 'Q';
	if (strcmp(arr, ".-.") == 0) return 'R';
	if (strcmp(arr, "...") == 0) return 'S';
	if (strcmp(arr, "-") == 0) return 'T';
	if (strcmp(arr, "..-") == 0) return 'U';
	if (strcmp(arr, "...-") == 0) return 'V';
	if (strcmp(arr, ".--") == 0) return 'W';
	if (strcmp(arr, "-..-") == 0) return 'X';
	if (strcmp(arr, "-.--") == 0) return 'Y';
	if (strcmp(arr, "--..") == 0) return 'Z';
	
	if (strcmp(arr, ".----") == 0) return '1';
	if (strcmp(arr, "..---") == 0) return '2';
	if (strcmp(arr, "...--") == 0) return '3';
	if (strcmp(arr, "....-") == 0) return '4';
	if (strcmp(arr, ".....") == 0) return '5';
	if (strcmp(arr, "-....") == 0) return '6';
	if (strcmp(arr, "--...") == 0) return '7';
	if (strcmp(arr, "---..") == 0) return '8';
	if (strcmp(arr, "----.") == 0) return '9';
	if (strcmp(arr, "-----") == 0) return '0';
	return '?';
}


void Initialize(){

	cli();
	DDRB |= (1<<DDB1); //Configure PB1, PB2, PB4 to output (for LEDs)
	DDRB |= (1<<DDB2);
	DDRB |= (1<<DDB4);
	
	DDRB &= ~(1<<DDB0); //Configure PB0 (ICP1) to be input
	DDRB |= (1<<DDB5); //Configure PB5 to be output
	
	//Timer setup - internal div by 256, results in 62500 ticks per second
	// 1 tick = 0.016 ms
	// This means one full cycle of timer = 2^16/62500 ~= 1s, which is sufficient for morse code
	TCCR1B &= ~(1<<CS10);
	TCCR1B &= ~(1<<CS11);
	TCCR1B |= (1<<CS12);
	
	TCCR1A &= ~(1<<WGM10);
	TCCR1A &= ~(1<<WGM11);
	TCCR1B &= ~(1<<WGM12);
	TCCR1B &= ~(1<<WGM13);
	
	//Looking for rising edge initially
	TCCR1B |= (1<<ICES1);
	
	//Clear interrupt flag
	TIFR1 |= (1<<ICF1);
	
	//Enable input capture interrupt
	TIMSK1 |= (1<<ICIE1);	
	
	sei();
}

ISR(TIMER1_CAPT_vect){
	TIFR1 |= (1<<ICF1); //Clear interrupt flag
	
	if(PINB & (1<<PINB0)){ //If PB0 is high
		
		TIMSK1 &= ~(1<<TOIE1); //Disable timer overflow interrupt
		edge1 = ICR1; //Save value of start edge
		
		space = edge1 + overflow * 65536 - edge2; //duration of space in ticks (edge2 is the timestamp for when the button was last depressed)
		space_ms = (int)((double) space / 62.5); //Converting from ticks to ms
		
		if(space_ms > 1000 && space_ms < 42000){  //Space is defined as > 1000 ms
			isSpace = 1; //isSpace is a boolean that determines whether a space was detected
		}
		overflow = 0; //reset overflow
		
		TIMSK1 |= (1<<TOIE1); //Enable timer overflow interrupt
		TCCR1B &= ~(1<<ICES1); //Now looking for falling edge
	}
	else { //If PB0 is low
		
		TIMSK1 &= ~(1<<TOIE1); //Disable timer overflow interrupt
		
		edge2 = ICR1; //Save value of end edge
		edge2 = edge2 + overflow * 65536;
		duration = edge2 - edge1; //Compute duration of signal (the duration variable is for measuring length of pulses)
		overflow = 0;
		
		TCCR1B |= (1<<ICES1); //Now looking for rising edge
		TIMSK1 |= (1<<TOIE1); //Enable timer overflow interrupt (for measuring spaces)
	}
	
}

ISR(TIMER1_OVF_vect){
	overflow ++; //keeping track of overflows
}

int main(void)
{
	Initialize();
	UART_init(BAUD_PRESCALER);
	
	while(1)
	{
		if(duration != 0){
			
			volatile unsigned long int duration_ms = (int)((double) duration / 62.5); //Converting from ticks to ms

			if(duration_ms > 30 && duration_ms < 200){ //dot is >10ms and <200ms
				PORTB |= (1<<PORTB1); //Set PB1 to high, indicating dot
				_delay_ms(50);
				PORTB &= ~(1<<PORTB1); //Set PB1 to low
				
				letter[counter] = dot; //add dot to char array
				letter[counter + 1] = '\0';
				counter++;
				
			} else if (duration_ms >= 200 && duration_ms < 1000){ //dash is >200ms and <1000ms
				PORTB |= (1<<PORTB2); //Set PB2 to high, indicating dash
				_delay_ms(50);
				PORTB &= ~(1<<PORTB2); //Set PB2 to low
				
				letter[counter] = dash; //add dash to char array
				letter[counter + 1] = '\0';
				counter++;
			} else { //runs if pulse was not within a dot or a dash (usually occurs when button debounces)
				PORTB |= (1<<PORTB4); //Blink red LED to indicate error
				_delay_ms(50);
				PORTB &= ~(1<<PORTB4); 
			}
			
			duration = 0; //reset duration
		}
		
		if(isSpace == 1){ //anytime a space is detected, the char array is decoded and a letter is printed to the monitor
			
			ANS = decode(letter); //decode letter
			
			letter[0] = '\0'; //Clear char array
			letter[1] = '\0'; //Clear char array
			letter[2] = '\0'; //Clear char array
			letter[3] = '\0'; //Clear char array
			letter[4] = '\0'; //Clear char array
			
			counter = 0; //Reset counter
			
	
			sprintf(String,"%c ", ANS);
			UART_putstring(String); //print letter
			
		
			isSpace = 0; //reset bool
		}
	}
}
