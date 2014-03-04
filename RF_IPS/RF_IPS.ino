#include <avr/io.h>
#include <avr/interrupt.h>
#include "scheduler.h"
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#include <avr/pgmspace.h>

//////Task declarations///////
task tasks[1];
unsigned char numTasks = 1;

unsigned char ledSM_period = 1;
unsigned char pingSM_period = 1;

unsigned char periodGCD = 1;
//////////////////////////////////

//////RF Module Settings//////
const unsigned int payloadSize = sizeof(unsigned long);		//Size of our payload
//////////////////////////////

//////pingSM variables//////
//All times are in millis unless otherwise stated
unsigned long pingSM_recoveryMax = 2000;					//Time to take to attempt to recover from module failure
unsigned long pingSM_timeoutMax = 1000;						//Max time to wait to receive data
unsigned long pingSM_waitingPeriod = 200;					//Time to cooldown before next send
unsigned long pingSM_timeCnt = 0;							//Counter to be used in the pingSM task
unsigned long pingSM_time;
unsigned char pingSM_validCommand = 1;						//Value to check if the receieved command is valid

//Commands to send to stations
//Using PROGMEM to free space SRAM usage
prog_char pingSM_commandStat1[] PROGMEM = "STAT1BEG";
prog_char pingSM_commandStat2[] PROGMEM = "STAT2BEG";
prog_char pingSM_commandACK[] PROGMEM = "ACK";
char pingSM_commandBuffer[12];
////////////////////////////

//STATES DECLARATIONS//
enum pingSM_states{ waiting, checkCommand, getData, sendData };
///////////////////////

int pingSM_Tick( int states ){
	////TRANSITIONS
	switch( states ){
		case -1:
			states = waiting;
			break;
		case waiting:
			if( !Mirf.isSending() && Mirf.dataReady() ){
				states = checkCommand;
				Serial.print("Getting command... ");
				Mirf.getData((byte*) &pingSM_commandBuffer);
				Serial.print(" Got: ");
				Serial.print(pingSM_commandBuffer);
				pingSM_validCommand = strcmp_P( pingSM_commandBuffer, pingSM_commandStat1 );
			}
			else{
				states = waiting;
			}
			break;
		case checkCommand:
			if( pingSM_validCommand ){
				states = waiting;
				Serial.println("Unrecognized command, back to listening");
			}
			else{
				states = getData;
				pingSM_timeCnt = 0;
				strcpy_P( pingSM_commandBuffer, pingSM_commandACK );
				Mirf.send((byte*) &pingSM_commandBuffer );
				Serial.print(" Sending " );
				Serial.print( pingSM_commandBuffer );
				Mirf.payload = sizeof(unsigned long);
				Mirf.setPayload();
			}
			break;
		case getData:
			if( !Mirf.dataReady() && pingSM_timeCnt < pingSM_timeoutMax ){
				states = getData;
			}
			else if( Mirf.dataReady() && pingSM_timeCnt < pingSM_timeoutMax ){
				states = sendData;
				Serial.print(" Sending: ");
				Serial.print(pingSM_time);
				Mirf.send((byte*) &pingSM_time);
				pingSM_timeCnt = 0;
			}
			else{
				states = waiting;
				Serial.println(" Error: timeout listening for data after ACK");
				Mirf.payload = sizeof( pingSM_commandBuffer );
				Mirf.setPayload();
			}
			break;
		case sendData:
			if( pingSM_timeCnt >= pingSM_timeoutMax && Mirf.isSending() ){
				states = waiting;
				Serial.println(" ... Error sending");
				pingSM_timeCnt = 0;
				Mirf.payload = sizeof( pingSM_commandBuffer );
				Mirf.setPayload();
			}
			else if(Mirf.isSending() && pingSM_timeCnt < pingSM_timeoutMax ){
				states = sendData;
			}
			else{
				states = waiting;
				Serial.println(" ... Sent");
				Mirf.payload = sizeof( pingSM_commandBuffer );
				Mirf.setPayload();
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
		case checkCommand:
			break;
		case getData:
			pingSM_timeCnt++;
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
	Serial.begin(9600);
	delay(2000);
	Serial.println("Begin setup");
	
	tasks[0].state = -1;
	tasks[0].elapsedTime = 0;
	tasks[0].period = pingSM_period;
	tasks[0].TickFunc = &pingSM_Tick;
	
	//////NRF24L01+ SETUP//////
	Mirf.csnPin = 6;
	Mirf.cePin = 7;
	
	Mirf.spi = &MirfHardwareSpi;
	Mirf.init();
	Mirf.setRADDR( (byte*) "station1");
	Mirf.setTADDR( (byte*) "tracker");
	Mirf.payload = sizeof(pingSM_commandBuffer);
	Mirf.channel = 25;
	Mirf.config();
	
	Mirf.setPower(RF_0);
	Mirf.setDatarate(RF_2MBPS);

	timerSet(periodGCD);
	timerOn();
	Serial.println("Setup complete");

}

void loop()
{

}