#ifndef TIMER_H
#define TIMER_H

#include <avr/interrupt.h>

volatile unsigned char timerFlag = 0;

unsigned long _avr_timer_start = 1;
unsigned long _avr_timer_currcnt = 0;

void timerSet(unsigned long M){
	_avr_timer_start = M;
	_avr_timer_currcnt = M;
}

void timerOn(){
	cli();
	TCCR1A = 0x00;	//TCCR1B bit 4:3 and TCCR1A bit 1:0 = 0100 = CTC mode
					// 0000 0000 = 0x00
					
	TCCR1B = 0x09;	//TCCR0B bit 4:3 and TCCR0A bit 1:0 = 0100 = CTC mode
					// bits 2:0 = 001 = no prescaler
					// 0000 1001 = 0x08
					//
					// If 3:0 = 0010, then: 16Mhz = 16 000 000 / 16 = 2 000 000 ticks/s
					// Then TCNT1 counts at 2 000 000 ticks/s

	OCR1A = 16000	;	//Interrupts occur when TCNT1 == OCR1A
					//1us ticks, so 0.000001s * 16 000 000 ticks/s = 16
					//TCNT1 == 16 means 1 us has passed	

	TIMSK1 = 0x02;	//bit 1 = 1
					//OCIE0A = enable compare match interrupt

	TCNT1 = 0;		//Initializing counter

	_avr_timer_currcnt = _avr_timer_start;

	sei();
}

void timerOff(){
	TCCR1A = 0x00;
	TCCR1B = 0x00;
}

void timerISR(){
	unsigned char i;
	for( i = 0; i < numTasks; ++i){
		if( tasks[i].elapsedTime >= tasks[i].period ){
			tasks[i].state = tasks[i].TickFunc(tasks[i].state);
			tasks[i].elapsedTime = 0;
		}
		tasks[i].elapsedTime += periodGCD;
	}
}

ISR(TIMER1_COMPA_vect){
	_avr_timer_currcnt--;
	if( _avr_timer_currcnt == 0 ){
		timerISR();
		_avr_timer_currcnt = _avr_timer_start;
	}
}

#endif