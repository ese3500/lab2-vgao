#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL


void Initialize(){

	cli();
	
	DDRB &= ~(1<<DDB0); //Configure PB0 (ICP1) to be input
	DDRB |= (1<<DDB5); //Configure PB5 to be output
	
	//Timer setup - 2MHz
	TCCR1B &= ~(1<<CS10);
	TCCR1B |= ~(1<<CS11);
	TCCR1B &= ~(1<<CS12);
	
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
		PORTB |= (1<<PORTB5); //Set PB5 to high
		TCCR1B &= ~(1<<ICES1); //Now looking for falling edge
	}
	else { //If PB0 is low
		PORTB &= ~(1<<PORTB5); //Set PB5 to low
		TCCR1B |= (1<<ICES1); //Now looking for rising edge
	}
	
}

int main(void)
{
	Initialize();
	
	while (1)
	{
	}
}
