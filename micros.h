#ifndef	MICROS_H
#define	MICROS_H

#include <avr/interrupt.h>

volatile unsigned long microseconds = 0;
volatile unsigned int overflowCnt = 0;

unsigned long micros_p(){
	return microseconds;
}

void micros_p_init(){
	cli();
	TCCR3A = 0x00;	//TCCR1B bit 4:3 and TCCR1A bit 1:0 = 0100 = CTC mode
		// 0000 0000 = 0x00
	
	TCCR3B = 0x0A;	//TCCR0B bit 4:3 and TCCR0A bit 1:0 = 0100 = CTC mode
		// bits 2:0 = 010 = prescaler = 8
		// 0000 1010 = 0x0A
		//
		// If 3:0 = 0010, then: 16Mhz = 16 000 000 / 16 = 2 000 000 ticks/s
		// Then TCNT1 counts at 2 000 000 ticks/s

	TIMSK3 = 0x01;	//bit 0 = 1
		//TOIE3 - Interrupt when TCNT3 overflows

	TCNT3 = 65534;		//Initializing counter
	sei();
}

ISR(TIMER3_OVF_vect){
	++overflowCnt;
	if( overflowCnt > 1){
		++microseconds;
		overflowCnt = 0;
	}
	TCNT3 = 65534;
	TIFR3 = 0x00;
}


#endif