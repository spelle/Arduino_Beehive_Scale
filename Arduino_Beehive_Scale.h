/*
 * Arduino_Beehive_Scale.h
 *
 *  Created on: Oct 4, 2014
 *      Author: nuptse
 */

#ifndef ARDUINO_BEEHIVE_SCALE_H_
#define ARDUINO_BEEHIVE_SCALE_H_

#include <Arduino.h>
#include "Arduino_Beehive_Scale_MBMap.h"

unsigned int holdingRegs[HOLDING_REGS_SIZE]; // function 3 and 16 register array

#define DEBUG

#ifdef DEBUG

void print_debug( unsigned long ulTime, const char * szMessaqe )
{
	if( 0x0000 != holdingRegs[DEBUG_ENABLE] )
	{
		Serial.print(ulTime) ;
		Serial.println(szMessaqe);
	}
}

void print_debug( unsigned long ulTime, unsigned int ui )
{
	if( 0x0000 != holdingRegs[DEBUG_ENABLE] )
	{
		Serial.print(ulTime) ;
		Serial.println(ui);
	}
}

#undef PDEBUG // Just in case
#ifndef PDEBUG
	#define PDEBUG(time, msg) \
		print_debug( time, msg)
#else
	#define PDEBUG(time, msg) /* nothing */
#endif

#endif // DEBUG

#endif /* ARDUINO_BEEHIVE_SCALE_H_ */
