/*
 * Lab2_ESE3500.c
 *
 * Created: 2023-01-27 2:13:46 PM
 * Author : vicgao
 */ 

#include <avr/io.h>
#include <util/delay.h>
#define F_CPU 16000000UL

void Initialize()
{
	DDRD &= ~(1<<DDD7); //Configure PD7 as input
	DDRB |= (1<<DDB1); //Configure PB1, PB2, PB3, and PB4 to output
	DDRB |= (1<<DDB2);
	DDRB |= (1<<DDB3);
	DDRB |= (1<<DDB4);
}

int main(void)
{
	Initialize();
	
	int i = 0; //
	
    while (1) 
    {
		if (PIND & (1<<PIND7)){ //If value at D7 is high (button depressed)
			i++;
		}
		if (i%4 == 0){
			PORTB |= (1<<PORTB1); //Set PB1 to high
			PORTB &= ~(1<<PORTB4); //Set PB4 to low
		}
		else if(i%4 == 1){
			PORTB |= (1<<PORTB2); //Set PB2 to high
			PORTB &= ~(1<<PORTB1); //Set PB1 to low
		}
		else if(i%4 == 2){
			PORTB |= (1<<PORTB3); //Set PB3 to high
			PORTB &= ~(1<<PORTB2); //Set PB2 to low
		} else {
			PORTB |= (1<<PORTB4); //Set PB4 to high
			PORTB &= ~(1<<PORTB3); //Set PB3 to low
		}
		_delay_ms(5000); //delay for 5s
    }
}

