#include <avr/io.h>
#include <avr/interrupt.h>
#include "scheduler.h"
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#include <avr/pgmspace.h>
unsigned long p1;
unsigned long p2;

#define DEBUG_MODE false

//////Task declarations///////
task tasks[2];
unsigned char numTasks = 2;

unsigned char ledSM_period = 1;					//in millis
unsigned char pingSM_period = 1;				//

unsigned char periodGCD = 1;					//in millis
//////////////////////////////////

////////Pin assignments///////////
uint8_t ledSM_ledPin = 13;						//pin to light up LED
uint8_t ledSM_buttonPin = 4;					//pin to activate LED
uint8_t pingSM_buttonPin = 4;					//pin to begin send

uint8_t testPin = 3;							//testing pin
//////////////////////////////////

//////ledSM variables//////
unsigned long ledSM_timer = 0;					//counting time that LED is on/off
unsigned long ledSM_timerThreshold = 50;		//in millis
///////////////////////////

//////pingSM variables//////
//All times are in millis unless otherwise stated
unsigned long pingSM_recoveryMax = 2000;		//Time to take to attempt to recover from module failure
unsigned long pingSM_timeoutMax = 1000;			//Max time to wait to receive data
unsigned long pingSM_waitingPeriod = 1;			//Time to cooldown before next send
unsigned long pingSM_timeCnt = 0;				//Counter to be used in the pingSM task

unsigned int pingSM_maxSamples = 300;			//Samples to collect to calc. avg round-trip time
unsigned int pingSM_samplesCnt = 0;				//How many samples so far
unsigned int pingSM_failedSample = 0;			//How many failed attempts to send/receive data

unsigned long pingSM_time;						//Holds the round-trip time for each pinging sample
unsigned long pingSM_runningTotal = 0;			//Running total so far for roundtrip times TODO: implement circle queue

unsigned char pingSM_currentStation = 1;
unsigned char pingSM_maxStations = 2;
unsigned long pingSM_averageTime_1 = 0;			//Average round-trip time from station 1
unsigned long pingSM_averageTime_2 = 0;			//Average round-trip time from station 2

//Commands to send to stations
//Using PROGMEM to free space SRAM usage
prog_char pingSM_commandStat1[] PROGMEM = "STAT1BEG";
prog_char pingSM_commandStat2[] PROGMEM = "STAT2BEG";
char pingSM_commandBuffer[12];
////////////////////////////

////RF module Settings////
const unsigned int payloadSize = sizeof(unsigned long);
//////////////////////////

//STATES DECLARATIONS//
enum ledSM_states{ begin, ledOn, ledOff };
enum pingSM_states{ main, sendCommand, listeningAck, bufferState, sendData, listening, getData, cooldown, waitReset };
///////////////////////

//////ledSM State machine function
int ledSM_tick( int states ){
	//////TRANSITIONS
	switch( states ){
		case -1:
		states = begin;
		break;
		case begin:
			if( digitalRead(ledSM_buttonPin) == 1 ){
				states = ledOn;
			}
			else{
				states = begin;
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
				states = begin;
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

//PINGING STATE MACHINE
//
//High level process:
//Upon button press, this station will do the following:
//	1. Try to send command to specified station. If failed, go back to main to wait for button press
//	2. Receive an acknowledgement. If not, go back to main to wait for button press
//	3. While n < pingSM_maxSamples
//		3a. Send timestamp to base station
//		3b. Receive timestamp from base station
//		3c. n++
int pingSM_Tick( int states ){
	////TRANSITIONS
	switch( states ){
		case -1:
			states = main;
			break;
		case main:
			if( !digitalRead( pingSM_buttonPin ) ){									//Waiting on button press
				states = main;
			}
			else{																	//Button has been pressed to start sending
				states = sendCommand;
				strcpy_P( pingSM_commandBuffer, pingSM_commandStat1 );
				Mirf.payload = sizeof( pingSM_commandBuffer );
				Mirf.setPayload();
				Mirf.send((byte*) pingSM_commandBuffer);
				pingSM_timeCnt = 0;
				if( DEBUG_MODE ){
					Serial.print("Sending command ");
					Serial.print(pingSM_commandBuffer);
					Serial.print("...");
				}
			}
			break;
		case sendCommand:
			if( Mirf.isSending() && pingSM_timeCnt < pingSM_timeoutMax ){			//While it's sending and !timeout
				states = sendCommand;
			}
			else if( !Mirf.isSending() && pingSM_timeCnt < pingSM_timeoutMax ){		//Sending is finished and it didn't time out
				states = listeningAck;
				pingSM_timeCnt = 0;
			}
			else{																	//Timed out
				states = main;
				if( DEBUG_MODE ){
					Serial.println("Failed to send command");
				}
			}
			break;
		case listeningAck:
			if( !Mirf.dataReady() && pingSM_timeCnt < pingSM_timeoutMax ){			//While waiting for data and !timeout
				states = listeningAck;
			}
			else if( pingSM_timeCnt >= pingSM_timeoutMax ){							//Timed out
				states = main;
				if( DEBUG_MODE ){
					Serial.println("Error: timeout listening for ACK");
				}
			}
			else{																	//Received an acknowledge and continue
				states = bufferState;
			}
			break;
		case bufferState:
			states = sendData;
			pingSM_time = micros();
			Mirf.payload = sizeof( pingSM_time );
			Mirf.setPayload();
			Mirf.send((byte*) &pingSM_time);
			pingSM_timeCnt = 0;
			if( DEBUG_MODE ){
				Serial.print("Sending: ");
				Serial.print(pingSM_time);
			}
			break;
		case sendData:
			if( pingSM_timeCnt >= pingSM_timeoutMax && Mirf.isSending() ){
				states = bufferState;
				pingSM_timeCnt = 0;
				pingSM_samplesCnt++;
				pingSM_failedSample++;
				if( DEBUG_MODE ){
					Serial.print(" ... Error sending");
				}
			}
			else if( Mirf.isSending() && pingSM_timeCnt < pingSM_timeoutMax ){
				states = sendData;
			}
			else{
				states = listening;
				pingSM_timeCnt = 0;
			}
			break;
		case listening:
			if( (Mirf.isSending() || !Mirf.dataReady()) && pingSM_timeCnt < pingSM_timeoutMax ){
				states = listening;
			}
			else if( !Mirf.isSending() && Mirf.dataReady() && pingSM_timeCnt < pingSM_timeoutMax ){
				states = getData;
			}
			else{
				states = bufferState;
				pingSM_samplesCnt++;
				pingSM_failedSample++;
				if( DEBUG_MODE ){
					Serial.println(" ... Error: timeout listening for data.");
				}
			}
			break;
		case getData:
			states = cooldown;
			pingSM_timeCnt = 0;
			pingSM_samplesCnt++;
			break;
		case cooldown:
			if( pingSM_timeCnt < pingSM_waitingPeriod ){
				states = cooldown;
			}
			else if( pingSM_timeCnt >= pingSM_waitingPeriod && pingSM_samplesCnt < pingSM_maxSamples ){
				states = bufferState;
			}
			else if( pingSM_timeCnt >= pingSM_waitingPeriod && pingSM_samplesCnt >= pingSM_maxSamples ){
				states = waitReset;
				pingSM_runningTotal /= (pingSM_maxSamples - pingSM_failedSample);
				if( DEBUG_MODE ){
					Serial.print(" .. From ");
					Serial.print(pingSM_currentStation);
					Serial.print(" Avg: ");
					Serial.print( pingSM_runningTotal );
					Serial.print(" ..Failed: ");
					Serial.println( pingSM_failedSample );
				}
			}
			break;
		case waitReset:
			if( digitalRead( pingSM_buttonPin ) ){
				states = waitReset;
			}
			else{
				states = main;
				pingSM_samplesCnt = 0;
				pingSM_failedSample = 0;
				if( ++pingSM_currentStation == pingSM_maxStations ){
					pingSM_currentStation = 1;
			}
			if( pingSM_currentStation == 1 ){
				Mirf.setTADDR( (byte*) "stat1");
			}
			else{
				Mirf.setTADDR( (byte*) "stat2");
			}
				Mirf.payload = sizeof(pingSM_commandBuffer);
				Mirf.setPayload();
			}
			break;
		default:
			states = main;
			break;
	}
	
	////ACTIONS
	switch( states ){
		case -1:
			break;
		case main:
			break;
		case sendCommand:
			break;
		case listeningAck:
			pingSM_timeCnt++;
			break;
		case bufferState:
			break;
		case sendData:
			pingSM_timeCnt++;
			break;
		case listening:
			pingSM_timeCnt++;
			break;
		case getData:
			Mirf.getData((byte*) &pingSM_time);
			p1 = micros();
			p2 = micros() - pingSM_time;
			pingSM_runningTotal += p2;
			if( DEBUG_MODE ){
				Serial.print(" ... Current time = ");
				Serial.print( p1 );
				Serial.print(" ... Roundtrip: ");
				Serial.println( p2 );
			}
			break;
		case cooldown:
			pingSM_timeCnt++;
			break;
		case waitReset:
			break;
		default:
			break;
	}
	return states;
}

#include "timer.h"

void setup()
{
	Serial.begin(9600);
	delay(500);
	
	pinMode(ledSM_ledPin, OUTPUT);
	pinMode(ledSM_buttonPin, INPUT);
	pinMode(pingSM_buttonPin, INPUT);
	pinMode(testPin, INPUT);
	
	digitalWrite(pingSM_buttonPin, HIGH);		//Enable pull-up resistors
	digitalWrite(ledSM_buttonPin, HIGH);		//Enable pull-up resistors
	digitalWrite(testPin, HIGH);
	
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
	Mirf.setTADDR( (byte*) "station1" );
	Mirf.setRADDR( (byte*) "tracker");
	Mirf.payload = sizeof(pingSM_commandBuffer);
	Mirf.channel = 25;
	Mirf.config();
	
	Mirf.setPower(RF_0);
	Mirf.setDatarate(RF_2MBPS);
	
	timerSet(periodGCD);
	timerOn();
	
}

void loop()
{

}