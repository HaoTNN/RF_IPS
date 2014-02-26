#include <avr/io.h>
#include <avr/interrupt.h>
#include "scheduler.h"
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#include "micros.h"

//////Task declarations///////
task tasks[2];
unsigned char numTasks = 2;

unsigned char ledSM_period = 1;					//in millis
unsigned char pingSM_period = 1;				//

unsigned char periodGCD = 1;					//in millis
//////////////////////////////////

////////Pin assignments///////////
uint8_t ledSM_ledPin = 13;						//pin to light up LED
uint8_t ledSM_buttonPin = 3;					//pin to activate LED
uint8_t pingSM_buttonPin = 4;					//pin to begin send
//////////////////////////////////

//////ledSM variables//////
unsigned long ledSM_timer = 0;					//counting time that LED is on/off
unsigned long ledSM_timerThreshold = 50;		//in millis
///////////////////////////

//////pingSM variables//////
//All times are in millis unless otherwise stated
unsigned long pingSM_recoveryMax = 2000;		//Time to take to attempt to recover from module failure
unsigned long pingSM_timeoutMax = 1000;			//Max time to wait to receive data
unsigned long pingSM_waitingPeriod = 200;		//Time to cooldown before next send
unsigned long pingSM_timeCnt = 0;
const unsigned int payloadSize = sizeof(unsigned long);
unsigned long pingSM_time;
////////////////////////////

//STATES DECLARATIONS//
enum ledSM_states{ main, ledOn, ledOff };
enum pingSM_states{ waiting, getData, sendData };
///////////////////////

//////ledSM State machine function
int ledSM_tick( int states ){
	//////TRANSITIONS
	switch( states ){
		case -1:
		states = main;
		break;
		case main:
		if( digitalRead(ledSM_buttonPin) == 1 ){
			states = ledOn;
		}
		else{
			states = main;
		}
		break;
		case ledOn:
		if( ledSM_timer >= ledSM_timerThreshold ){
			states = ledOff;
			ledSM_timer = 0;
		}
		else{
			states = ledOn;
		}
		break;
		case ledOff:
		if( ledSM_timer >= ledSM_timerThreshold ){
			states = main;
			ledSM_timer = 0;
		}
		else{
			states = ledOff;
		}
		break;
		default:
		break;
	}

	//////ACTIONS
	switch( states ){
		case -1:
		digitalWrite( ledSM_ledPin, LOW );
		break;
		case main:
		digitalWrite( ledSM_ledPin, LOW );
		break;
		case ledOn:
		digitalWrite( ledSM_ledPin, HIGH );
		ledSM_timer++;
		break;
		case ledOff:
		digitalWrite( ledSM_ledPin, LOW );
		ledSM_timer++;
		break;
		default:
		digitalWrite( ledSM_ledPin, LOW );
		break;
	}
	return states;
}

int pingSM_Tick( int states ){
	////TRANSITIONS
	switch( states ){
		case -1:
		states = waiting;
		break;
		case waiting:
		if( !Mirf.isSending() && Mirf.dataReady() ){
			states = getData;
			Serial.print("Getting data... ");
			Mirf.getData((byte*) &pingSM_time);
			Serial.print(" Got: ");
			Serial.print(pingSM_time);
		}
		else{
			states = waiting;
		}
		break;
		case getData:
		states = sendData;
		Serial.print(" Sending: ");
		Serial.print(pingSM_time);
		Mirf.send((byte*) &pingSM_time);
		pingSM_timeCnt = 0;
		break;
		case sendData:
		if( pingSM_timeCnt >= pingSM_timeoutMax && Mirf.isSending() ){
			states = waiting;
			pingSM_timeCnt = 0;
			Serial.println(" ... Error sending");
		}
		else if(Mirf.isSending() && pingSM_timeCnt < pingSM_timeoutMax ){
			states = sendData;
		}
		else{
			states = waiting;
			Serial.println(" ... Sent");
		}
		break;
		default:
		states = waiting;
		break;
	}
	
	////ACTIONS
	switch( states ){
		case -1:
		break;
		case waiting:
		break;
		case getData:
		break;
		case sendData:
		pingSM_timeCnt++;
		break;
		default:
		break;
	}
	return states;
}

#include "timer.h"

void setup()
{
	micros_p_init();
	Serial.begin(9600);
	
	pinMode(ledSM_ledPin, OUTPUT);
	pinMode(ledSM_buttonPin, INPUT);
	pinMode(pingSM_buttonPin, INPUT);
	
	digitalWrite(pingSM_buttonPin, HIGH);		//Enable pull-up resistors
	digitalWrite(ledSM_buttonPin, HIGH);		//Enable pull-up resistors
	
	tasks[0].state = -1;
	tasks[0].elapsedTime = 0;
	tasks[0].period = ledSM_period;
	tasks[0].TickFunc = &ledSM_tick;
	
	tasks[1].state = -1;
	tasks[1].elapsedTime = 0;
	tasks[1].period = pingSM_period;
	tasks[1].TickFunc = &pingSM_Tick;
	
	
	//////NRF24L01+ SETUP//////
	Mirf.csnPin = 6;
	Mirf.cePin = 7;
	
	Mirf.spi = &MirfHardwareSpi;
	Mirf.init();
	Mirf.setRADDR( (byte*) "rec1" );
	Mirf.setTADDR( (byte*) "trans1");
	Mirf.payload = payloadSize;
	Mirf.channel = 90;
	Mirf.config();
	
	Mirf.setPower(RF_0);
	Mirf.setDatarate(RF_2MBPS);

	timerSet(periodGCD);
	timerOn();
}

void loop()
{

}