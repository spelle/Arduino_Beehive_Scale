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


#ifdef DEBUG

	#ifdef SCALE_SIMU

		#define SIMULATIONSINE_NB_STEP 128

		unsigned int simulationSine[SIMULATIONSINE_NB_STEP] = {
				0x600,0x64b,0x697,0x6e1,0x72c,0x775,0x7be,0x805,
				0x84c,0x891,0x8d4,0x916,0x955,0x993,0x9ce,0xa08,
				0xa3e,0xa72,0xaa3,0xad2,0xafd,0xb25,0xb4b,0xb6d,
				0xb8b,0xba6,0xbbe,0xbd2,0xbe2,0xbef,0xbf9,0xbfe,
				0xc00,0xbfe,0xbf9,0xbef,0xbe2,0xbd2,0xbbe,0xba6,
				0xb8b,0xb6d,0xb4b,0xb25,0xafd,0xad2,0xaa3,0xa72,
				0xa3e,0xa08,0x9ce,0x993,0x955,0x916,0x8d4,0x891,
				0x84c,0x805,0x7be,0x775,0x72c,0x6e1,0x697,0x64b,
				0x600,0x5b5,0x569,0x51f,0x4d4,0x48b,0x442,0x3fb,
				0x3b4,0x36f,0x32c,0x2ea,0x2ab,0x26d,0x232,0x1f8,
				0x1c2,0x18e,0x15d,0x12e,0x103,0xdb,0xb5,0x93,
				0x75,0x5a,0x42,0x2e,0x1e,0x11,0x7,0x2,
				0x0,0x2,0x7,0x11,0x1e,0x2e,0x42,0x5a,
				0x75,0x93,0xb5,0xdb,0x103,0x12e,0x15d,0x18e,
				0x1c2,0x1f8,0x232,0x26d,0x2ab,0x2ea,0x32c,0x36f,
				0x3b4,0x3fb,0x442,0x48b,0x4d4,0x51f,0x569,0x5b5,
		};

	#endif


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
		Serial.print(" : ") ;
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
