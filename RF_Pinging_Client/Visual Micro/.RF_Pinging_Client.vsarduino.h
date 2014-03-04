/* 
	Editor: http://www.visualmicro.com
	        arduino debugger, visual micro +, free forum and wiki
	
	Hardware: Arduino Leonardo, Platform=avr, Package=arduino
*/

#define __AVR_ATmega32u4__
#define __AVR_ATmega32U4__
#define ARDUINO 101
#define ARDUINO_MAIN
#define F_CPU 16000000L
#define __AVR__
#define __cplusplus
extern "C" void __cxa_pure_virtual() {;}

int ledSM_tick( int states );
int pingSM_Tick( int states );
//
//

#include "J:\arduino-1.0.5-r2\hardware\arduino\variants\leonardo\pins_arduino.h" 
#include "J:\arduino-1.0.5-r2\hardware\arduino\cores\arduino\arduino.h"
#include "C:\Users\hao\Documents\Arduino\RF_IPS\RF_Pinging_Client\RF_Pinging_Client.ino"
#include "C:\Users\hao\Documents\Arduino\RF_IPS\RF_Pinging_Client\micros.h"
#include "C:\Users\hao\Documents\Arduino\RF_IPS\RF_Pinging_Client\scheduler.h"
#include "C:\Users\hao\Documents\Arduino\RF_IPS\RF_Pinging_Client\timer.h"
